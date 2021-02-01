#include "prod.h"

float prod_array(int n, float *ar) {
    float ans = 1;
    for (int i = 0; i < n; ++i) {
        ans *= ar[i];
    }
    return ans;
}
