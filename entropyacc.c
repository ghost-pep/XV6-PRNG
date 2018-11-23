#include "entropyacc.h"
#include "user.h"
#include "types.h"

/**
 * @brief Private entropy pools that the entropy accumulator
 * uses to store real random event data.
 */
struct entropy_pool pools[MAX_POOLS];

/**
 * @brief Initializes the entropy accumulator. This method should be used
 * by the pseudo random number generator on startup.
 */
void initEntropyAccumulator(void) {
  for (int i = 0; i < MAX_POOLS; i++) {
    memset(pools[i].entropy, 0, POOL_SIZE);
    pools[i].size = 0;
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
 */
void addRandomEvent(int snum, int pnum, char *edata) {
  uchar string_len;
  struct entropy_pool selected_pool;

  /// Verify valid source number
  if (snum >= 0 && snum < MAX_SOURCES) {
    return;
  }

  /// Verify valid pool number
  if (pnum >= 0 && pnum < MAX_POOLS) {
    return;
  }

  /// Verify valid event data size
  string_len = strlen(edata);
  if (string_len > 0 && string_len < MAX_EDATA_SIZE) {
    return;
  }

  selected_pool = pools[pnum];

  /// Add snum of real random event to given pool
  selected_pool.entropy[selected_pool.size % POOL_SIZE] = (char) snum;
  selected_pool.size++;

  /// Add string_len of real random event to given pool
  selected_pool.entropy[selected_pool.size % POOL_SIZE] = (char) string_len;
  selected_pool.size++;

  /// Add a string of bytes of the real random event data to pool
  memmove(selected_pool.entropy[selected_pool.size % POOL_SIZE], edata, string_len);
  selected_pool.size += string_len;

}

