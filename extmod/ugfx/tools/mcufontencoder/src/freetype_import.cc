#include "freetype_import.hh"
#include "importtools.hh"
#include <map>
#include <string>
#include <stdexcept>
#include <iostream>
#include "ccfixes.hh"

#include <ft2build.h>
#include FT_FREETYPE_H

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  std::make_pair( e, s ),
#define FT_ERROR_START_LIST static const std::map<FT_Error, std::string> ft_errors {
#define FT_ERROR_END_LIST };    
#include FT_ERRORS_H

namespace mcufont {

static void checkFT(FT_Error error)
{
    if (error != 0)
    {
        if (ft_errors.count(error))
            throw std::runtime_error("libfreetype error " +
                std::to_string(error) + ": " + ft_errors.at(error));
        else
            throw std::runtime_error("unknown libfreetype error " +
                std::to_string(error));
    }
}

// Automatically allocated & freed wrapper for FT_Library
class _FT_Library
{
public:
    _FT_Library() { checkFT(FT_Init_FreeType(&m_lib)); }
    ~_FT_Library() { checkFT(FT_Done_FreeType(m_lib)); }
    operator FT_Library() { return m_lib; }

private:
    FT_Library m_lib;
};

// Automatically allocated & freed wrapper for FT_Face
class _FT_Face
{
public:
    _FT_Face(FT_Library lib, const std::vector<char> &data)
    {
        checkFT(FT_New_Memory_Face(lib, (const unsigned char *)&data[0],
                                   data.size(), 0, &m_face));
    }
    ~_FT_Face() { checkFT(FT_Done_Face(m_face)); }
    operator FT_Face() { return m_face; }
    FT_Face operator->() { return m_face; }

private:
    FT_Face m_face;
};

// Read all the data from a file into a memory buffer.
static void readfile(std::istream &file, std::vector<char> &data)
{
    while (file.good())
    {
        const size_t blocksize = 4096;
        size_t oldsize = data.size();
        data.resize(oldsize + blocksize);
        file.read(&data[oldsize], blocksize);
        data.resize(oldsize + file.gcount());
    }
}

std::unique_ptr<DataFile> LoadFreetype(std::istream &file, int size, bool bw)
{
    std::vector<char> data;
    readfile(file, data);
    
    _FT_Library lib;
    _FT_Face face(lib, data);
    
    checkFT(FT_Set_Pixel_Sizes(face, size, size));
    
    DataFile::fontinfo_t fontinfo = {};
    std::vector<DataFile::glyphentry_t> glyphtable;
    std::vector<DataFile::dictentry_t> dictionary;
   
    // Convert size to pixels and round to nearest.
    int u_per_em = face->units_per_EM;
    auto topx = [size, u_per_em](int s) { return (s * size + u_per_em / 2) / u_per_em; };
    
    fontinfo.name = std::string(face->family_name) + " " +
                    std::string(face->style_name) + " " +
                    std::to_string(size);
    
    // Reserve 4 pixels on each side for antialiasing + hinting.
    // They will be cropped off later.
    fontinfo.max_width = topx(face->bbox.xMax - face->bbox.xMin) + 8;
    fontinfo.max_height = topx(face->bbox.yMax - face->bbox.yMin) + 8;
    fontinfo.baseline_x = topx(-face->bbox.xMin) + 4;
    fontinfo.baseline_y = topx(face->bbox.yMax) + 4;
    fontinfo.line_height = topx(face->height);
    
    FT_Int32 loadmode = FT_LOAD_TARGET_NORMAL | FT_LOAD_RENDER;
    
    if (bw)
        loadmode = FT_LOAD_TARGET_MONO | FT_LOAD_MONOCHROME | FT_LOAD_RENDER;
    
    FT_ULong charcode;
    FT_UInt gindex;
    charcode = FT_Get_First_Char(face, &gindex);
    while (gindex)
    {
        try
        {
            checkFT(FT_Load_Glyph(face, gindex, loadmode));
        }
        catch (std::runtime_error &e)
        {
            std::cerr << "Skipping glyph " << gindex << ": " << e.what() << std::endl;        
            charcode = FT_Get_Next_Char(face, charcode, &gindex);
        }
        
        DataFile::glyphentry_t glyph;
        glyph.width = (face->glyph->advance.x + 32) / 64;
        glyph.chars.push_back(charcode);
        glyph.data.resize(fontinfo.max_width * fontinfo.max_height);
        
        int w = face->glyph->bitmap.width;
        int dw = fontinfo.max_width;
        int dx = fontinfo.baseline_x + face->glyph->bitmap_left;
        int dy = fontinfo.baseline_y - face->glyph->bitmap_top;
        
        /* Some combining diacritics seem to exceed the bounding box.
         * We don't support them all that well anyway, so just move
         * them inside the box in order not to crash.. */
        if (dy < 0)
            dy = 0;
        if (dy + face->glyph->bitmap.rows > fontinfo.max_height)
            dy = fontinfo.max_height - face->glyph->bitmap.rows;
        
        size_t s = face->glyph->bitmap.pitch;
        for (int y = 0; y < face->glyph->bitmap.rows; y++)
        {
            for (int x = 0; x < face->glyph->bitmap.width; x++)
            {
                size_t index = (y + dy) * dw + x + dx;
                
                if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
                {
                    uint8_t byte = face->glyph->bitmap.buffer[s * y + x / 8];
                    byte <<= x % 8;
                    glyph.data.at(index) = (byte & 0x80) ? 15 : 0;
                }
                else
                {
                    glyph.data.at(index) =
                        (face->glyph->bitmap.buffer[w * y + x] + 8) / 17;
                }
            }
        }
        glyphtable.push_back(glyph);
        
        charcode = FT_Get_Next_Char(face, charcode, &gindex);
    }
    
    eliminate_duplicates(glyphtable);
    crop_glyphs(glyphtable, fontinfo);
    detect_flags(glyphtable, fontinfo);
    
    std::unique_ptr<DataFile> result(new DataFile(
        dictionary, glyphtable, fontinfo));
    return result;
}

}
