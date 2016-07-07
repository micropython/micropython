#include "exporttools.hh"
#include <iomanip>
#include <set>

namespace mcufont {
    
    
// Convert a file name to a valid C identifier
std::string filename_to_identifier(std::string name)
{
    // If the name contains path separators (/ or \), take only the last part.
    size_t pos = name.find_last_of("/\\");
    if (pos != std::string::npos)
        name = name.substr(pos + 1);
    
    // If the name contains a file extension, strip it.
    pos = name.find_first_of(".");
    if (pos != std::string::npos)
        name = name.substr(0, pos);
    
    // Replace any special characters with _.
    for (pos = 0; pos < name.size(); pos++)
    {
        if (!isalnum(name.at(pos)))
            name.at(pos) = '_';
    }
    
    return name;
}

// Write a vector of integers as line-wrapped hex/integer data for initializing const array.
void wordwrap_vector(std::ostream &out, const std::vector<unsigned> &data,
                     const std::string &prefix, size_t width)
{
    int values_per_column = (width <= 2) ? 16 : 8;
    
    std::ios::fmtflags flags(out.flags());
    out << prefix;
    out << std::hex << std::setfill('0');
    for (size_t i = 0; i < data.size(); i++)
    {
        if (i % values_per_column == 0 && i != 0)
            out << std::endl << prefix;
        
        out << "0x" << std::setw(width) << (int)data.at(i) << ", ";
    }
    out.flags(flags);
}

// Write a vector of integers as a C constant array of given datatype.
 void write_const_table(std::ostream &out, const std::vector<unsigned> &data,
                        const std::string &datatype, const std::string &tablename,
                        size_t width)
{
    out << "static const " << datatype << " " << tablename;
    out << "[" << data.size() << "] = {" << std::endl;
    wordwrap_vector(out, data, "    ", width);
    out << std::endl << "};" << std::endl;
    out << std::endl;
}    

int get_min_x_advance(const DataFile &datafile)
{
    int min = datafile.GetGlyphEntry(0).width;
    
    for (const DataFile::glyphentry_t &g : datafile.GetGlyphTable())
    {
        if (min > g.width)
            min = g.width;
    }
    
    return min;
}

int get_max_x_advance(const DataFile &datafile)
{
    int max = 0;
    
    for (const DataFile::glyphentry_t &g : datafile.GetGlyphTable())
    {
        if (max < g.width)
            max = g.width;
    }
    
    return max;
}

// Select the character to use as a fallback.
int select_fallback_char(const DataFile &datafile)
{
    std::set<int> chars;
    
    size_t i = 0;
    for (const DataFile::glyphentry_t &g: datafile.GetGlyphTable())
    {
        for (size_t c: g.chars)
        {
            chars.insert(c);
        }
        i++;
    }
    
    if (chars.count(0xFFFD))
        return 0xFFFD; // Unicode replacement character
    
    if (chars.count(0))
        return 0; // Used by many BDF fonts as replacement char
    
    if (chars.count('?'))
        return '?';
    
    return ' ';
}

// Decide how to best divide the characters in the font into ranges.
// Limitations are:
//  - Gaps longer than minimum_gap should result in separate ranges.
//  - Each range can have encoded data size of at most maximum_size.
std::vector<char_range_t> compute_char_ranges(const DataFile &datafile,
    std::function<size_t(size_t)> get_encoded_glyph_size,
    size_t maximum_size,
    size_t minimum_gap)
{
    std::vector<char_range_t> result;
    std::map<size_t, size_t> char_to_glyph = datafile.GetCharToGlyphMap();
    std::vector<size_t> chars;
    
    // Get list of all characters in numeric order.
    for (auto iter : char_to_glyph)
        chars.push_back(iter.first);
    
    // Pick out ranges until we have processed all characters
    size_t i = 0;
    while (i < chars.size())
    {
        char_range_t range;
        range.first_char = chars.at(i);
        
        // Find the point where there is a gap larger than minimum_gap.
        i++;
        while (i < chars.size() && chars.at(i) - chars.at(i - 1) < minimum_gap)
            i++;
        
        uint16_t last_char = chars.at(i - 1);
        
        // Then store the indices of glyphs for each character
        size_t data_length = 0;
        for (size_t j = range.first_char; j <= last_char; j++)
        {
            if (char_to_glyph.count(j) == 0)
            {
                // Missing character
                range.glyph_indices.push_back(-1);
                continue;
            }
            
            int glyph_index = char_to_glyph[j];
            
            // Monitor the amount of the data in the range and split it
            // if it grows too large.
            data_length += get_encoded_glyph_size(glyph_index);
            if (data_length > maximum_size)
            {
                last_char = j - 1;
                
                // Return the rest of characters to be processed by next range.
                while (chars.at(i-1) > last_char)
                    i--;
                
                break;
            }
            
            range.glyph_indices.push_back(glyph_index);
        }
        
        range.char_count = last_char - range.first_char + 1;
        result.push_back(range);
    }
    
    return result;
}
    
    
}
