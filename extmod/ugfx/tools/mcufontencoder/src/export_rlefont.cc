#include "export_rlefont.hh"
#include <vector>
#include <iomanip>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <cctype>
#include "exporttools.hh"
#include "ccfixes.hh"

#define RLEFONT_FORMAT_VERSION 4

namespace mcufont {
namespace rlefont {

// Encode the dictionary entries and the offsets to them.
// Generates tables dictionary_data and dictionary_offsets.
static void encode_dictionary(std::ostream &out,
                              const std::string &name,
                              const DataFile &datafile,
                              const encoded_font_t &encoded)
{
    std::vector<unsigned> offsets;
    std::vector<unsigned> data;
    for (const encoded_font_t::rlestring_t &r : encoded.rle_dictionary)
    {
        offsets.push_back(data.size());
        data.insert(data.end(), r.begin(), r.end());
    }
    
    for (const encoded_font_t::refstring_t &r : encoded.ref_dictionary)
    {
        offsets.push_back(data.size());
        data.insert(data.end(), r.begin(), r.end());
    }
    offsets.push_back(data.size());
    
    write_const_table(out, data, "uint8_t", "mf_rlefont_" + name + "_dictionary_data");
    write_const_table(out, offsets, "uint16_t", "mf_rlefont_" + name + "_dictionary_offsets", 4);
}

// Encode the data tables for a single character range.
// Generates tables glyph_data_i and glyph_offsets_i.
static void encode_character_range(std::ostream &out,
                              const std::string &name,
                              const DataFile &datafile,
                              const encoded_font_t& encoded,
                              const char_range_t& range,
                              unsigned range_index)
{
    std::vector<unsigned> offsets;
    std::vector<unsigned> data;
    std::map<size_t, unsigned> already_encoded;
    
    for (int glyph_index : range.glyph_indices)
    {
        if (already_encoded.count(glyph_index))
        {
            offsets.push_back(already_encoded[glyph_index]);
        }
        else
        {
            encoded_font_t::refstring_t r;
            int width = 0;
            
            if (glyph_index >= 0)
            {
                r = encoded.glyphs[glyph_index];
                width = datafile.GetGlyphEntry(glyph_index).width;
            }
            
            offsets.push_back(data.size());
            already_encoded[glyph_index] = data.size();
            
            data.push_back(width);
            data.insert(data.end(), r.begin(), r.end());
        }
    }
    
    write_const_table(out, data, "uint8_t", "mf_rlefont_" + name + "_glyph_data_" + std::to_string(range_index));
    write_const_table(out, offsets, "uint16_t", "mf_rlefont_" + name + "_glyph_offsets_" + std::to_string(range_index), 4);
}

void write_source(std::ostream &out, std::string name, const DataFile &datafile)
{
    name = filename_to_identifier(name);
    std::unique_ptr<encoded_font_t> encoded = encode_font(datafile, false);
    
    out << std::endl;
    out << std::endl;
    out << "/* Start of automatically generated font definition for " << name << ". */" << std::endl;
    out << std::endl;
    
    out << "#ifndef MF_RLEFONT_INTERNALS" << std::endl;
    out << "#define MF_RLEFONT_INTERNALS" << std::endl;
    out << "#endif" << std::endl;
    out << "#include \"mf_rlefont.h\"" << std::endl;
    out << std::endl;
    
    out << "#ifndef MF_RLEFONT_VERSION_" << RLEFONT_FORMAT_VERSION << "_SUPPORTED" << std::endl;
    out << "#error The font file is not compatible with this version of mcufont." << std::endl;
    out << "#endif" << std::endl;
    out << std::endl;
    
    // Write out the dictionary entries
    encode_dictionary(out, name, datafile, *encoded);
    
    // Split the characters into ranges
    auto get_glyph_size = [&encoded](size_t i)
    {
        return encoded->glyphs[i].size() + 1; // +1 byte for glyph width
    };
    std::vector<char_range_t> ranges = compute_char_ranges(datafile,
        get_glyph_size, 65536, 16);

    // Write out glyph data for character ranges
    for (size_t i = 0; i < ranges.size(); i++)
    {
        encode_character_range(out, name, datafile, *encoded, ranges.at(i), i);
    }
    
    // Write out a table describing the character ranges
    out << "static const struct mf_rlefont_char_range_s mf_rlefont_" << name << "_char_ranges[] = {" << std::endl;
    for (size_t i = 0; i < ranges.size(); i++)
    {
        out << "    {" << ranges.at(i).first_char
            << ", " << ranges.at(i).char_count
            << ", mf_rlefont_" << name << "_glyph_offsets_" << i
            << ", mf_rlefont_" << name << "_glyph_data_" << i << "}," << std::endl; 
    }
    out << "};" << std::endl;
    out << std::endl;
    
    // Pull it all together in the rlefont_s structure.
    out << "const struct mf_rlefont_s mf_rlefont_" << name << " = {" << std::endl;
    out << "    {" << std::endl;
    out << "    " << "\"" << datafile.GetFontInfo().name << "\"," << std::endl;
    out << "    " << "\"" << name << "\"," << std::endl;
    out << "    " << datafile.GetFontInfo().max_width << ", /* width */" << std::endl;
    out << "    " << datafile.GetFontInfo().max_height << ", /* height */" << std::endl;
    out << "    " << get_min_x_advance(datafile) << ", /* min x advance */" << std::endl;
    out << "    " << get_max_x_advance(datafile) << ", /* max x advance */" << std::endl;
    out << "    " << datafile.GetFontInfo().baseline_x << ", /* baseline x */" << std::endl;
    out << "    " << datafile.GetFontInfo().baseline_y << ", /* baseline y */" << std::endl;
    out << "    " << datafile.GetFontInfo().line_height << ", /* line height */" << std::endl;
    out << "    " << datafile.GetFontInfo().flags << ", /* flags */" << std::endl;
    out << "    " << select_fallback_char(datafile) << ", /* fallback character */" << std::endl;
    out << "    " << "&mf_rlefont_character_width," << std::endl;
    out << "    " << "&mf_rlefont_render_character," << std::endl;
    out << "    }," << std::endl;
    
    out << "    " << RLEFONT_FORMAT_VERSION << ", /* version */" << std::endl;
    out << "    " << "mf_rlefont_" << name << "_dictionary_data," << std::endl;
    out << "    " << "mf_rlefont_" << name << "_dictionary_offsets," << std::endl;
    out << "    " << encoded->rle_dictionary.size() << ", /* rle dict count */" << std::endl;
    out << "    " << encoded->ref_dictionary.size() + encoded->rle_dictionary.size() << ", /* total dict count */" << std::endl;
    out << "    " << ranges.size() << ", /* char range count */" << std::endl;
    out << "    " << "mf_rlefont_" << name << "_char_ranges," << std::endl;
    out << "};" << std::endl;
    
    // Write the font lookup structure
    out << std::endl;
    out << "#ifdef MF_INCLUDED_FONTS" << std::endl;
    out << "/* List entry for searching fonts by name. */" << std::endl;
    out << "static const struct mf_font_list_s mf_rlefont_" << name << "_listentry = {" << std::endl;
    out << "    MF_INCLUDED_FONTS," << std::endl;
    out << "    (struct mf_font_s*)&mf_rlefont_" << name << std::endl;
    out << "};" << std::endl;
    out << "#undef MF_INCLUDED_FONTS" << std::endl;
    out << "#define MF_INCLUDED_FONTS (&mf_rlefont_" << name << "_listentry)" << std::endl;
    out << "#endif" << std::endl;
    
    out << std::endl;
    out << std::endl;
    out << "/* End of automatically generated font definition for " << name << ". */" << std::endl;
    out << std::endl;
}

}}

