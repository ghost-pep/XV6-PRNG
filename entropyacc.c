#include "types.h"
#include "defs.h"
#include "entropyacc.h"

/**
 * @brief Private entropy pools that the entropy accumulator
 * uses to store real random event data.
 */
struct entropy_pool pools[MAX_POOLS];

/**
 * @brief Initializes the entropy accumulator. This method should be used
 * by the pseudo random number generator on startup.
 */
void
initEntropyAccumulator(void)
{
  for (int i = 0; i < MAX_POOLS; i++) {
    memset(pools[i].entropy, 0, POOL_SIZE);
    pools[i].size = 0;
    initlock(&pools[i].lock, "pools");
  }
}

/**
 * @brief Adds given real random event data from a given source number to
 * the entropy pool with the given pool number. The string that gets added
 * to the entropy pools is in the following format:
 * snum (1 byte) + lenght of event data (1 byte) + event data (up to 32 bytes)
 *
 * @param snum the source number describing the source of the real
 * random event
 * @param pnum the pool number to add the real random event to
 * @param edata the string of bytes of the real random event data
 * @param data_len the length of the event data
 */
void
addRandomEvent(int snum, int pnum, char *edata, int data_len)
{
  struct entropy_pool *selected_pool;

  /// Verify valid source number
  if (snum < 0 || snum > MAX_SOURCES) {
    return;
  }

  /// Verify valid pool number
  if (pnum < 0 || pnum >= MAX_POOLS) {
    return;
  }

  /// Verify valid edata address
  if (edata == 0) {
    return;
  }

  /// Verify valid event data size
  if (data_len < 0 || data_len > MAX_EDATA_SIZE) {
    return;
  }

  /// Set up selected pool and acquire lock for it
  selected_pool = &pools[pnum];
  acquire(&selected_pool->lock);

  /// Add source number of real random event to given pool
  selected_pool->entropy[selected_pool->size % POOL_SIZE] = (char) snum;
  selected_pool->size++;

  /// Add size of real random event to given pool
  selected_pool->entropy[selected_pool->size % POOL_SIZE] = (char) data_len;
  selected_pool->size++;

  /// Add a string of bytes of the real random event data to pool
  for (int i = 0; i < data_len; i ++) {
    selected_pool->entropy[(selected_pool->size + i) % POOL_SIZE] = edata[i];
  }
  selected_pool->size += data_len;

  /// Release lock for selected pool
  release(&selected_pool->lock);
}

/**
 * @brief Prints out the data at all the pools. Used for debugging purposes only.
 */
void
printPoolsData(void)
{
  int sum;

  for (int i = 0; i < MAX_POOLS; i++) {
    sum = 0;
    for (int j = 0; j < pools[i].size; j++) {
      sum += pools[i].entropy[j];
    }
    cprintf("POOL %d SIZE: %d DATA: %d\n", i, pools[i].size, sum);
  }
}

