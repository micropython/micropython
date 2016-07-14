#include "optimize_rlefont.hh"
#include "encode_rlefont.hh"
#include <random>
#include <iostream>
#include <set>
#include <thread>
#include <algorithm>
#include "ccfixes.hh"

namespace mcufont {
namespace rlefont {

typedef std::mt19937 rnd_t;

// Select a random substring among all the glyphs in the datafile.
std::unique_ptr<DataFile::pixels_t> random_substring(const DataFile &datafile, rnd_t &rnd)
{
    std::uniform_int_distribution<size_t> dist1(0, datafile.GetGlyphCount() - 1);
    size_t index = dist1(rnd);
    
    const DataFile::pixels_t &pixels = datafile.GetGlyphEntry(index).data;
    
    std::uniform_int_distribution<size_t> dist2(2, pixels.size());
    size_t length = dist2(rnd);
    
    std::uniform_int_distribution<size_t> dist3(0, pixels.size() - length);
    size_t start = dist3(rnd);
    
    std::unique_ptr<DataFile::pixels_t> result;
    result.reset(new DataFile::pixels_t(pixels.begin() + start,
                                        pixels.begin() + start + length));
    return result;
}

// Try to replace the worst dictionary entry with a better one.
void optimize_worst(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose)
{
    std::uniform_int_distribution<size_t> dist(0, 1);
    
    DataFile trial = datafile;
    size_t worst = trial.GetLowScoreIndex();
    DataFile::dictentry_t d = trial.GetDictionaryEntry(worst);
    d.replacement = *random_substring(datafile, rnd);
    d.ref_encode = dist(rnd);
    trial.SetDictionaryEntry(worst, d);
    
    size_t newsize = get_encoded_size(trial);
    
    if (newsize < size)
    {
        d.score = size - newsize;
        datafile.SetDictionaryEntry(worst, d);
        size = newsize;
        
        if (verbose)
            std::cout << "optimize_worst: replaced " << worst
                      << " score " << d.score << std::endl;
    }
}

// Try to replace random dictionary entry with another one.
void optimize_any(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose)
{
    DataFile trial = datafile;
    std::uniform_int_distribution<size_t> dist(0, DataFile::dictionarysize - 1);
    size_t index = dist(rnd);
    DataFile::dictentry_t d = trial.GetDictionaryEntry(index);
    d.replacement = *random_substring(datafile, rnd);
    trial.SetDictionaryEntry(index, d);
    
    size_t newsize = get_encoded_size(trial);
    
    if (newsize < size)
    {
        d.score = size - newsize;
        datafile.SetDictionaryEntry(index, d);
        size = newsize;
        
        if (verbose)
            std::cout << "optimize_any: replaced " << index
                      << " score " << d.score << std::endl;
    }
}

// Try to append or prepend random dictionary entry.
void optimize_expand(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose, bool binary_only)
{
    DataFile trial = datafile;
    std::uniform_int_distribution<size_t> dist1(0, DataFile::dictionarysize - 1);
    size_t index = dist1(rnd);
    DataFile::dictentry_t d = trial.GetDictionaryEntry(index);
    
    std::uniform_int_distribution<size_t> dist3(1, 3);
    size_t count = dist3(rnd);
    
    for (size_t i = 0; i < count; i++)
    {
        std::uniform_int_distribution<size_t> booldist(0, 1);
        std::uniform_int_distribution<size_t> pixeldist(0, 15);
        uint8_t pixel;
        
        if (binary_only)
        {
            pixel = booldist(rnd) ? 15 : 0;
        }
        else
        {
            pixel = pixeldist(rnd);
        }
        
        bool prepend = booldist(rnd);
        
        if (prepend)
        {
            d.replacement.insert(d.replacement.begin(), pixel);
        }
        else
        {
            d.replacement.push_back(pixel);
        }
    }
    
    trial.SetDictionaryEntry(index, d);
    
    size_t newsize = get_encoded_size(trial);
    
    if (newsize < size)
    {
        d.score = size - newsize;
        datafile.SetDictionaryEntry(index, d);
        size = newsize;
        
        if (verbose)
            std::cout << "optimize_expand: expanded " << index
                      << " by " << count << " pixels, score " << d.score << std::endl;
    }
}

// Try to trim random dictionary entry.
void optimize_trim(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose)
{
    DataFile trial = datafile;
    std::uniform_int_distribution<size_t> dist1(0, DataFile::dictionarysize - 1);
    size_t index = dist1(rnd);
    DataFile::dictentry_t d = trial.GetDictionaryEntry(index);
    
    if (d.replacement.size() <= 2) return;
    
    std::uniform_int_distribution<size_t> dist2(0, std::min((int)d.replacement.size() / 2, 5));
    size_t start = dist2(rnd);
    size_t end = dist2(rnd);
    
    if (start)
    {
        d.replacement.erase(d.replacement.begin(), d.replacement.begin() + start);
    }
    
    if (end)
    {
        d.replacement.erase(d.replacement.end() - end, d.replacement.end() - 1);
    }
    
    trial.SetDictionaryEntry(index, d);
    
    size_t newsize = get_encoded_size(trial);
    
    if (newsize < size)
    {
        d.score = size - newsize;
        datafile.SetDictionaryEntry(index, d);
        size = newsize;
        
        if (verbose)
            std::cout << "optimize_trim: trimmed " << index
                      << " by " << start << " pixels from start and "
                      << end << " pixels from end, score " << d.score << std::endl;
    }
}

// Switch random dictionary entry to use ref encoding or back to rle.
void optimize_refdict(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose)
{
    DataFile trial = datafile;
    std::uniform_int_distribution<size_t> dist1(0, DataFile::dictionarysize - 1);
    size_t index = dist1(rnd);
    DataFile::dictentry_t d = trial.GetDictionaryEntry(index);
    
    d.ref_encode = !d.ref_encode;
    
    trial.SetDictionaryEntry(index, d);
    
    size_t newsize = get_encoded_size(trial);
    
    if (newsize < size)
    {
        d.score = size - newsize;
        datafile.SetDictionaryEntry(index, d);
        size = newsize;
        
        if (verbose)
            std::cout << "optimize_refdict: switched " << index
                      << " to " << (d.ref_encode ? "ref" : "RLE")
                      << ", score " << d.score << std::endl;
    }
}

// Combine two random dictionary entries.
void optimize_combine(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose)
{
    DataFile trial = datafile;
    std::uniform_int_distribution<size_t> dist1(0, DataFile::dictionarysize - 1);
    size_t worst = datafile.GetLowScoreIndex();
    size_t index1 = dist1(rnd);
    size_t index2 = dist1(rnd);
    
    const DataFile::pixels_t &part1 = datafile.GetDictionaryEntry(index1).replacement;
    const DataFile::pixels_t &part2 = datafile.GetDictionaryEntry(index2).replacement;
    
    DataFile::dictentry_t d;
    d.replacement = part1;
    d.replacement.insert(d.replacement.end(), part2.begin(), part2.end());
    d.ref_encode = true;
    trial.SetDictionaryEntry(worst, d);
    
    size_t newsize = get_encoded_size(trial);
    
    if (newsize < size)
    {
        d.score = size - newsize;
        datafile.SetDictionaryEntry(worst, d);
        size = newsize;
        
        if (verbose)
            std::cout << "optimize_combine: combined " << index1
                      << " and " << index2 << " to replace " << worst
                      << ", score " << d.score << std::endl;
    }
}

// Pick a random part of an encoded glyph and encode it as a ref dict.
void optimize_encpart(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose)
{
    std::unique_ptr<encoded_font_t> e = encode_font(datafile);
    
    // Pick a random encoded glyph
    std::uniform_int_distribution<size_t> dist1(0, datafile.GetGlyphCount() - 1);
    size_t index = dist1(rnd);
    const encoded_font_t::refstring_t &refstr = e->glyphs.at(index);
    
    if (refstr.size() < 2)
        return;
    
    // Pick a random part of it
    std::uniform_int_distribution<size_t> dist2(2, refstr.size());
    size_t length = dist2(rnd);
    std::uniform_int_distribution<size_t> dist3(0, refstr.size() - length);
    size_t start = dist3(rnd);
    
    // Decode that part
    encoded_font_t::refstring_t substr(refstr.begin() + start,
                                       refstr.begin() + start + length);
    std::unique_ptr<DataFile::pixels_t> decoded =
        decode_glyph(*e, substr, datafile.GetFontInfo());
    
    // Add that as a new dictionary entry
    DataFile trial = datafile;
    size_t worst = trial.GetLowScoreIndex();
    DataFile::dictentry_t d = trial.GetDictionaryEntry(worst);
    d.replacement = *decoded;
    d.ref_encode = true;
    trial.SetDictionaryEntry(worst, d);
    
    size_t newsize = get_encoded_size(trial);
    
    if (newsize < size)
    {
        d.score = size - newsize;
        datafile.SetDictionaryEntry(worst, d);
        size = newsize;
        
        if (verbose)
            std::cout << "optimize_encpart: replaced " << worst
                      << " score " << d.score << std::endl;
    }
}

// Execute all the optimization algorithms once.
void optimize_pass(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose)
{
    optimize_worst(datafile, size, rnd, verbose);
    optimize_any(datafile, size, rnd, verbose);
    optimize_expand(datafile, size, rnd, verbose, false);
    optimize_expand(datafile, size, rnd, verbose, true);
    optimize_trim(datafile, size, rnd, verbose);
    optimize_refdict(datafile, size, rnd, verbose);
    optimize_combine(datafile, size, rnd, verbose);
    optimize_encpart(datafile, size, rnd, verbose);
}

// Execute multiple passes in parallel and take the one with the best result.
// The amount of parallelism is hardcoded in order to retain deterministic
// behaviour.
void optimize_parallel(DataFile &datafile, size_t &size, rnd_t &rnd, bool verbose, int num_threads = 4)
{
    std::vector<DataFile> datafiles;
    std::vector<size_t> sizes;
    std::vector<rnd_t> rnds;
    std::vector<std::unique_ptr<std::thread> > threads;
    
    for (int i = 0; i < num_threads; i++)
    {
        datafiles.emplace_back(datafile);
        sizes.emplace_back(size);
        rnds.emplace_back(rnd());
    }
    
    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(new std::thread(optimize_pass,
                                             std::ref(datafiles.at(i)),
                                             std::ref(sizes.at(i)),
                                             std::ref(rnds.at(i)),
                                             verbose));
    }
    
    for (int i = 0; i < num_threads; i++)
    {
        threads.at(i)->join();
    }
    
    int best = std::min_element(sizes.begin(), sizes.end()) - sizes.begin();
    size = sizes.at(best);
    datafile = datafiles.at(best);
}

// Go through all the dictionary entries and check what it costs to remove
// them. Removes any entries with negative or zero score.
void update_scores(DataFile &datafile, bool verbose)
{
    size_t oldsize = get_encoded_size(datafile);
    
    for (size_t i = 0; i < DataFile::dictionarysize; i++)
    {
        DataFile trial = datafile;
        DataFile::dictentry_t dummy = {};
        trial.SetDictionaryEntry(i, dummy);
        size_t newsize = get_encoded_size(trial);
        
        DataFile::dictentry_t d = datafile.GetDictionaryEntry(i);
        d.score = newsize - oldsize;
        
        if (d.score > 0)
        {
            datafile.SetDictionaryEntry(i, d);
        }
        else
        {
            datafile.SetDictionaryEntry(i, dummy);
            
            if (verbose && d.replacement.size() != 0)
                std::cout << "update_scores: dropped " << i
                        << " score " << -d.score << std::endl;
        }
    }
}

void init_dictionary(DataFile &datafile)
{
    rnd_t rnd(datafile.GetSeed());
    
    if (datafile.GetGlyphCount() == 0)
        return;
    
    std::set<DataFile::pixels_t> seen_substrings;
    std::set<DataFile::pixels_t> added_substrings;
    
    size_t i = 0;
    while (i < DataFile::dictionarysize)
    {
        DataFile::pixels_t substring = *random_substring(datafile, rnd);
        
        if (!seen_substrings.count(substring))
        {
            seen_substrings.insert(substring);
        }
        else if (!added_substrings.count(substring))
        {
            // When we see a substring second time, add it.
            DataFile::dictentry_t d;
            d.score = 0;
            d.replacement = substring;
            datafile.SetDictionaryEntry(i, d);
            i++;
            added_substrings.insert(substring);
        }
    }
}

void optimize(DataFile &datafile, size_t iterations)
{
    bool verbose = false;
    rnd_t rnd(datafile.GetSeed());
    
    update_scores(datafile, verbose);
    
    size_t size = get_encoded_size(datafile);
    
    for (size_t i = 0; i < iterations; i++)
    {
        optimize_parallel(datafile, size, rnd, verbose);
    }
    
    std::uniform_int_distribution<size_t> dist(0, std::numeric_limits<uint32_t>::max());
    datafile.SetSeed(dist(rnd));
}

}}
