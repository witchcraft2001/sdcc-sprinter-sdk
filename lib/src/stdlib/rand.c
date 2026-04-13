#include <stdlib.h>

static unsigned int _rand_seed = 1;

void srand(unsigned int seed) {
    _rand_seed = seed;
}

int rand(void) {
    _rand_seed ^= _rand_seed << 7;
    _rand_seed ^= _rand_seed >> 9;
    _rand_seed ^= _rand_seed << 8;
    return (int)(_rand_seed & RAND_MAX);
}
