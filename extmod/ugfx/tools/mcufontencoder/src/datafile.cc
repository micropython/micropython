#include "datafile.hh"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "ccfixes.hh"

#define DATAFILE_FORMAT_VERSION 1

namespace mcufont {

DataFile::DataFile(const std::vector<dictentry_t> &dictionary,
                   const std::vector<glyphentry_t> &glyphs,
                   const fontinfo_t &fontinfo):
    m_dictionary(dictionary), m_glyphtable(glyphs), m_fontinfo(fontinfo)
{
    dictentry_t dummy = {};
    while (m_dictionary.size() < dictionarysize)
        m_dictionary.push_back(dummy);
    
    UpdateLowScoreIndex();
}

void DataFile::Save(std::ostream &file) const
{
    file << "Version " << DATAFILE_FORMAT_VERSION << std::endl;
    file << "FontName " << m_fontinfo.name << std::endl;
    file << "MaxWidth " << m_fontinfo.max_width << std::endl;
    file << "MaxHeight " << m_fontinfo.max_height << std::endl;
    file << "BaselineX " << m_fontinfo.baseline_x << std::endl;
    file << "BaselineY " << m_fontinfo.baseline_y << std::endl;
    file << "LineHeight " << m_fontinfo.line_height << std::endl;
    file << "Flags " << m_fontinfo.flags << std::endl;
    file << "RandomSeed " << m_seed << std::endl;
    
    for (const dictentry_t &d : m_dictionary)
    {
        if (d.replacement.size() != 0)
        {
            file << "DictEntry " << d.score << " ";
            file << d.ref_encode << " " << d.replacement << std::endl;
        }
    }
    
    for (const glyphentry_t &g : m_glyphtable)
    {
        file << "Glyph ";
        for (size_t i = 0; i < g.chars.size(); i++)
        {
            if (i != 0) file << ',';
            file << g.chars.at(i);
        }
        file << " " << g.width << " " << g.data << std::endl;
    }
}

std::unique_ptr<DataFile> DataFile::Load(std::istream &file)
{
    fontinfo_t fontinfo = {};
    std::vector<dictentry_t> dictionary;
    std::vector<glyphentry_t> glyphtable;
    uint32_t seed = 1234;
    int version = -1;
    
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream input(line);
        std::string tag;
        
        input >> tag;
        
        if (tag == "Version")
        {
            input >> version;
        }
        else if (tag == "FontName")
        {
            while (std::isspace(input.peek())) input.get();
            std::getline(input, fontinfo.name);
        }
        else if (tag == "MaxWidth")
        {
            input >> fontinfo.max_width;
        }
        else if (tag == "MaxHeight")
        {
            input >> fontinfo.max_height;
        }
        else if (tag == "BaselineX")
        {
            input >> fontinfo.baseline_x;
        }
        else if (tag == "BaselineY")
        {
            input >> fontinfo.baseline_y;
        }
        else if (tag == "LineHeight")
        {
            input >> fontinfo.line_height;
        }
        else if (tag == "RandomSeed")
        {
            input >> seed;
        }
        else if (tag == "Flags")
        {
            input >> fontinfo.flags;
        }
        else if (tag == "DictEntry" && dictionary.size() < dictionarysize)
        {
            dictentry_t d = {};
            input >> d.score >> d.ref_encode >> d.replacement;
            dictionary.push_back(d);
        }
        else if (tag == "Glyph")
        {
            glyphentry_t g = {};
            std::string chars;
            input >> chars >> g.width >> g.data;
            
            if ((int)g.data.size() != fontinfo.max_width * fontinfo.max_height)
                throw std::runtime_error("wrong glyph data length: " + std::to_string(g.data.size()));
            
            size_t pos = 0;
            while (pos < chars.size()) {
                size_t p;
                g.chars.push_back(std::stoi(chars.substr(pos), &p));
                pos += p + 1;
            }
            
            glyphtable.push_back(g);
        }
    }
    
    if (version != DATAFILE_FORMAT_VERSION)
    {
        return std::unique_ptr<DataFile>(nullptr);
    }
    
    std::unique_ptr<DataFile> result(new DataFile(dictionary, glyphtable, fontinfo));
    result->SetSeed(seed);
    return result;
}

void DataFile::SetDictionaryEntry(size_t index, const dictentry_t &value)
{
    m_dictionary.at(index) = value;
    
    if (index == m_lowscoreindex ||
        m_dictionary.at(m_lowscoreindex).score > value.score)
    {
        UpdateLowScoreIndex();
    }
}

std::map<size_t, size_t> DataFile::GetCharToGlyphMap() const
{
    std::map<size_t, size_t> char_to_glyph;
    
    for (size_t i = 0; i < m_glyphtable.size(); i++)
    {
        for (size_t c: m_glyphtable[i].chars)
        {
            char_to_glyph[c] = i;
        }
    }
    
    return char_to_glyph;
}

std::string DataFile::GlyphToText(size_t index) const
{
    std::ostringstream os;
    
    const char glyphchars[] = "....,,,,----XXXX";
    
    for (int y = 0; y < m_fontinfo.max_height; y++)
    {
        for (int x = 0; x < m_fontinfo.max_width; x++)
        {
            size_t pos = y * m_fontinfo.max_width + x;
            os << glyphchars[m_glyphtable.at(index).data.at(pos)];
        }
        os << std::endl;
    }
    
    return os.str();
}

void DataFile::UpdateLowScoreIndex()
{
    auto comparison = [](const dictentry_t &a, const dictentry_t &b)
    {
        return a.score < b.score;
    };
    
    auto iter = std::min_element(m_dictionary.begin(),
                                 m_dictionary.end(),
                                 comparison);
    
    m_lowscoreindex = iter - m_dictionary.begin();
}

std::ostream& operator<<(std::ostream& os, const DataFile::pixels_t& str)
{
    for (uint8_t p: str)
    {
        if (p <= 9)
            os << (char)(p + '0');
        else if (p <= 15)
            os << (char)(p - 10 + 'A');
        else
            throw std::logic_error("invalid pixel alpha: " + std::to_string(p));
    }
    return os;
}

std::istream& operator>>(std::istream& is, DataFile::pixels_t& str)
{
    char c;
    str.clear();
    
    while (isspace(is.peek())) is.get();
    
    while (is.get(c))
    {
        if (c >= '0' && c <= '9')
            str.push_back(c - '0');
        else if (c >= 'A' && c <= 'F')
            str.push_back(c - 'A' + 10);
        else
            break;
    }
    
    return is;
}

}
