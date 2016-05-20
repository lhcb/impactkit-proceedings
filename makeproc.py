#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   makeproc.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   11.05.2016
# =============================================================================
"""Make the proceedings book."""

import os
import glob

from PyPDF2 import PdfFileWriter, PdfFileReader

CONTRIB_DIRS = 'contrib-*'
SINGLE_TEMPLATE = 'latex/standalone.tex'
CHAPTER_TEMPLATE = 'pter.tex'


def build_single_pdf(dir_):
    """Build PDF for a single contribution.

    Uses pandoc getting the `md` and `yaml` files in the
    given folder. It will use the first Markdown file name
    as base name for the PDF.

    Arguments:
        dir_ (str): Directory of the contribution.

    Returns:
        str: Path of the PDF output, empty if none was produced.

    Raises:
        OSError: If the call to `pandoc` fails.

    """
    prev_cwd = os.getcwd()
    dir_ = os.path.abspath(dir_)
    os.chdir('latex')
    md_files = [os.path.abspath(file_)
                for file_ in glob.glob(os.path.join(dir_,
                                                    '*.md'))]
    if not md_files:
        os.chdir(prev_cwd)
        return ''
    yaml_files = [os.path.abspath(file_)
                  for file_ in glob.glob(os.path.join(dir_,
                                                      '*.yaml'))]
    out_file = os.path.abspath(os.path.splitext(md_files[0])[0] + '.pdf')
    # Build single PDF
    ret = os.system('pandoc -o {output} {yaml} {md} '
                    '--listings '
                    '--template="{template}"'.format(output=out_file,
                                                     yaml=' '.join(yaml_files),
                                                     md=' '.join(md_files),
                                                     template=os.path.join(prev_cwd,
                                                                           SINGLE_TEMPLATE)))
    os.chdir(prev_cwd)
    if ret:
        raise OSError("Pandoc error")
    return out_file


def merge_pdfs(output_file, pdf_files, overwrite=True):
    """Merge PDFs taking into account that each of them needs to start in an odd page.

    Arguments:
        output_file (str): File to write the output to.
        pdf_files (list[str]): List of PDFs to merge.
        overwrite (bool, optional): Overwrite `output_file`? Defaults to True.

    Returns:
        bool: If a new file was written.

    Raises:
        OSError: If an input file is missing.

    """
    if not overwrite and os.path.exists(output_file):
        return False
    output = PdfFileWriter()
    output_page_number = 0
    alignment = 2
    for filename in pdf_files:
        if not os.path.exists(filename):
            raise OSError(filename)
        input_pdf = PdfFileReader(open(filename, 'rb'))
        for page_num in [input_pdf.getPage(i)
                         for i in range(0, input_pdf.getNumPages())]:
            # This code is executed for every input page in turn
            output.addPage(page_num)
            output_page_number += 1
        while output_page_number % alignment != 0:
            output.addBlankPage()
            output_page_number += 1
    output.write(open(output_file, 'wb'))
    return True


def make_proceedings():
    """Make the proceedings.

    Create individual PDFs and merge them.

    Raises:
        ValueError: If merging fails.

    """
    pdfs = []
    for dir_name in glob.glob(CONTRIB_DIRS):
        pdfs.append(build_single_pdf(dir_name))
    try:
        if not merge_pdfs('output.pdf', pdfs):
            raise ValueError("cannot merge files!")
    except OSError, excp:
        raise ValueError("Cannot merge files due to missing input -> %s",
                         excp.message)

if __name__ == "__main__":
    make_proceedings()

# EOF
