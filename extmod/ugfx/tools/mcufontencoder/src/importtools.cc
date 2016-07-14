#include "importtools.hh"
#include <limits>

namespace mcufont {

void eliminate_duplicates(std::vector<DataFile::glyphentry_t> &glyphtable)
{
    for (size_t i = 0; i + 1 < glyphtable.size(); i++)
    {
        for (size_t j = i + 1; j < glyphtable.size(); j++)
        {
            if (glyphtable.at(i).data == glyphtable.at(j).data &&
                glyphtable.at(i).width == glyphtable.at(j).width)
            {
                for (int c : glyphtable.at(j).chars)
                    glyphtable.at(i).chars.push_back(c);
                
                glyphtable.erase(glyphtable.begin() + j);
                j--;
            }
        }
    }
}

struct bbox_t
{
    int left;
    int top;
    int right;
    int bottom;
    
    bbox_t()
    {
        left = std::numeric_limits<int>::max();
        top = std::numeric_limits<int>::max();
        right = std::numeric_limits<int>::min();
        bottom = std::numeric_limits<int>::min();
    }
    
    void update(int x, int y)
    {
        if (x < left) left = x;
        if (x > right) right = x;
        if (y < top) top = y;
        if (y > bottom) bottom = y;
    }
};

void crop_glyphs(std::vector<DataFile::glyphentry_t> &glyphtable,
                 DataFile::fontinfo_t &fontinfo)
{
    // Find out the maximum bounding box
    bbox_t bbox;
    for (DataFile::glyphentry_t &glyph : glyphtable)
    {
        if (glyph.data.size() == 0)
            continue; // Dummy glyph
    
        for (int y = 0; y < fontinfo.max_height; y++)
        {
            for (int x = 0; x < fontinfo.max_width; x++)
            {
                if (glyph.data.at(y * fontinfo.max_width + x))
                    bbox.update(x, y);
            }
        }
    }
    
    if (bbox.right < bbox.left)
        return; // There were no glyphs
    
    // Crop the glyphs to that
    size_t old_w = fontinfo.max_width;
    size_t new_w = bbox.right - bbox.left + 1;
    size_t new_h = bbox.bottom - bbox.top + 1;
    for (DataFile::glyphentry_t &glyph : glyphtable)
    {
        if (glyph.data.size() == 0)
            continue; // Dummy glyph
    
        DataFile::pixels_t old = glyph.data;
        glyph.data.clear();
        
        for (size_t y = 0; y < new_h; y++)
        {
            for (size_t x = 0; x < new_w; x++)
            {
                size_t old_x = bbox.left + x;
                size_t old_y = bbox.top + y;
                size_t old_pos = old_w * old_y + old_x; 
                glyph.data.push_back(old.at(old_pos));
            }
        }
    }
    
    fontinfo.max_width = new_w;
    fontinfo.max_height = new_h;
    fontinfo.baseline_x -= bbox.left;
    fontinfo.baseline_y -= bbox.top;
}

void detect_flags(const std::vector<DataFile::glyphentry_t> &glyphtable,
                  DataFile::fontinfo_t &fontinfo)
{
    if (!glyphtable.size())
        return;
    
    // Check if all glyphs have equal width
    int width = glyphtable[0].width;
    bool is_monospace = true;
    for (const DataFile::glyphentry_t &g : glyphtable)
    {
        if (g.width != width)
        {
            is_monospace = false;
            break;
        }
    }
    
    if (is_monospace)
        fontinfo.flags |= DataFile::FLAG_MONOSPACE;
    
    // Check if all glyphs contain only 0 or 15 alpha
    bool is_bw = true;
    for (const DataFile::glyphentry_t &g : glyphtable)
    {
        for (uint8_t pixel : g.data)
        {
            if (pixel != 0 && pixel != 15)
            {
                is_bw = false;
                break;
            }
        }
        if (!is_bw) break;
    }
    
    if (is_bw)
        fontinfo.flags |= DataFile::FLAG_BW;
}


}
