#include "types.h"
#include "defs.h"
#include "prng.h"
#include "entropyacc.h"
#include "hash.h"
#include "aes.h"
#include "ctr.h"

extern struct entropy_pool pools[MAX_POOLS];

struct {
  int counter[4];  // 128-bit counter
  char key[32];    // 256-bit key
  int reseed_ctr;
  int last_reseed_ticks;
} prng;

static void prnginit_internal();
static void prngreseed(char*, int);
static void prngrand(char*, int);
static void prngranddata(int, char**);
static void prnggenblocks(int, char**);
static void incrctr();

void
prnginit(void)
{
  initEntropyAccumulator();
  prng.reseed_ctr = 0;
  prng.last_reseed_ticks = 0;
  prnginit_internal();
}

/**
 * @brief Generates a pseudorandom string of bytes.
 *
 * @return int 0 on success and -1 on error
 */
int
sys_random(void) {
  char* bytesout;
  int numbytes;

  argint(1, &numbytes);
  if (numbytes < 0) {
    return -1;
  }

  argptr(0, &bytesout, numbytes);
  if (!bytesout) {
    return -1;
  }

  /// For Debugging purposes;
  printPoolsData();

  prngrand(bytesout, numbytes);
  return 0;
}

static void
prngreseed(char* seed, int length)
{
  hash(seed, length, prng.key);
  incrctr();
  kfree(seed);

  acquire(&tickslock);
  prng.last_reseed_ticks = ticks;
  release(&tickslock);
}

static void
prnginit_internal(void)
{
  memset(prng.counter, 0, 16);
  memset(prng.key, 0, 32);
}

static void
prngrand(char* output, int numbytes)
{
  /* Get the number of ticks for time comparison. */
  unsigned int xticks;
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);

  /* Reseed if p0 has > 64 bytes and if 100ms (10 ticks) has passed since the last reseed. */
  acquire(&pools[0].lock);
  if (pools[0].size > MIN_SIZE_POOL && (xticks - prng.last_reseed_ticks) > 10) {
    prng.reseed_ctr++;
    release(&pools[0].lock);

    /* Allocate memory for new seed. */
    char* seed_data = kalloc();
    if (!seed_data) {
      panic("prngrand: unable to allocate memory.");
    }
    memset(seed_data, 0, 4096);

    /* Get data from selected pools. */
    int offset = 0;
    for (int i = 0; i < MAX_POOLS; i++) {
      if (prng.reseed_ctr | (1 << i)) {
        acquire(&pools[i].lock);
        hash(pools[i].entropy, (pools[i].size > POOL_SIZE) ? POOL_SIZE : pools[i].size, seed_data + offset);
        offset += 32;
        memset(pools[i].entropy, 0, (pools[i].size > POOL_SIZE) ? POOL_SIZE : pools[i].size);
        pools[i].size = 0;
        release(&pools[i].lock);
      }
    }

    /* Reseed with pool data. */
    prngreseed(seed_data, offset);
  }

  if (prng.reseed_ctr == 0) {
    panic("prngrand: attempted to get random data without seeding.");
  } else {
    /* Allocate memory to hold pseudo-random data. */
    char* data_ptr_page = kalloc();
    if (!data_ptr_page) {
      panic("prngrand: unable to allocate memory.");
    }
    memset(data_ptr_page, 0, 4096);

    prngranddata(numbytes, (char**) data_ptr_page);

    /* Get pseudo-random data from indirect ptr_page and put it in the output pointer. */
    int left = numbytes;
    for (int i = 0; i < 1024; i++) {
      char* data_page = ((char**) data_ptr_page)[i];
      if (left < 4096) {
        memmove(output + (i * 4096), data_page, left);
        left = 0;
        kfree(data_page);
        break;
      } else {
        memmove(output + (i * 4096), data_page, 4096);
        left -= 4096;
        kfree(data_page);
      }
    }
    kfree(data_ptr_page);
  }
}

static void
prngranddata(int size, char** output)
{
  /* Enforce bounds on input size. */
  if (size < 0 || size > (1 << 20)) {
    panic("prngranddata: requested negative or too much data.");
  }

  /* Enforce not-null output. */
  if (!output) {
    panic("prnggranddata: provided null memory location for output.");
  }

  /* Allocate memory for first call to prnggenblocks for the pseudo-random data. */
  char* blocks_ptr_page = kalloc();
  if (!blocks_ptr_page) {
    panic("prngranddata: unable to allocate memory.");
  }
  memset(blocks_ptr_page, 0, 4096);

  /* Apply ceiling to size/16 for number of blocks. */
  int numblocks = ((size % 16) == 0) ? (size / 16) : (size / 16) + 1;

  /* Generate desired number of blocks. */
  prnggenblocks(numblocks, (char**) blocks_ptr_page);

  /* Get size bytes of data from prnggenblocks. */
  char* pgwrittento = 0;
  char* pgreadfrom = 0;
  for (int j = 0; j < size; j++) {
    if ((j % 4096) == 0) {
      pgwrittento = kalloc();
      if (!pgwrittento) {
        panic("prngranddata: unable to allocate memory.");
      }
      memset(pgwrittento, 0, 4096);
      output[j / 4096] = pgwrittento;

      pgreadfrom = ((char**) blocks_ptr_page)[j / 4096];
    }
    pgwrittento[j] = pgreadfrom[j];
  }

  /* Free memory allocated by prnggenblocks after having gotten desired data. */
  char** iter_bpp = (char**) blocks_ptr_page;
  for (int i = 0; i < 1024; i++) {
    if (!iter_bpp[i]) {
      break;
    } else {
      kfree(iter_bpp[i]);
    }
  }
  kfree(blocks_ptr_page);

  /* Allocate memory for second call to prnggenblocks for new key. */
  blocks_ptr_page = kalloc();
  if (!blocks_ptr_page) {
    panic("prngranddata: unable to allocate memory.");
  }
  memset(blocks_ptr_page, 0, 4096);

  /* Get new 32-byte key from prnggenblocks. */
  prnggenblocks(2, (char**) blocks_ptr_page);
  memmove(prng.key, ((char**) blocks_ptr_page)[0], 32);

  /* Free memory allocated by prnggenblocks after having gotten desired data. */
  kfree(((char**) blocks_ptr_page)[0]);
  kfree(blocks_ptr_page);
}

static void
prnggenblocks(int blocks, char** memaddr_arr)
{
  /* Enforce that generator has been seeded. */
  if (prng.counter[0] == 0) {
    panic("prnggenblocks: called with non-seeded generator.");
  }

  /* Enforce that memaddr_arr is not null. */
  if (!memaddr_arr) {
    panic("prnggenblocks: called with null memory location for output.");
  }

  /* Use aesblockcipher to generate blocks and kalloc to allocate memory to store the blocks. */
  char* pgwrittento = 0;
  for (int i = 0; i < 16 * blocks; i += 16) {
    if ((i % 4096) == 0) {
      char* kalloc_pg = kalloc();
      if (!kalloc_pg) {
        panic("prnggenblocks: unable to allocate memory for block generation.");
      }
      memset(kalloc_pg, 0, 4096);
      memaddr_arr[i / 4096] = kalloc_pg;
      pgwrittento = kalloc_pg;
    }
      // Creating a 128-bits key from a 256-bit keys
      u_int32_t* key1 = (u_int32_t*) prng.key;
      u_int32_t* key2 = (u_int32_t*) (prng.key + 16);
      key key;
      for (uint i = 0; i<KEY_SIZE; i+=1) {
          key[i] = key1[i] ^ key2[i];
      }
      u_int32_t roundKeys[BLOCK_COLUMNS*(ROUNDS+1)];    // Keys needed for AES-encryption

      keyExpansion(key, roundKeys, KEY_SIZE);
      aes_encrypt((u_int8_t*)prng.counter, (u_int8_t*)pgwrittento + (i % 4096), roundKeys);
      incrctr();
  }
}

static void
incrctr(void)
{
  if (prng.counter[0] != 0xFFFFFFFF) {
    prng.counter[0]++;
  } else {
    if (prng.counter[1] != 0xFFFFFFFF) {
      prng.counter[1]++;
    } else {
      if (prng.counter[2] != 0xFFFFFFFF) {
        prng.counter[2]++;
      } else {
        if (prng.counter[3] != 0xFFFFFFFF) {
          prng.counter[3]++;
        } else {
          prng.counter[0] = 0;
          prng.counter[1] = 0;
          prng.counter[2] = 0;
          prng.counter[3] = 0;
        }
      }
    }
  }
}
