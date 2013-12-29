import sys
import os
import os.path
import datetime
import argparse
from xml.etree.ElementTree import Element, SubElement, tostring

from log import Log
from texparser import TexParser
from latexparser import LatexParser
from gettexfile import file_has_suffix
from gettexfile import get_tex_file

from xiwi.common.misc import buildFileList
from xiwi.common import arxivid
from xiwi.common.stats import Statistics

def str_contains(s1, s2):
    return s1.find(s2) != -1

def str_contains_one_of(st, st_list):
    for st2 in st_list:
        if str_contains(st, st2):
            return True
    return False

def detect_file_kind(file_obj):
    """Simple detection of kind of source file."""
    kind = 'unknown'
    firstline = file_obj.readline()
    while firstline.isspace():
        firstline = file_obj.readline()
    if firstline.startswith('%!PS'):
        kind = 'PS'
    elif firstline.startswith('%auto-ignore'):
        kind = 'auto-ignore'
    else:
        file_obj.seek(0)
        for line in file_obj:
            if str_contains(line, '\\def'):
                # might be tex, if we don't find anything else
                kind = 'tex'
            if str_contains(line, '\\input'):
                # might be tex, if we don't find anything else
                kind = 'tex'
                if str_contains(line, 'amstex') or str_contains(line, 'harvmac'):
                    # definitely tex
                    kind = 'tex'
                    break
            if str_contains(line, '\\documentclass'):
                # definitely latex
                kind = 'latex'
                break
            if str_contains(line, '\\documentstyle'):
                # could be tex or latex
                if str_contains(line, 'amsppt'):
                    kind = 'tex'
                    break
                else:
                    kind = 'latex'
                    break
    file_obj.seek(0)
    return kind

class WithdrawnPaper(object):
    def __init__(self):
        pass

    def __getitem__(self, item):
        if item == 'refs':
            return []
        elif item == 'success':
            return True

    def parse(self):
        pass

def process_article(filename):
    """Returns TexParserBase derived object on success, None on failure."""

    # get the tex file
    filename, file_obj, tarfile_obj = get_tex_file(filename)
    if file_obj is None:
        return None

    # detect the type of file
    kind = detect_file_kind(file_obj)

    # act on the type of file
    parser = None
    if kind == 'PS':
        print('skipping postscript file')
    elif kind == 'auto-ignore':
        print('asked to ignore file, most likely it was withdrawn')
        parser = WithdrawnPaper()
    if kind == 'tex':
        print('parsing as TeX')
        parser = TexParser(filename, file_obj, tarfile_obj)
    elif kind == 'latex':
        print('parsing as LaTeX')
        parser = LatexParser(filename, file_obj, tarfile_obj)
    else:
        print('cannot determine kind of file')

    # attempt to parse the document
    try:
        if parser is not None:
            parser.parse()
    except Exception as e:
        print('exception while trying to parse file:')
        print(str(e))
        parser = None

    # close the files
    file_obj.close()
    if tarfile_obj is not None:
        tarfile_obj.close()

    # return the parsed document
    return parser

arxiv_classes = [
    'acc-phys', 'adap-org', 'alg-geom', 'ao-sci', 'astro-ph', 'atom-ph',
    'bayes-an', 'chao-dyn', 'chem-ph', 'cmp-lg', 'comp-gas', 'cond-mat',
    'cs', 'dg-ga', 'funct-an', 'gr-qc', 'hep-ex', 'hep-lat',
    'hep-ph', 'hep-th', 'math', 'math-ph', 'mtrl-th', 'nlin',
    'nucl-ex', 'nucl-th', 'patt-sol', 'physics', 'plasm-ph', 'q-alg',
    'q-bio', 'quant-ph', 'solv-int', 'supr-con'
]

def do_single_file(file_name, print_xml, write_xml_dir):
    arxiv_id, arxiv_version = arxivid.filenameToArxivAndVersion(file_name)
    if arxiv_id is None:
        print('WARN: could not determine arXiv identifier for', file_name)
        arxiv_id = '<unknown>'
        arxiv_version = 0

    Log.reset()
    Statistics.begin_item(arxiv_id)

    if file_has_suffix(file_name, '.pdf'):
        Statistics.count('1) pdf')
        succ = True
    else:
        Statistics.count('2) processed')

        parser = process_article(file_name)

        if parser is not None :
            succ = parser['success']
            bib_refs = parser['refs']
        else :
            succ = False
            bib_refs = []

        if str_contains_one_of(arxiv_id, ['gr-qc', 'hep-']):
            Statistics.count('hep-processed')
            if succ:
                Statistics.count('hep-success')
        if succ:
            print('-success--------')
            Statistics.count('3) success')
        else:
            print('-fail-----------')
            Statistics.count('4) fail')

        show_ref = False

        if succ and show_ref:
            for bib_ref in bib_refs:
                print(bib_ref.key, 'with', bib_ref.cite_count, 'citations in paper')
                if len(bib_ref.bib_info) == 0:
                    print('no reference')
                else:
                    print(bib_ref.bib_info_as_str(keep_comments=True))

        if succ and (print_xml or write_xml_dir):
            xml = Element('article')
            SubElement(xml, 'id').text = arxiv_id
            if arxiv_version > 0:
                SubElement(xml, 'version').text = str(arxiv_version)
            refs = SubElement(xml, 'refs')
            for bib_ref in bib_refs:
                bib_text = bib_ref.bib_info_as_str(keep_comments=True)
                if len(bib_text) != 0:
                    ncites = bib_ref.cite_count
                    if ncites < 1:
                        ncites = 1
                    ref = SubElement(refs, 'ref', order=str(bib_ref.ref_order_num), freq=str(ncites))
                    ref.text = bib_text
            if print_xml:
                print(tostring(xml))
            if isinstance(write_xml_dir, str):
                if arxiv_id != '<unknown>':
                    xml_file_name = os.path.join(write_xml_dir, arxiv_id.replace('/', '') + '.xml')
                else:
                    fname = os.path.split(file_name)[1]
                    if fname.rfind('.') > 0:
                        fname = fname[:fname.rfind('.')]
                    xml_file_name = write_xml_dir + '/' + fname + '.xml'
                file_obj = open(xml_file_name, 'wb')
                file_obj.write(tostring(xml, encoding='utf-8'))
                file_obj.close()

    Statistics.end_item()

    return succ

summaryStrs = []

if __name__ == "__main__":
    cmd_parser = argparse.ArgumentParser(description='Parse TeX/LaTeX to find references.')
    cmd_parser.add_argument('--filelist', action='store_true', help='file names on the command line each contain a list of files to process')
    cmd_parser.add_argument('--print-xml', action='store_true', help='print XML output to stdout')
    cmd_parser.add_argument('--write-xml', metavar='<dir>', help='destination directory to write XML output files')
    cmd_parser.add_argument('--failed', metavar='<file>', help='output file to write list of failed files')
    cmd_parser.add_argument('files', nargs='+', help='input files')
    args = cmd_parser.parse_args()

    # print date stamp
    timeStart = datetime.datetime.now()
    print('[ptex] started processing at', str(timeStart))

    print('given', len(args.files), 'files, first file:', args.files[0])
    print('================')

    Statistics.clear('article')

    # build list of files to process
    file_list = buildFileList(args.filelist, args.files)

    # ensure the destination directory exists
    if args.write_xml is not None and os.path.exists(args.write_xml):
        try:
            os.makedirs(args.write_xml)
        except:
            pass

    # process the files
    failed_files = []
    for file_name in file_list:
        success = do_single_file(file_name, args.print_xml, args.write_xml)
        if not success:
            failed_files.append(file_name)

    # write the failed files to an output file, if requested
    if args.failed is not None:
        file_obj = open(args.failed, 'w')
        file_obj.writelines(f + '\n' for f in failed_files)
        file_obj.close()

    print('================')
    Statistics.show()
    Statistics.show_detail('fail')
    #Statistics.show_detail('cite-range')
    #Statistics.show_detail('bad-ascii')
    #Statistics.show_detail('non-ascii')

    print('================')

    # print date stamp
    timeEnd = datetime.datetime.now()
    print('[ptex] finished processing at', str(timeEnd))

    # print summary for email
    summaryStrs.extend(Statistics.get_summary())
    summaryStrs.insert(0, 'started processing at %s, took %.1f minutes' % (timeStart.strftime('%H:%M'), (timeEnd - timeStart).total_seconds() / 60))
    for s in summaryStrs:
        print('**SUMMARY** [ptex]', s)
