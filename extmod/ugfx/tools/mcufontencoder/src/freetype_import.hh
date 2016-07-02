// Function for importing any font supported by libfreetype.

#pragma once
#include "datafile.hh"

namespace mcufont {

std::unique_ptr<DataFile> LoadFreetype(std::istream &file, int size, bool bw);

}
