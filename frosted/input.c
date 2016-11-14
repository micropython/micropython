/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "py/mpstate.h"
#include "py/mphal.h"
#include "input.h"

#if MICROPY_USE_READLINE == 1
#include "lib/mp-readline/readline.h"
#elif MICROPY_USE_READLINE == 2
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/tilde.h>
#endif
static char lastcmd[128] = "";

char *readline_tty(char *input, int size, char *prompt)
{

    while (2>1)
    {
        int len = 0, pos = 0;
        char got[5];
        int i, j;
        int repeat = 0;

        while(len < size)
        {
            const char del = 0x08;
            const char space = 0x20;
            int ret = read(STDIN_FILENO, got, 4);
            if (ret < 0) {
                printf("read: %s\r\n", strerror(errno));
                fputs(prompt, stdout);
                fflush(stdout);
                continue;
            }

            /* arrows */
            if ((ret == 3) && (got[0] == 0x1b)) {
                char dir = got[2];
                if (dir == 'A') {
                    if ((strlen(lastcmd) == 0) || repeat) {
                        continue;
                    }
                    repeat++;

                    while (len > 0) {
                        write(STDOUT_FILENO, &del, 1);
                        write(STDOUT_FILENO, &space, 1);
                        write(STDOUT_FILENO, &del, 1);
                        len--;
                    }
                    len = strlen(lastcmd);
                    lastcmd[len] = 0x00;
                    len--;
                    lastcmd[len] = 0x00;
                    pos = len;
                    printf( "%s", lastcmd);
                    fflush(stdout);
                    strcpy(input, lastcmd);
                    continue;
                } else if (dir == 'B') {
                } else if (dir == 'C') {
                    if (pos < len) {
                        printf("%c", input[pos++]);
                        fflush(stdout);
                    }
                } else if (dir == 'D') {
                    write(STDOUT_FILENO, &del, 1);
                    pos--;
                    continue;
                }
            }

            if (ret > 3) {
            	if ((got[0] == 0x1B) && (got[2] == 0x33) && (got[3] == 0x7E)) {
                	if (pos < len) {
	                    //write(STDOUT_FILENO, &del, 1);
        	            //printf( " ");
                	    //write(STDOUT_FILENO, &del, 1);
                	    pos--;
        	            len--;
                	    if (pos < len) {
                    	for ( i = pos+1; i < len; i++) {
	                	input[i] = input[i+1];
                        write(STDOUT_FILENO, &input[i], 1);
                	}
                    write(STDOUT_FILENO, &space, 1);
	                i = len - pos;
        	        while (i > 0) {
                		write(STDOUT_FILENO, &del, 1);
                    		i--;
                    	}

	                    } else {
	                    input[pos] = 0x00;
	                    pos--;
	                    len--;
	            }

                    continue;
                }
            	}
                continue;
            }
            if ((ret > 0) && (got[0] >= 0x20) && (got[0] <= 0x7e)) {
                for (i = 0; i < ret; i++) {
                    /* Echo to terminal */
                    if (got[i] >= 0x20 && got[i] <= 0x7e)
                        write(STDOUT_FILENO, &got[i], 1);
                	if (pos < len) {
                		for (j = len + 1; j > pos; j--) {
                			input[j] = input[j-1];
                		}
                		input[pos] = got[i];
                    	for ( j = pos + 1; j < len +1; j++) {
                            write(STDOUT_FILENO, &input[j], 1);
                    	}
                        write(STDOUT_FILENO, &input[i], 1);
                    	j = len - pos + 1;
                    	while (j > 0) {
                    		write(STDOUT_FILENO, &del, 1);
                    		j--;
                    	}
                	} else {
                		input[pos] = got[i];
                	}

                    len++;
                    pos++;
                }
            }

            if ((got[0] == 0x0D)) {
            	input[len] = 0x0D;
                input[len + 1] = '\0';
                printf( "\r\n");
                fflush(stdout);
                if (len > 0)
                    strncpy(lastcmd, input, 128);
                return input; /* CR (\r\n) */
            }

            if ((got[0] == 0x4)) {
                printf( "\r\n");
                fflush(stdout);
                len = 0;
                pos = 0;
                break;
            }

            /* tab */
            if ((got[0] == 0x09)) {
                memset(input + len, ' ', 4);
                printf("    ");
                pos+=4;
                len+=4;
                fflush(stdout);
                continue;
            }

            /* backspace */
            if ((got[0] == 0x7F) || (got[0] == 0x08)) {
                if (pos > 0) {
                    write(STDOUT_FILENO, &del, 1);
                    write(STDOUT_FILENO, &space, 1);
                    write(STDOUT_FILENO, &del, 1);
                    pos--;
                    len--;
                    if (pos < len) {
                    	for ( i = pos; i < len; i++) {
                    		input[i] = input[i+1];
                            write(STDOUT_FILENO, &input[i], 1);
                    	}
                        write(STDOUT_FILENO, &space, 1);
                    	i = len - pos + 1;
                    	while (i > 0) {
                    		write(STDOUT_FILENO, &del, 1);
                    		i--;
                    	}

                    } else {
	                    input[pos] = 0x00;
	            }

                    continue;
                }
            }
        }
        printf("\r\n");
        fflush(stdout);
        if (len < 0)
            return NULL;

        input[len + 1] = '\0';
    }
    return input;
}

char *prompt(char *p) {
    static char buf[256];
    char *s;
    int l;
    fputs(p, stdout);
    fflush(stdout);

    s = readline_tty(buf, sizeof(buf), p);
    if (!s) {
        return NULL;
    }
    l = strlen(buf);
    if ((buf[l - 1] == '\n'|| buf[l - 1] == '\r')) {
        buf[l - 1] = 0;
    } else {
        l++;
    }
    char *line = malloc(l);
    memcpy(line, buf, l);
    return line;
}

void prompt_read_history(void) {
#if MICROPY_USE_READLINE_HISTORY
    #if MICROPY_USE_READLINE == 1
    readline_init0(); // will clear history pointers
    char *home = getenv("HOME");
    if (home != NULL) {
        vstr_t vstr;
        vstr_init(&vstr, 50);
        vstr_printf(&vstr, "%s/.micropython.history", home);
        int fd = open(vstr_null_terminated_str(&vstr), O_RDONLY);
        if (fd != -1) {
            vstr_reset(&vstr);
            for (;;) {
                char c;
                int sz = read(fd, &c, 1);
                if (sz < 0) {
                    break;
                }
                if (sz == 0 || c == '\n') {
                    readline_push_history(vstr_null_terminated_str(&vstr));
                    if (sz == 0) {
                        break;
                    }
                    vstr_reset(&vstr);
                } else {
                    vstr_add_byte(&vstr, c);
                }
            }
            close(fd);
        }
        vstr_clear(&vstr);
    }
    #elif MICROPY_USE_READLINE == 2
    read_history(tilde_expand("~/.micropython.history"));
    #endif
#endif
}

void prompt_write_history(void) {
#if MICROPY_USE_READLINE_HISTORY
    #if MICROPY_USE_READLINE == 1
    char *home = getenv("HOME");
    if (home != NULL) {
        vstr_t vstr;
        vstr_init(&vstr, 50);
        vstr_printf(&vstr, "%s/.micropython.history", home);
        int fd = open(vstr_null_terminated_str(&vstr), O_CREAT | O_TRUNC | O_WRONLY, 0644);
        if (fd != -1) {
            for (int i = MP_ARRAY_SIZE(MP_STATE_PORT(readline_hist)) - 1; i >= 0; i--) {
                const char *line = MP_STATE_PORT(readline_hist)[i];
                if (line != NULL) {
                    int res;
                    res = write(fd, line, strlen(line));
                    res = write(fd, "\r\n", 1);
                    (void)res;
                }
            }
            close(fd);
        }
    }
    #elif MICROPY_USE_READLINE == 2
    write_history(tilde_expand("~/.micropython.history"));
    #endif
#endif
}

STATIC mp_obj_t mp_builtin_input(size_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        mp_obj_print(args[0], PRINT_STR);
    }

    char *line = prompt("");
    if (line == NULL) {
        nlr_raise(mp_obj_new_exception(&mp_type_EOFError));
    }
    mp_obj_t o = mp_obj_new_str(line, strlen(line), false);
    free(line);
    return o;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_input_obj, 0, 1, mp_builtin_input);
