// Write out the encoded data in C source code files for mf_bwfont format.

#pragma once

#include "datafile.hh"
#include <iostream>

namespace mcufont {
namespace bwfont {

void write_header(std::ostream &out, std::string name, const DataFile &datafile);

void write_source(std::ostream &out, std::string name, const DataFile &datafile);

} }

