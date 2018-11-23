#include "types.h"
#include "defs.h"
#include "prng.h"
#include "entropyacc.h"

extern struct entropy_pool pools[MAX_POOLS];

void prnginit(void) {

}

void prngrand(int size, char* output) {

}   

/**
 * @brief Generates a pseudrandom string of bytes.
 *
 * @param random_string the pointer to the pseudorandom string of bytes
 * @param bytes the number of bytes of random data to generate
 * @return int 0 on success and -1 on error
 */
void sys_random(void) {
  
}
