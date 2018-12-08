#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "ctr.h"
#include "encryption.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#define returnIfNeg(X) \
            if((X)<0) {     \
                return -1;  \
            }               \

int sys_encrypt(void) {
    const u_int8_t * message;
    u_int8_t * ciphertext;
    size_t size;
    const u_int8_t * sequence;
    u_int32_t * key;
    
    returnIfNeg(argint(2, (int*)&size));
    returnIfNeg(argptr(0, (char**)&message, size));
    returnIfNeg(argptr(1, (char**)&ciphertext, size));
    returnIfNeg(argptr(3, (char**)&sequence, size + BLOCK_LENGTH - (size%BLOCK_LENGTH)));   // Rounding the size up
    returnIfNeg(argptr(4, (char**)&key, KEY_SIZE));
    
    ctr_encrypt(message, ciphertext, size, sequence, key, KEY_SIZE);
    return 0;
}

int sys_decrypt(void) {
    const u_int8_t * ciphertext;
    u_int8_t * message;
    size_t size;
    const u_int8_t * sequence;
    u_int32_t * key;
    
    returnIfNeg(argint(2, (int*)&size));
    returnIfNeg(argptr(0, (char**)&ciphertext, size));
    returnIfNeg(argptr(1, (char**)&message, size));
    returnIfNeg(argptr(3, (char**)&sequence, size + BLOCK_LENGTH - (size%BLOCK_LENGTH)));   // Rounding the size up
    returnIfNeg(argptr(4, (char**)&key, KEY_SIZE));
    
    ctr_decrypt(ciphertext, message, size, sequence, key, KEY_SIZE);
    return 0;
}
