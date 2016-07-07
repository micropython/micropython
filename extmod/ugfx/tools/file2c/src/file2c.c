/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef WIN32
	#include <io.h>
#endif

static unsigned char	buf[1024];
static char				tname[FILENAME_MAX];

static char *filenameof(char *fname) {
	char *p;

#ifdef WIN32
	if (fname[1] == ':')
		fname = fname+2;
	p = strrchr(fname, '\\');
	if (p) fname = p+1;
#endif
	p = strrchr(fname, '/');
	if (p) fname = p+1;
	return fname;
}

static char *basenameof(char *fname) {
	char *p;

	fname = filenameof(fname);
	p = strchr(fname, '.');
	if (p) *p = 0;
	return fname;
}

static char *clean4c(char *fname) {
	char *p;

	while((p = strpbrk(fname, "-+ `~!@#$%^&*(){}[]|:;'\",<>?/|=.\\"))) *p = '_';
	return fname;
}

int main(int argc, char * argv[])
{
char *		opt_progname;
char *		opt_inputfile;
char *		opt_outputfile;
char *		opt_arrayname;
char *		opt_dirname;
int			opt_breakblocks;
int			opt_romdir;
char *		opt_static;
char *		opt_const;
FILE *		f_input;
FILE *		f_output;
unsigned	blocknum;
size_t		len, totallen;
size_t		i;

	/* Default values for our parameters */
	opt_progname = basenameof(argv[0]);
	opt_inputfile = opt_outputfile = opt_arrayname = opt_dirname = 0;
	opt_breakblocks = opt_romdir = 0;
	opt_static = opt_const = "";

	/* Read the arguments */
	while(*++argv) {
		if (argv[0][0] == '-') {
			while (*++(argv[0])) {
				switch(argv[0][0]) {
				case '?': case 'h':										goto usage;
				case 'd':		opt_romdir = 1;							break;
				case 'b':		opt_breakblocks = 1;					break;
				case 'c':		opt_const = "const ";					break;
				case 's':		opt_static = "static ";					break;
				case 'n':		opt_arrayname = *++argv;				goto nextarg;
				case 'f':		opt_romdir = 1; opt_dirname = *++argv;	goto nextarg;
				default:
					fprintf(stderr, "Unknown flag -%c\n", argv[0][0]);
					goto usage;
				}
			}
		} else if (!opt_inputfile)
			opt_inputfile = argv[0];
		else if (!opt_outputfile)
			opt_outputfile = argv[0];
		else {
			usage:
			fprintf(stderr, "Usage:\n\n%s -?\n"
							"%s [-dbcs] [-n name] [-f file] [inputfile] [outputfile]\n"
							"\t-?\tThis help\n"
							"\t-h\tThis help\n"
							"\t-d\tAdd a directory entry for the ROM file system\n"
							"\t-b\tBreak the arrays for compilers that won't handle large arrays\n"
							"\t-c\tDeclare as const (useful to ensure they end up in Flash)\n"
							"\t-s\tDeclare as static\n"
							"\t-n name\tUse \"name\" as the name of the array\n"
							"\t-f file\tUse \"file\" as the filename in the ROM directory entry\n"
					, opt_progname, opt_progname);
			return 1;
		}
	nextarg:	;
	}

	/* Make sure we can generate a default directory name if required */
	if (opt_romdir && !opt_dirname && !opt_inputfile) {
		fprintf(stderr, "If using -d you must either specify an input filename or use -f to specify a directory entry filename\n");
		goto usage;
	}

	/* Open the input file */
	if (opt_inputfile) {
		f_input = fopen(opt_inputfile,
#ifdef WIN32
			"rb");
#else
			"r");
#endif
		if (!f_input) {
			fprintf(stderr, "Could not open input file '%s'\n", opt_inputfile);
			goto usage;
		}
	} else {
		f_input = stdin;
#ifdef WIN32
		_setmode(_fileno(stdin), _O_BINARY);
#endif
	}

	/* Open the output file */
	if (opt_outputfile) {
		f_output = fopen(opt_outputfile, "w");
		if (!f_output) {
			fprintf(stderr, "Could not open output file '%s'\n", opt_outputfile);
			goto usage;
		}
	} else
		f_output = stdout;

	/* Print the comment header */
	fprintf(f_output, "/**\n * This file was generated ");
	if (opt_inputfile) fprintf(f_output, "from \"%s\" ", opt_inputfile);
	fprintf(f_output, "using...\n *\n *\t%s", opt_progname);
	if (opt_arrayname || opt_static[0] || opt_const[0] || opt_breakblocks || opt_romdir) {
		fprintf(f_output, " -");
		if (opt_romdir)  fprintf(f_output, "d");
		if (opt_breakblocks) fprintf(f_output, "b");
		if (opt_const[0]) fprintf(f_output, "c");
		if (opt_static[0]) fprintf(f_output, "s");
		if (opt_arrayname) fprintf(f_output, "n %s", opt_arrayname);
		if (opt_dirname) fprintf(f_output, (opt_arrayname ? " -f %s" : "f %s"), opt_dirname);
	}
	if (opt_inputfile) fprintf(f_output, " %s", opt_inputfile);
	if (opt_outputfile) fprintf(f_output, " %s", opt_outputfile);
	fprintf(f_output, "\n *\n */\n");

	/*
	 * Set the array name and dir name
	 */
	if (!opt_arrayname) {
		if (opt_inputfile)
			opt_arrayname = basenameof(strcpy(tname, opt_inputfile));
		if (!opt_arrayname || !opt_arrayname[0]) {
			srand(time(0));
			sprintf(tname, "filearray%u", rand());
			opt_arrayname = tname;
		}
	}
	opt_arrayname = clean4c(opt_arrayname);
	if (opt_romdir && !opt_dirname)
		opt_dirname = filenameof(opt_inputfile);

	/* Read the file processing 1K at a time */
	blocknum = 0;
	totallen = 0;
	while((len = fread(buf, 1, sizeof(buf), f_input))) {
		if (!blocknum++)
			fprintf(f_output, "%s%schar %s[] = {", opt_static, opt_const, opt_arrayname);
		else if (opt_breakblocks)
			fprintf(f_output, "\n};\n%s%schar %s_p%u[] = {", opt_static, opt_const, opt_arrayname, blocknum);
		for(i = 0; i < len; i++) {
			fprintf(f_output, (i & 0x0F) ? " 0x%02X," : "\n\t0x%02X,", buf[i]);
		}
		totallen += len;
	}
	fprintf(f_output, "\n};\n");

	/* Add the directory entry if required */
	if (opt_romdir) {
		fprintf(f_output, "\n#ifdef ROMFS_DIRENTRY_HEAD\n");
		fprintf(f_output, "\t%s%sROMFS_DIRENTRY %s_dir = { 0, 0, ROMFS_DIRENTRY_HEAD, \"%s\", %u, %s };\n", opt_static, opt_const, opt_arrayname, opt_dirname, totallen, opt_arrayname);
		fprintf(f_output, "\t#undef ROMFS_DIRENTRY_HEAD\n\t#define ROMFS_DIRENTRY_HEAD &%s_dir\n#endif\n", opt_arrayname);
	}

	/* Clean up */
	if (ferror(f_input))
		fprintf(stderr, "Input file read error\n");
	if (ferror(f_output))
		fprintf(stderr, "Output file write error - disk full?\n");
	if (f_input != stdin)
		fclose(f_input);
	if (f_output != stdout)
		fclose(f_output);

	return 0;
}

