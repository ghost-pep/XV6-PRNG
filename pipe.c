#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "file.h"
#include "spinlock.h"
#include "ctr.h"

#define PIPESIZE 512

struct pipe {
  struct spinlock lock;
  char data[PIPESIZE];
  uint nread;     // number of bytes read
  uint nwrite;    // number of bytes written
  int readopen;   // read fd is still open
  int writeopen;  // write fd is still open
  struct selproc selprocread;
  struct selproc selprocwrite;
  
    char encrypted;
    u_int32_t* key;
    u_int8_t* counter;
};

int
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *p;

  p = 0;
  *f0 = *f1 = 0;
  if((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  if((p = (struct pipe*)kalloc()) == 0)
    goto bad;
  p->readopen = 1;
  p->writeopen = 1;
  p->nwrite = 0;
  p->nread = 0;
  initselproc(&p->selprocread);
  initselproc(&p->selprocwrite);
  initlock(&p->lock, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = p;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = p;
  return 0;

//PAGEBREAK: 20
 bad:
  if(p)
    kfree((char*)p);
  if(*f0)
    fileclose(*f0);
  if(*f1)
    fileclose(*f1);
  return -1;
}

void
pipeclose(struct pipe *p, int writable)
{
  acquire(&p->lock);
  if(writable){
    p->writeopen = 0;
    // Wake up anything waiting to read
    // Lab 4: Your code here.
    wakeupselect(&p->selprocread);
    wakeup(&p->nread);
  } else {
    p->readopen = 0;
    // Wake up anything waiting to write
    // LAB 4: Your code here
    wakeupselect(&p->selprocwrite);
    wakeup(&p->nwrite);
  }
  if(p->readopen == 0 && p->writeopen == 0){
    release(&p->lock);
    kfree((char*)p);
  } else
    release(&p->lock);
}

//PAGEBREAK: 40
int
pipewrite(struct pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);
  for(i = 0; i < n; i++){
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
      if(p->readopen == 0 || proc->killed){
        release(&p->lock);
        return -1;
      }

      wakeupselect(&p->selprocread);
      wakeup(&p->nread);
      sleep(&p->nwrite, &p->lock);  //DOC: pipewrite-sleep
    }
    p->data[p->nwrite++ % PIPESIZE] = addr[i];
  }

  // Wake up anything waiting to read
  // LAB 4: Your code here
  wakeupselect(&p->selprocread);
  wakeup(&p->nread);  //DOC: pipewrite-wakeup1
  release(&p->lock);
  return n;
}

int
piperead(struct pipe *p, char *addr, int n)
{
  int i;

  acquire(&p->lock);
  while(p->nread == p->nwrite && p->writeopen){  //DOC: pipe-empty
    if(proc->killed){
      release(&p->lock);
      return -1;
    }
    sleep(&p->nread, &p->lock); //DOC: piperead-sleep
  }
  for(i = 0; i < n; i++){  //DOC: piperead-copy
    if(p->nread == p->nwrite)
      break;
    addr[i] = p->data[p->nread++ % PIPESIZE];
  }

  // Wake up anything waiting to write
  // LAB 4: Your code here
  wakeupselect(&p->selprocwrite);
  wakeup(&p->nwrite);  //DOC: piperead-wakeup
  release(&p->lock);
  return i;
}

/* Checks if this pipe is writeable or not.
 *
 * Requirements:
 *
 * 1. Return -1 if the pipe is not open for reading or the process has been killed.
 * 2. Check if the pipe is writeable and return 1 if yes and 0 if not.
 */
int
pipewriteable(struct pipe *p)
{
    // LAB 4: Your code here
    if(p->readopen == 0 || proc->killed)	// If we have no process or we can't read what we write, return -1
      return -1;

    // If we have room in the pipe, write to it!
    return p->nwrite != p->nread + PIPESIZE;
}

/* Checks if this pipe is readable or not.
 *
 * Requirements:
 *
 * 1. If the process has been killed, return -1.
 * 2. If the pipe is non-empty or closed, return 1; otherwise 0.
 */
int
pipereadable(struct pipe *p)
{
    // LAB 4: Your code here
    if(proc->killed)	// If there is no process, return an error
      return -1;

    // If there is something in the pipe or the pipe is closed, we can read from it!
    return p->nread != p->nwrite || p->writeopen == 0;
}

/* Sets a wakeup call.
 *
 * Requirements:
 *
 * 1. Use addselid to add the selid channel to the list of wakeups
 *
 * Note: 5 denotes a read, while 6 denotes a write
 */
int
pipeselect(struct pipe *p, int * selid, struct spinlock * lk)
{
    if ((*selid) == 5) {			//We are trying to wait on the read side
      if(p->selprocread.selcount < NSELPROC){
	addselid(&p->selprocread, selid, lk);
      }
    } else {
      if(p->selprocwrite.selcount < NSELPROC){	//We are trying to wait on the write side
	addselid(&p->selprocwrite, selid, lk);
      }
    }

    return 0;
}

/* Clears a wakeup call
 *
 * Requirements:
 *
 * 1. Clear a selid from the list of wakeups.
 */
int
pipeclrsel(struct pipe *p, int * selid)
{

    // LAB 4: Your code here.
    if((*selid) == 5)//5 indicates a read
      clearselid(&p->selprocread, selid);
    else
      clearselid(&p->selprocwrite, selid);
    return 0;
}
