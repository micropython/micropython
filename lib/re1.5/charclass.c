#include "re1.5.h"

// More efficient character range check macro
#define CHAR_IN_RANGE(c, min, max) ((unsigned char)(c) - (unsigned char)(min) <= (unsigned char)(max) - (unsigned char)(min))

// Specialized character class checks for better performance
#define IS_DIGIT(c) CHAR_IN_RANGE(c, '0', '9')
#define IS_SPACE(c) ((c) == ' ' || CHAR_IN_RANGE(c, '\t', '\r'))
#define IS_WORD(c) (IS_DIGIT(c) || CHAR_IN_RANGE(c, 'A', 'Z') || CHAR_IN_RANGE(c, 'a', 'z') || (c) == '_')

int _re1_5_classmatch(const char *pc, const char *sp)
{
    // pc points to "cnt" byte after opcode
    int is_positive = (pc[-1] == Class);
    int cnt = *(unsigned char*)pc++;  // Use unsigned to avoid sign extension
    char sp_c = *sp;                  // Cache value for performance
    
    // Fast path for common classes
    while (cnt--) {
        if (*pc == RE15_CLASS_NAMED_CLASS_INDICATOR) {
            if (_re1_5_namedclassmatch(pc + 1, sp)) {
                return is_positive;
            }
        } else if (CHAR_IN_RANGE(sp_c, *pc, pc[1])) {
            return is_positive;
        }
        pc += 2;
    }
    return !is_positive;
}

int _re1_5_namedclassmatch(const char *pc, const char *sp)
{
    // pc points to name of class
    char sp_c = *sp;                  // Cache value for performance
    char class_type = *pc | 0x20;     // Case-insensitive comparison
    int inverted = (*pc >> 5) & 1;    // Upper bits = inversion flag
    int result;

    // Specialized class matching logic with dedicated macros
    if (class_type == 'd') {
        result = IS_DIGIT(sp_c);
    } else if (class_type == 's') {
        result = IS_SPACE(sp_c);
    } else { // 'w'
        result = IS_WORD(sp_c);
    }
    
    // XOR with inverted flag to handle negated classes (\D, \S, \W)
    return result ^ inverted;
}