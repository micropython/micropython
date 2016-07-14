// Given a dictionary and glyphs, encode the data for all the glyphs.

#pragma once

#include "datafile.hh"
#include <vector>
#include <memory>

namespace mcufont {
namespace rlefont {

struct encoded_font_t
{
    // RLE-encoded format for storing the dictionary entries.
    // Each item is a byte. Top bit means the value in the original bitstream,
    // and the bottom 7 bits store the repetition count.
    typedef std::vector<uint8_t> rlestring_t;
    
    // Reference encoded format for storing the glyphs.
    // Each item is a reference to the dictionary.
    // Values 0 and 1 are hardcoded to mean 0 and 1.
    // All other values mean dictionary entry at (i-2).
    typedef std::vector<uint8_t> refstring_t;
    
    std::vector<rlestring_t> rle_dictionary;
    std::vector<refstring_t> ref_dictionary;
    std::vector<refstring_t> glyphs;
};

// Encode all the glyphs.
std::unique_ptr<encoded_font_t> encode_font(const DataFile &datafile,
                                            bool fast = true);

// Sum up the total size of the encoded glyphs + dictionary.
size_t get_encoded_size(const encoded_font_t &encoded);

inline size_t get_encoded_size(const DataFile &datafile, bool fast = true)
{
    std::unique_ptr<encoded_font_t> e = encode_font(datafile, fast);
    return get_encoded_size(*e);
}

// Decode a single glyph (for verification).
std::unique_ptr<DataFile::pixels_t> decode_glyph(
    const encoded_font_t &encoded,
    const encoded_font_t::refstring_t &refstring,
    const DataFile::fontinfo_t &fontinfo);

// Decode a single glyph (for verification).
std::unique_ptr<DataFile::pixels_t> decode_glyph(
    const encoded_font_t &encoded, size_t index,
    const DataFile::fontinfo_t &fontinfo);

}}


#ifdef CXXTEST_RUNNING
#include <cxxtest/TestSuite.h>

using namespace mcufont;
using namespace mcufont::rlefont;

class RLEFontEncodeTests: public CxxTest::TestSuite
{
public:
    void testEncode()
    {
        std::istringstream s(testfile);
        std::unique_ptr<DataFile> f = DataFile::Load(s);
        std::unique_ptr<encoded_font_t> e = encode_font(*f, false);
        
        TS_ASSERT_EQUALS(e->glyphs.size(), 3);
        
        // Expected values for dictionary
        encoded_font_t::rlestring_t dict0 = {0x01, 0xCE, 0x01, 0xCE};
        encoded_font_t::rlestring_t dict1 = {0x0C};
        encoded_font_t::rlestring_t dict2 = {0xFE};
        encoded_font_t::refstring_t dict3 = {24, 24};
        
        TS_ASSERT(e->rle_dictionary.at(0) == dict0);
        TS_ASSERT(e->rle_dictionary.at(1) == dict1);
        TS_ASSERT(e->rle_dictionary.at(2) == dict2);
        TS_ASSERT(e->ref_dictionary.at(0) == dict3);
        
        // Expected values for glyphs
        encoded_font_t::refstring_t glyph0 = {27, 27, 27};
        encoded_font_t::refstring_t glyph1 = {24, 0, 132, 25, 14};
        encoded_font_t::refstring_t glyph2 = {228, 26, 244, 14, 14, 14, 228, 26, 16};
        
        TS_ASSERT_EQUALS(e->glyphs.at(0), glyph0);
        TS_ASSERT_EQUALS(e->glyphs.at(1), glyph1);
        TS_ASSERT_EQUALS(e->glyphs.at(2), glyph2);
    }
    
    void testDecode()
    {
        std::istringstream s(testfile);
        std::unique_ptr<DataFile> f = DataFile::Load(s);
        std::unique_ptr<encoded_font_t> e = encode_font(*f, false);
        
        for (size_t i = 0; i < 3; i++)
        {
            std::unique_ptr<DataFile::pixels_t> dec;
            dec = decode_glyph(*e, i, f->GetFontInfo());
            
            TS_ASSERT_EQUALS(*dec, f->GetGlyphEntry(i).data);
        }
    }
    
private:
    static constexpr const char *testfile =
        "Version 1\n"
        "FontName Sans Serif\n"
        "MaxWidth 4\n"
        "MaxHeight 6\n"
        "BaselineX 1\n"
        "BaselineY 1\n"
        "DictEntry 1 0 0E0E\n"
        "DictEntry 1 0 000000000000\n"
        "DictEntry 1 0 EEEE\n"
        "DictEntry 1 1 0E0E0E0E\n"
        "Glyph 0 4 0E0E0E0E0E0E0E0E0E0E0E0E\n"
        "Glyph 1 4 0E0E0000000000000000000E\n"
        "Glyph 2 4 0000EEEE000EEE0000EEEE00\n";
};
#endif
