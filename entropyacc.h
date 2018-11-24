#ifndef ENTROPYACC_H
#define ENTROPYACC_H

#include "spinlock.h"

/// Entropy Collector Characteristics
#define MAX_POOLS 32       /// Maximum number of entropy pools
#define MAX_SOURCES 255    /// Maximum number of event sources
#define MAX_EDATA_SIZE 32  /// Maximum number of bytes of event data
#define MIN_SIZE_POOL 64   /// Number of bytes of pool to reseed after
#define POOL_SIZE 4096     /// Size of each pool in bytes

/// Event source codes
#define KEYBOARD_PRESS 0     /// The key pressed by the keyboard
#define KEYBOARD_TIMING 1    /// Clock ticks when keyboard is pressed
#define INTERRUPT_TIMING 2   /// Clock ticks when interrupt occured
#define SCHEDULING_TIMING 3  /// Clock ticks when new process is scheduled
#define FREE_MEMORY_SIZE 4   /// Size of free physical memory
#define READ_DATA 5          /// Data read from disk
#define READ_TIMING 6        /// Clock ticks when new data is read from disk
#define WRITE_DATA 7         /// Data written to disk
#define WRITE_TIMING 8       /// Clock ticks when new data is written to disk

/**
 * @brief Struct representing an entropy pools. The pool
 * assumes each real random event generates 8 bits entropy
 * and takes 4 bytes in the pool. Since the optimal number of bits of
 * entropy is 128, we should reseed after a pool is filled with 64 bytes.
 * Each pool can theoretically hold infinite amount of data but we're
 * limiting it each pool to a page for simplicity. Each pool also has
 * a lock to prevent unintended race conditions.
 */
struct entropy_pool {
  int size;
  char entropy[POOL_SIZE];
  struct spinlock lock;
};

/// Function Prototypes
void initEntropyAccumulator(void);
void addRandomEvent(int snum, int pnum, char *edata, int data_len);
void printPoolsData(void);

#endif
