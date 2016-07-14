// Class to store the data of a font while it is being processed.
// This class can be safely cloned using the default copy constructor.

#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <map>

namespace mcufont
{

class DataFile
{
public:
    typedef std::vector<uint8_t> pixels_t;
    
    struct dictentry_t
    {
        pixels_t replacement; // The expanded version of this block.
        int score; // Number of bytes that having this entry saves.
        bool ref_encode; // Encode using references to other dictionary entries.
        
        dictentry_t(): score(0), ref_encode(false) {}
    };
    
    struct glyphentry_t
    {
        pixels_t data; // The full data of the glyph.
        std::vector<int> chars; // Characters that this glyph represents.
        int width; // Tracking width of the character.
    };
    
    struct fontinfo_t
    {
        std::string name; // Name of the typeface
        int max_width; // Width of the character bounding box.
        int max_height; // Height of the character bounding box.
        int baseline_x; // X coordinate (from left) of the baseline.
        int baseline_y; // Y coordinate (from top) of the baseline.
        int line_height; // Line height (vertical advance).
        int flags;
    };
    
    static const int FLAG_MONOSPACE = 0x01;
    static const int FLAG_BW = 0x02;
    
    // Construct from data in memory.
    DataFile(const std::vector<dictentry_t> &dictionary,
             const std::vector<glyphentry_t> &glyphs,
             const fontinfo_t &fontinfo);
    
    // Save to a file (custom format)
    void Save(std::ostream &file) const;

    // Load from a file (custom format)
    // Returns nullptr if load fails.
    static std::unique_ptr<DataFile> Load(std::istream &file);
    
    // Get or set an entry in the dictionary. The size of the dictionary
    // is constant. Entries 0 to 23 are reserved for special purposes.
    static const size_t dictionarysize = 256 - 24;
    const dictentry_t &GetDictionaryEntry(size_t index) const
        { return m_dictionary.at(index); }
    void SetDictionaryEntry(size_t index, const dictentry_t &value);
    const std::vector<dictentry_t> &GetDictionary() const
        { return m_dictionary; }
        
    // Get the index of the dictionary entry with the lowest score.
    size_t GetLowScoreIndex() const
        { return m_lowscoreindex; }
    
    // Get an entry in the glyph table.
    size_t GetGlyphCount() const
        { return m_glyphtable.size(); }
    const glyphentry_t &GetGlyphEntry(size_t index) const
        { return m_glyphtable.at(index); }
    const std::vector<glyphentry_t> &GetGlyphTable() const
        { return m_glyphtable; }
    
    // Create a map of char indices to glyph indices
    std::map<size_t, size_t> GetCharToGlyphMap() const;
    
    // Get the information that applies to all glyphs.
    const fontinfo_t &GetFontInfo() const
        { return m_fontinfo; }
    
    // Show a glyph as text.
    std::string GlyphToText(size_t index) const;
    
    // Get the random generator seed
    // The seed is stored in the datafile to get deterministic behaviour
    // for debugging.
    uint32_t GetSeed() const { return m_seed; }
    void SetSeed(uint32_t seed) { m_seed = seed; }
    
private:
    std::vector<dictentry_t> m_dictionary;
    std::vector<glyphentry_t> m_glyphtable;
    fontinfo_t m_fontinfo;
    uint32_t m_seed;
    
    size_t m_lowscoreindex;
    
    void UpdateLowScoreIndex();
};

std::ostream& operator<<(std::ostream& os, const DataFile::pixels_t& str);
std::istream& operator>>(std::istream& is, DataFile::pixels_t& str);

}

#ifdef CXXTEST_RUNNING
#include <cxxtest/TestSuite.h>

using namespace mcufont;

class DataFileTests: public CxxTest::TestSuite
{
public:
    void testFileLoad()
    {
        std::istringstream s(testfile);
        std::unique_ptr<DataFile> f = DataFile::Load(s);
        
        TS_ASSERT_EQUALS(f->GetFontInfo().name, "Sans Serif");
        TS_ASSERT_EQUALS(f->GetFontInfo().max_width, 4);
        TS_ASSERT_EQUALS(f->GetFontInfo().max_height, 6);
        TS_ASSERT_EQUALS(f->GetDictionaryEntry(0).score, 5);
        TS_ASSERT_EQUALS(f->GetDictionaryEntry(1).score, 13);
        TS_ASSERT_EQUALS(f->GetGlyphCount(), 3);
        
        DataFile::pixels_t expected = {
            0,15,0,15, 0,15,0,15, 0,15,0,15, 0,15,0,15, 0,15,0,15, 0,15,0,15
        };
        TS_ASSERT_EQUALS(f->GetGlyphEntry(0).data.size(), 24);
        TS_ASSERT(f->GetGlyphEntry(0).data == expected);
    }
    
    void testFileSave()
    {
        std::istringstream is1(testfile);
        std::unique_ptr<DataFile> f1 = DataFile::Load(is1);
        
        std::ostringstream os;
        f1->Save(os);
        
        std::string text = os.str();
        std::istringstream is2(text);
        std::unique_ptr<DataFile> f2 = DataFile::Load(is2);
        
        TS_ASSERT_EQUALS(f1->GetFontInfo().name, f2->GetFontInfo().name);
        TS_ASSERT(f1->GetGlyphEntry(0).data == f2->GetGlyphEntry(0).data);
    }
    
private:
    static constexpr const char *testfile =
        "Version 1\n"
        "FontName Sans Serif\n"
        "MaxWidth 4\n"
        "MaxHeight 6\n"
        "BaselineX 1\n"
        "BaselineY 1\n"
        "DictEntry 5 0 0F0F0\n"
        "DictEntry 13 0 F0F0F0\n"
        "DictEntry 1 0 F0F0F0\n"
        "Glyph 1,2,3 4 0F0F0F0F0F0F0F0F0F0F0F0F\n"
        "Glyph 4 4 0F0F0F0F0F0F0F0F0F0F0F0F\n"
        "Glyph 5 4 0F0F0F0F0F0F0F0F0F0F0F0F\n";
};

#endif
