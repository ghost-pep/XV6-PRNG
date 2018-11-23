#ifndef ENTROPYACC_H
#define ENTROPYACC_H

#define MAX_POOLS 32       /// Maximum number of entropy pools
#define MAX_SOURCES 255    /// Maximum number of event sources
#define MAX_EDATA_SIZE 32  /// Maximum number of bytes of event data
#define MIN_SIZE_POOL 64   /// Number of bytes of pool to reseed after
#define POOL_SIZE 4096     /// Size of each pool in bytes

/**
 * @brief Struct representing an entropy pools. The pool
 * assumes each real random event generates 8 bits entropy
 * and takes 4 bytes in the pool. Since the optimal number of bits of
 * entropy is 128, we should reseed after a pool is filled with 64 bytes.
 * Each pool can theoretically hold infinite amount of data but we're
 * limiting it each pool to a page for simplicity.
 */
struct entropy_pool {
  int size;
  char entropy[POOL_SIZE];
};

/// Function Prototypes
void initEntropyAccumulator(void);
void addRandomEvent(int snum, int pnum, char *edata);

#endif
