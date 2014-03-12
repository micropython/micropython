#include "../ff.h"

#if _USE_LFN != 0

#if   _CODE_PAGE == 932
#include "cc932.c"
#elif _CODE_PAGE == 936
#include "cc936.c"
#elif _CODE_PAGE == 949
#include "cc949.c"
#elif _CODE_PAGE == 950
#include "cc950.c"
#else
#include "ccsbcs.c"
#endif

#endif
