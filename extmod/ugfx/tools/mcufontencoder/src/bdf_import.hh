// Function for importing .BDF fonts as data files.

#pragma once
#include "datafile.hh"

namespace mcufont
{

std::unique_ptr<DataFile> LoadBDF(std::istream &file);

}

#ifdef CXXTEST_RUNNING
#include <cxxtest/TestSuite.h>

using namespace mcufont;

class BDFTests: public CxxTest::TestSuite
{
public:
    void testLoadBDF()
    {
        std::istringstream s(testfile);
        std::unique_ptr<DataFile> f = LoadBDF(s);
        
        TS_ASSERT_EQUALS(f->GetFontInfo().name, "-Misc-Fixed-Medium-R-Normal--14-130-75-75-C-70-ISO8859-15");
        TS_ASSERT_EQUALS(f->GetFontInfo().max_width, 6);
        TS_ASSERT_EQUALS(f->GetFontInfo().max_height, 11);
        TS_ASSERT_EQUALS(f->GetGlyphCount(), 1);
        TS_ASSERT_EQUALS(f->GetGlyphEntry(0).chars.size(), 2);
    }
    
private:
    static constexpr const char *testfile = 
        "STARTFONT 2.1\n"
        "FONT -Misc-Fixed-Medium-R-Normal--14-130-75-75-C-70-ISO8859-15\n"
        "FONTBOUNDINGBOX 7 14 0 -2\n"
        "STARTCHAR defaultchar\n"
        "ENCODING 0\n"
        "DWIDTH 7 0\n"
        "BBX 7 14 0 -2\n"
        "BITMAP\n"
        "00\n"
        "B4\n"
        "84\n"
        "00\n"
        "84\n"
        "84\n"
        "00\n"
        "84\n"
        "84\n"
        "00\n"
        "84\n"
        "B4\n"
        "00\n"
        "00\n"
        "ENDCHAR\n"
        "STARTCHAR copychar\n"
        "ENCODING 2\n"
        "DWIDTH 7 0\n"
        "BBX 7 14 0 -2\n"
        "BITMAP\n"
        "00\n"
        "B4\n"
        "84\n"
        "00\n"
        "84\n"
        "84\n"
        "00\n"
        "84\n"
        "84\n"
        "00\n"
        "84\n"
        "B4\n"
        "00\n"
        "00\n"
        "ENDCHAR\n";

};
#endif
