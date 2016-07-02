// Utility functions for exporting to C source code files.

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include "datafile.hh"

namespace mcufont {
    
// Convert a file name to a valid C identifier
std::string filename_to_identifier(std::string name);

// Write a vector of integers as line-wrapped hex/integer data for initializing const array.
void wordwrap_vector(std::ostream &out, const std::vector<unsigned> &data,
                     const std::string &prefix, size_t width = 2);

// Write a vector of integers as a C constant array of given datatype.
void write_const_table(std::ostream &out, const std::vector<unsigned> &data,
                       const std::string &datatype, const std::string &tablename,
                       size_t width = 2);

// Get minimum tracking width of font
int get_min_x_advance(const DataFile &datafile);

// Get maximum tracking width of font
int get_max_x_advance(const DataFile &datafile);

// Select the character to use as a fallback.
int select_fallback_char(const DataFile &datafile);

// Structure to represent one consecutive range of characters.
struct char_range_t
{
    uint16_t first_char;
    uint16_t char_count;
    std::vector<int> glyph_indices;
    
    char_range_t(): first_char(0), char_count(0) {}
};

// Decide how to best divide the characters in the font into ranges.
// Limitations are:
//  - Gaps longer than minimum_gap should result in separate ranges.
//  - Each range can have encoded data size of at most maximum_size.
std::vector<char_range_t> compute_char_ranges(const DataFile &datafile,
    std::function<size_t(size_t)> get_encoded_glyph_size,
    size_t maximum_size,
    size_t minimum_gap);

}