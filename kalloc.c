// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"
#include "entropyacc.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file

/**
 * @brief Flag keeping track of if kinit2 has been called.
 */
int kinit2_called;

/**
 * @brief Keeps track of how many memory operations have been performed.
 */
int memory_operations;

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
  kinit2_called = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}

//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r, *pg;
  static uint free_memory_pool = 0;
  uint free_pages = 0;
  uint free_memory;

  /// Increment number of memory operations
  memory_operations++;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;

  /// Add random event occasionally
  if (kinit2_called == 1 && memory_operations > 500) {
    /// Record timings of free physical memory in entropy collectors
    pg = kmem.freelist;
    while (pg != 0) {
      free_pages++;
      pg = pg->next;
    }
    free_memory = free_pages * PGSIZE;
    addRandomEvent(FREE_MEMORY_SIZE, free_memory_pool % MAX_POOLS, (char *) &free_memory, sizeof(uint));
    free_memory_pool++;
    memory_operations = 0;
  }

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r, *pg;
  static uint free_memory_pool = 0;
  uint free_pages = 0;
  uint free_memory;

  /// Increment number of memory operations
  memory_operations++;

  if(kmem.use_lock)
    acquire(&kmem.lock);

  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;

  /// Add random event occasionally
  if (kinit2_called == 1 && memory_operations > 500) {
    /// Record timings of free physical memory in entropy collectors
    pg = kmem.freelist;
    while (pg != 0) {
      free_pages++;
      pg = pg->next;
    }
    free_memory = free_pages * PGSIZE;
    addRandomEvent(FREE_MEMORY_SIZE, free_memory_pool % MAX_POOLS, (char *) &free_memory, sizeof(uint));
    free_memory_pool++;
    memory_operations = 0;
  }

  if(kmem.use_lock)
    release(&kmem.lock);

  return (char*)r;
}

