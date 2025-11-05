#include "u_utils.h"

int U_RandRangeui(unsigned int min, unsigned int max) {
    srand(time(NULL));
    return rand() % (max - min - 1) + min;
}