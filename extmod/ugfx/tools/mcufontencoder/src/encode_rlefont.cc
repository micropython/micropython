#include "encode_rlefont.hh"
#include <algorithm>
#include <stdexcept>
#include "ccfixes.hh"

// Number of reserved codes before the dictionary entries.
#define DICT_START 24

// Special reference to mean "fill with zeros to the end of the glyph"
#define REF_FILLZEROS 16

// RLE codes
#define RLE_CODEMASK    0xC0
#define RLE_VALMASK     0x3F
#define RLE_ZEROS       0x00 // 0 to 63 zeros
#define RLE_64ZEROS     0x40 // (1 to 64) * 64 zeros
#define RLE_ONES        0x80 // 1 to 64 full alphas
#define RLE_SHADE       0xC0 // 1 to 4 partial alphas

// Dictionary "fill entries" for encoding bits directly.
#define DICT_START7BIT  4
#define DICT_START6BIT  132
#define DICT_START5BIT  196
#define DICT_START4BIT  228
#define DICT_START3BIT  244
#define DICT_START2BIT  252

namespace mcufont {
namespace rlefont {

// Get bit count for the "fill entries"
static size_t fillentry_bitcount(size_t index)
{
    if (index >= DICT_START2BIT)
        return 2;
    else if (index >= DICT_START3BIT)
        return 3;
    else if (index >= DICT_START4BIT)
        return 4;
    else if (index >= DICT_START5BIT)
        return 5;
    else if (index >= DICT_START6BIT)
        return 6;
    else
        return 7;
}

// Count the number of equal pixels at the beginning of the pixelstring.
static size_t prefix_length(const DataFile::pixels_t &pixels, size_t pos)
{
    uint8_t pixel = pixels.at(pos);
    size_t count = 1;
    while (pos + count < pixels.size() &&
            pixels.at(pos + count) == pixel)
    {
        count++;
    }
    return count;
}

// Perform the RLE encoding for a dictionary entry.
static encoded_font_t::rlestring_t encode_rle(const DataFile::pixels_t &pixels)
{
    encoded_font_t::rlestring_t result;
    
    size_t pos = 0;
    while (pos < pixels.size())
    {
        uint8_t pixel = pixels.at(pos);
        size_t count = prefix_length(pixels, pos);
        pos += count;
        
        if (pixel == 0)
        {
            // Up to 63 zeros can be encoded with RLE_ZEROS. If there are more,
            // encode using RLE_64ZEROS, and then whatever remains with RLE_ZEROS.
            while (count >= 64)
            {
                size_t c = (count > 4096) ? 64 : (count / 64);
                result.push_back(RLE_64ZEROS | (c - 1));
                count -= c * 64;
            }
            
            if (count)
            {
                result.push_back(RLE_ZEROS | count);
            }
        }
        else if (pixel == 15)
        {
            // Encode ones.
            while (count)
            {
                size_t c = (count > 64) ? 64 : count;
                result.push_back(RLE_ONES | (c - 1));
                count -= c;
            }
        }
        else
        {
            // Encode shades.
            while (count)
            {
                size_t c = (count > 4) ? 4 : count;
                result.push_back(RLE_SHADE | ((c - 1) << 4) | pixel);
                count -= c;
            }
        }
    }
    
    return result;
}

// We use a tree structure to represent the dictionary entries.
// Using this tree, we can perform a combined Aho-Corasick string matching
// and breadth-first search to find the optimal encoding of glyph data.
class DictTreeNode
{
public:
    constexpr DictTreeNode():
        m_index(-1),
        m_ref(false),
        m_length(0),
        m_child0(nullptr),
        m_child15(nullptr),
        m_suffix(nullptr)
        {}
    
    void SetChild(uint8_t p, DictTreeNode *child)
    {
        if (p == 0)
            m_child0 = child;
        else if (p == 15)
            m_child15 = child;
        else if (p > 15)
            throw std::logic_error("invalid pixel alpha: " + std::to_string(p));
        else
        {
            if (!m_children)
            {
                m_children.reset(new DictTreeNode*[14]());
            }
            m_children[p - 1] = child;
        }
    }
    
    DictTreeNode* GetChild(uint8_t p) const
    { 
        if (p == 0)
            return m_child0;
        else if (p == 15)
            return m_child15;
        else if (p > 15)
            throw std::logic_error("invalid pixel alpha: " + std::to_string(p));
        else if (!m_children)
            return nullptr;
        else
            return m_children[p - 1];
    }
    
    bool HasIntermediateChildren() const { return m_children != nullptr; }
    
    int GetIndex() const { return m_index; }
    void SetIndex(int index) { m_index = index; }
    bool GetRef() const { return m_ref; }
    void SetRef(bool ref) { m_ref = ref; }
    size_t GetLength() const { return m_length; }
    void SetLength(size_t length) { m_length = length; }
    DictTreeNode *GetSuffix() const { return m_suffix; }
    void SetSuffix(DictTreeNode *suffix) { m_suffix = suffix; }
    
private:
    // Index of dictionary entry or -1 if just a intermediate node.
    int m_index;
    
    // True for ref-encoded dictionary entries. Used to avoid recursion when
    // encoding them.
    bool m_ref;
    
    // Length of the corresponding dictionary entry replacement.
    // Equals the distance from the tree root.
    size_t m_length;
    
    // Most tree nodes will only ever contains children for 0 or 15.
    // Therefore the array for other nodes is allocated only on demand.
    DictTreeNode *m_child0;
    DictTreeNode *m_child15;
    std::unique_ptr<DictTreeNode*[]> m_children;
    
    // Pointer to the longest suffix of this entry that exists in the
    // dictionary.
    DictTreeNode *m_suffix;
};

// Preallocated array for tree nodes
class TreeAllocator
{
public:
    TreeAllocator(size_t count)
    {
        m_storage.reset(new DictTreeNode[count]);
        m_next = m_storage.get();
        m_left = count;
    }
    
    DictTreeNode *allocate()
    {
        if (m_left == 0)
            throw std::logic_error("Ran out of preallocated entries");
        
        m_left--;
        return m_next++;
    }
    
private:
    std::unique_ptr<DictTreeNode[]> m_storage;
    DictTreeNode *m_next;
    size_t m_left;
};

// Add a new dictionary entry to the tree. Adds the intermediate nodes, but
// does not yet fill the suffix pointers.
static DictTreeNode* add_tree_entry(const DataFile::pixels_t &entry, int index,
                                    bool ref_encoded, DictTreeNode *root,
                                    TreeAllocator &storage)
{
    DictTreeNode* node = root;
    for (uint8_t p : entry)
    {
        DictTreeNode* branch = node->GetChild(p);
        if (!branch)
        {
            branch = storage.allocate();
            node->SetChild(p, branch);
        }
        
        node = branch;
    }
    
    // Replace the entry if it either does not yet have an encoding, or if
    // the new entry is non-ref (i.e. can be used in more situations).
    if (node->GetIndex() < 0 || (node->GetRef() && !ref_encoded))
    {
        node->SetIndex(index);
        node->SetRef(ref_encoded);
        node->SetLength(entry.size());
    }
    
    return node;
}

// Walk the tree and find if the entry exists in the tree. If it does,
// returns a pointer to it, otherwise nullptr.
static DictTreeNode *find_tree_node(DataFile::pixels_t::const_iterator begin,
                                    DataFile::pixels_t::const_iterator end,
                                    DictTreeNode *root)
{
    DictTreeNode* node = root;
    while (begin != end)
    {
        uint8_t pixel = *begin++;
        node = node->GetChild(pixel);
        
        if (!node)
            return nullptr;
    }
    
    return node;
}

// Fill in the suffix pointers recursively for the given subtree.
static void fill_tree_suffixes(DictTreeNode *root, DictTreeNode *subtree,
    const DataFile::pixels_t &entry)
{
    for (size_t i = 1; i < entry.size(); i++)
    {
        DictTreeNode *node = find_tree_node(entry.begin() + i, entry.end(), root);
        if (node)
        {
            subtree->SetSuffix(node);
            break;
        }
    }
    
    if (!subtree->GetSuffix())
        subtree->SetSuffix(root);
    
    DataFile::pixels_t newentry(entry);
    newentry.resize(entry.size() + 1);
    for (uint8_t i = 0; i < 16; i++)
    {
        // Speed-up for the common case of 0 and 15 alphas.
        if (i == 1 && !subtree->HasIntermediateChildren())
            i += 14;
        
        DictTreeNode *child = subtree->GetChild(i);
        if (child)
        {
            newentry.at(entry.size()) = i;
            fill_tree_suffixes(root, child, newentry);
        }
    }
}

// Construct a lookup tree from the dictionary entries.
static DictTreeNode* construct_tree(const std::vector<DataFile::dictentry_t> &dictionary,
                                    TreeAllocator &storage, bool fast)
{
    DictTreeNode* root = storage.allocate();
    
    // Populate the hardcoded entries for 0 to 15 alpha.
    for (int j = 0; j < 16; j++)
    {
        DictTreeNode *node = storage.allocate();
        node->SetIndex(j);
        node->SetRef(false);
        node->SetLength(1);
        root->SetChild(j, node);
    }
    
    // Populate the actual dictionary entries
    size_t i = DICT_START;
    for (DataFile::dictentry_t d : dictionary)
    {
        if (!d.replacement.size())
            break;
        
        add_tree_entry(d.replacement, i, d.ref_encode, root, storage);
        i++;
    }
    
    if (!fast)
    {
        // Populate the fill entries for rest of dictionary
        for (; i < 256; i++)
        {
            DataFile::pixels_t pixels;
            size_t bitcount = fillentry_bitcount(i);
            uint8_t byte = i - DICT_START7BIT;
            for (size_t j = 0; j < bitcount; j++)
            {
                uint8_t p = (byte & (1 << j)) ? 15 : 0;
                pixels.push_back(p);
            }
            
            add_tree_entry(pixels, i, false, root, storage);
        }
        
        // Fill in the suffix pointers for optimal encoding
        DataFile::pixels_t nullentry;
        fill_tree_suffixes(root, root, nullentry);
    }
    
    return root;
}

// Structure for keeping track of the shortest encoding to reach particular
// point of the pixel string.
struct encoding_link_t
{
    // Index of the position prior to the last dictionary entry.
    size_t previous;
    
    // Index of the dictionary entry that brings us to this point.
    int index;
    
    // Number of links to get here from the start of the string.
    size_t length;
    
    constexpr encoding_link_t(): previous(0), index(-1), length(9999999) {}
};

// Perform the reference encoding for a glyph entry (optimal version).
// Uses a modified Aho-Corasick algorithm combined with breadth first search
// to find the shortest representation.
static encoded_font_t::refstring_t encode_ref_slow(const DataFile::pixels_t &pixels,
                                                   const DictTreeNode *root,
                                                   bool is_glyph)
{
    // Chain of encodings. Each entry in this array corresponds to a position
    // in the pixel string.
    std::unique_ptr<encoding_link_t[]> chain(new encoding_link_t[pixels.size() + 1]);
    
    chain[0].previous = 0;
    chain[0].index = 0;
    chain[0].length = 0;
    
    // Read the pixels one-by-one and update the encoding links accordingly.
    const DictTreeNode *node = root;
    for (size_t pos = 0; pos < pixels.size(); pos++)
    {
        uint8_t pixel = pixels.at(pos);
        const DictTreeNode *branch = node->GetChild(pixel);
        
        while (!branch)
        {
            // Cannot expand this sequence, defer to suffix.
            node = node->GetSuffix();
            branch = node->GetChild(pixel);
        }
        
        node = branch;
        
        // We have arrived at a new node, add it and any proper suffixes to
        // the link chain.
        const DictTreeNode *suffix = node;
        while (suffix != root)
        {
            if (suffix->GetIndex() >= 0 && (is_glyph || !suffix->GetRef()))
            {
                encoding_link_t link;
                link.previous = pos + 1 - suffix->GetLength();
                link.index = suffix->GetIndex();
                link.length = chain[link.previous].length + 1;
                
                if (link.length < chain[pos + 1].length)
                    chain[pos + 1] = link;
            }
            suffix = suffix->GetSuffix();
        }
    }
    
    // Check if we can shorten the final encoding using REF_FILLZEROS.
    if (is_glyph)
    {
        for (size_t pos = pixels.size() - 1; pos > 0; pos--)
        {
            if (pixels.at(pos) != 0)
                break;
            
            encoding_link_t link;
            link.previous = pos;
            link.index = REF_FILLZEROS;
            link.length = chain[pos].length + 1;
            
            if (link.length <= chain[pixels.size()].length)
                chain[pixels.size()] = link;
        }
    }
    
    // Backtrack from the final link back to the start and construct the
    // encoded string.
    encoded_font_t::refstring_t result;
    size_t len = chain[pixels.size()].length;
    result.resize(len);
    
    size_t pos = pixels.size();
    for (size_t i = len; i > 0; i--)
    {
        result.at(i - 1) = chain[pos].index;
        pos = chain[pos].previous;
    }
    
    return result;
}

// Walk the tree as far as possible following the given pixel string iterator.
// Returns number of pixels encoded, and index is set to the dictionary reference.
static size_t walk_tree(const DictTreeNode *tree,
                        DataFile::pixels_t::const_iterator pixels,
                        DataFile::pixels_t::const_iterator pixelsend,
                        int &index, bool is_glyph)
{
    size_t best_length = 0;
    size_t length = 0;
    index = -1;
    
    const DictTreeNode* node = tree;
    while (pixels != pixelsend)
    {
        uint8_t pixel = *pixels++;
        node = node->GetChild(pixel);
        
        if (!node)
            break;
        
        length++;
        
        if (is_glyph || !node->GetRef())
        {
            if (node->GetIndex() >= 0)
            {
                index = node->GetIndex();
                best_length = length;
            }
        }
    }
    
    if (index < 0)
        throw std::logic_error("walk_tree failed to find a valid encoding");
    
    return best_length;
}

// Perform the reference encoding for a glyph entry (fast version).
// Uses a simple greedy search to find select the encodings.
static encoded_font_t::refstring_t encode_ref_fast(const DataFile::pixels_t &pixels,
                                                   const DictTreeNode *tree,
                                                   bool is_glyph)
{
    encoded_font_t::refstring_t result;
    
    // Strip any zeroes from end
    size_t end = pixels.size();
    
    if (is_glyph)
    {
        while (end > 0 && pixels.at(end - 1) == 0) end--;
    }
    
    size_t i = 0;
    while (i < end)
    {
        int index;
        i += walk_tree(tree, pixels.begin() + i, pixels.end(), index, is_glyph);
        result.push_back(index);
    }
    
    if (i < pixels.size())
        result.push_back(REF_FILLZEROS);
    
    return result;
}

static encoded_font_t::refstring_t encode_ref(const DataFile::pixels_t &pixels,
                                              const DictTreeNode *tree,
                                              bool is_glyph, bool fast)
{
    if (fast)
        return encode_ref_fast(pixels, tree, is_glyph);
    else
        return encode_ref_slow(pixels, tree, is_glyph);
}

// Compare dictionary entries by their coding type.
// Sorts RLE-encoded entries first and any empty entries last.
static bool cmp_dict_coding(const DataFile::dictentry_t &a,
                            const DataFile::dictentry_t &b)
{
    if (a.replacement.size() == 0 && b.replacement.size() != 0)
        return false;
    else if (a.replacement.size() != 0 && b.replacement.size() == 0)
        return true;
    else if (a.ref_encode == false && b.ref_encode == true)
        return true;
    else
        return false;
}

size_t estimate_tree_node_count(const std::vector<DataFile::dictentry_t> &dict)
{
    size_t count = DICT_START; // Preallocated entries
    for (const DataFile::dictentry_t &d: dict)
    {
        count += d.replacement.size();
    }
    count += 128 * 7; // Fill entries
    return count;
}

std::unique_ptr<encoded_font_t> encode_font(const DataFile &datafile,
                                            bool fast)
{
    std::unique_ptr<encoded_font_t> result(new encoded_font_t);
    
    // Sort the dictionary so that RLE-coded entries come first.
    // This way the two are easy to distinguish based on index.
    std::vector<DataFile::dictentry_t> sorted_dict = datafile.GetDictionary();
    std::stable_sort(sorted_dict.begin(), sorted_dict.end(), cmp_dict_coding);
    
    // Build the binary tree for looking up references.
    size_t count = estimate_tree_node_count(sorted_dict);
    TreeAllocator allocator(count);
    DictTreeNode* tree = construct_tree(sorted_dict, allocator, fast);
    
    // Encode the dictionary entries, using either RLE or reference method.
    for (const DataFile::dictentry_t &d : sorted_dict)
    {
        if (d.replacement.size() == 0)
        {
            continue;
        }
        else if (d.ref_encode)
        {
            result->ref_dictionary.push_back(encode_ref(d.replacement, tree, false, fast));
        }
        else
        {
            result->rle_dictionary.push_back(encode_rle(d.replacement));
        }
    }
    
    // Then reference-encode the glyphs
    for (const DataFile::glyphentry_t &g : datafile.GetGlyphTable())
    {
        result->glyphs.push_back(encode_ref(g.data, tree, true, fast));
    }
    
    // Optionally verify that the encoding was correct.
    if (!fast)
    {
        for (size_t i = 0; i < datafile.GetGlyphCount(); i++)
        {
            std::unique_ptr<DataFile::pixels_t> decoded = 
                decode_glyph(*result, i, datafile.GetFontInfo());
            if (*decoded != datafile.GetGlyphEntry(i).data)
            {
                auto iter = std::mismatch(decoded->begin(), decoded->end(),
                                          datafile.GetGlyphEntry(i).data.begin());
                size_t pos = iter.first - decoded->begin();
                throw std::logic_error("verification of glyph " + std::to_string(i) +
                    " failed at position " + std::to_string(pos));
            }
        }
    }
    
    return result;
}

size_t get_encoded_size(const encoded_font_t &encoded)
{
    size_t total = 0;
    for (const encoded_font_t::rlestring_t &r : encoded.rle_dictionary)
    {
        total += r.size();
        
        if (r.size() != 0)
            total += 2; // Offset table entry
    }
    for (const encoded_font_t::refstring_t &r : encoded.ref_dictionary)
    {
        total += r.size();
        
        if (r.size() != 0)
            total += 2; // Offset table entry
    }
    for (const encoded_font_t::refstring_t &r : encoded.glyphs)
    {
        total += r.size();
        total += 2; // Offset table entry
        total += 1; // Width table entry
    }
    return total;
}

std::unique_ptr<DataFile::pixels_t> decode_glyph(
    const encoded_font_t &encoded,
    const encoded_font_t::refstring_t &refstring,
    const DataFile::fontinfo_t &fontinfo)
{
    std::unique_ptr<DataFile::pixels_t> result(new DataFile::pixels_t);
    
    for (uint8_t ref : refstring)
    {
        if (ref <= 15)
        {
            result->push_back(ref);
        }
        else if (ref == REF_FILLZEROS)
        {
            result->resize(fontinfo.max_width * fontinfo.max_height, 0);
        }
        else if (ref < DICT_START)
        {
            throw std::logic_error("unknown code: " + std::to_string(ref));
        }
        else if (ref - DICT_START < (int)encoded.rle_dictionary.size())
        {
            for (uint8_t rle : encoded.rle_dictionary.at(ref - DICT_START))
            {
                if ((rle & RLE_CODEMASK) == RLE_ZEROS)
                {
                    for (int i = 0; i < (rle & RLE_VALMASK); i++)
                    {
                        result->push_back(0);
                    }
                }
                else if ((rle & RLE_CODEMASK) == RLE_64ZEROS)
                {
                    for (int i = 0; i < ((rle & RLE_VALMASK) + 1) * 64; i++)
                    {
                        result->push_back(0);
                    }
                }
                else if ((rle & RLE_CODEMASK) == RLE_ONES)
                {
                    for (int i = 0; i < (rle & RLE_VALMASK) + 1; i++)
                    {
                        result->push_back(15);
                    }
                }
                else if ((rle & RLE_CODEMASK) == RLE_SHADE)
                {
                    uint8_t count, alpha;
                    count = ((rle & RLE_VALMASK) >> 4) + 1;
                    alpha = ((rle & RLE_VALMASK) & 0xF);
                    for (int i = 0; i < count; i++)
                    {
                        result->push_back(alpha);
                    }
                }
            }
        }
        else if (ref - DICT_START - encoded.rle_dictionary.size() < encoded.ref_dictionary.size())
        {
            size_t index = ref - DICT_START - encoded.rle_dictionary.size();
            std::unique_ptr<DataFile::pixels_t> part =
                decode_glyph(encoded, encoded.ref_dictionary.at(index),
                             fontinfo);
            result->insert(result->end(), part->begin(), part->end());
        }
        else
        {
            size_t bitcount = fillentry_bitcount(ref);
            
            uint8_t byte = ref - DICT_START7BIT;
            for (size_t i = 0; i < bitcount; i++)
            {
                uint8_t p = (byte & (1 << i)) ? 15 : 0;
                result->push_back(p);
            }
        }
    }
    
    return result;
}

std::unique_ptr<DataFile::pixels_t> decode_glyph(
    const encoded_font_t &encoded, size_t index,
    const DataFile::fontinfo_t &fontinfo)
{
    return decode_glyph(encoded, encoded.glyphs.at(index), fontinfo);
}

}}
