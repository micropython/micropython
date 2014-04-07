#include "misc.h"
#include "mpconfig.h"
#include "repl.h"

#if MICROPY_ENABLE_REPL_HELPERS

bool str_startswith_word(const char *str, const char *head) {
    int i;
    for (i = 0; str[i] && head[i]; i++) {
        if (str[i] != head[i]) {
            return false;
        }
    }
    return head[i] == '\0' && (str[i] == '\0' || !unichar_isalpha(str[i]));
}

bool mp_repl_is_compound_stmt(const char *line) {
    // compound if line starts with a certain keyword
    if (
           str_startswith_word(line, "if")
        || str_startswith_word(line, "while")
        || str_startswith_word(line, "for")
        || str_startswith_word(line, "try")
        || str_startswith_word(line, "with")
        || str_startswith_word(line, "def")
        || str_startswith_word(line, "class")
        || str_startswith_word(line, "@")
       ) {
        return true;
    }

    // also "compound" if unmatched open bracket or triple quote
    int n_paren = 0;
    int n_brack = 0;
    int n_brace = 0;
    int in_triple_quote = 0;
    for (const char *l = line; *l; l++) {
        switch (*l) {
            case '(': n_paren += 1; break;
            case ')': n_paren -= 1; break;
            case '[': n_brack += 1; break;
            case ']': n_brack -= 1; break;
            case '{': n_brace += 1; break;
            case '}': n_brace -= 1; break;
            case '"':
                if (l[1] == '"' && l[2] == '"') {
                    l += 2;
                    in_triple_quote = 1 - in_triple_quote;
                }
                break;
        }
    }
    return n_paren > 0 || n_brack > 0 || n_brace > 0 || in_triple_quote != 0;
}

#endif // MICROPY_ENABLE_REPL_HELPERS
