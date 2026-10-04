#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

// prj 3
// return the number of page allocated
uint64
sys_pagecount(void)
{
  return myproc()->pagealloc;
}
// return the total page fault 
uint64
sys_pagefault(void)
{
  return myproc()->pagefault;
}

// prj 2
// set the priority of current proc
uint64
sys_set_sched_priority(void)
{
  int prio;
  argint(0, &prio);
  // from 0 to 3 only
  if(prio < 0 || prio > 3)
    return -1;

  struct proc *p = myproc();
  acquire(&p->lock);
  p->priority = prio;
  release(&p->lock);
  return 0;
}

// get the priority of pid
uint64
sys_get_sched_priority(void)
{
  int pid;
  argint(0, &pid);
  return(proc_prio(pid)); // proc.c
}

// return burst time of a proc
uint64
sys_sjf_job_length(void)
{
  int pid;
  argint(0, &pid);
  pid = proc_job_length(pid); //proc.c
  return pid;
}

// return RUNNING time of pid
uint64
sys_ticks_running(void)
{
  int pid;
  argint(0, &pid);
  pid = proc_runtime(pid); // proc.c
  return(pid);
}

// prj 1
uint64
sys_hello(void)
{
  printf("Hello from Kernel Mode!\n");
  return 0;
}
uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}
// prj 3
uint64
sys_sbrk(void)
{
  // DBG
  if(0) {
#ifdef LAZY
    printf("LAZY ALLOCATOR\n");
#elif LOCALITY
    printf("LOCALITY ALLOCATOR\n");
#endif
  }

  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;
/*
  // added 0 to test sbrk prj 3
  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    printf("lazy\n");
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  */
  // increase the pagetable size for lazy allocator
  acquire(&myproc()->lock);
  myproc()->sz += n;
  release(&myproc()->lock);
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
