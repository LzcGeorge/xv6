#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
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
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 va;
  uint64 user_addr;
  int page_num;
  unsigned int abits = 0;

  if(argaddr(0, &va) < 0)
    return -1;

  if(argint(1, &page_num) < 0)
    return -1;

  if(argaddr(2,&user_addr) < 0)
    return -1;

  pte_t * pte;
  struct proc *p = myproc();

  uint64 cur = PGROUNDDOWN(va); // 对地址进行舍入操作，以确保它们指向页的开始，而不是位于页的中间。

  // 第 page_num 页
  uint64 va_page_end = PGROUNDDOWN(cur + page_num * PGSIZE - 1);

  // 32 ：set an upper limit on the number of pages that can be scanned.
  for(int i = 0; i < 32 && cur <= va_page_end; i ++, cur += PGSIZE )
  {
    // walk(pagetable_t pagetable, uint64 va, int alloc) 
    // Return the address of the PTE in page table pagetable
    // 1 --> alloc bit : If alloc!=0, create any required page-table pages.
    if((pte = walk(p->pagetable,cur,0)) == 0) {
      continue;
    }
    else
    {
      if(*pte & PTE_A) {
        abits |= (1L << i);
        *pte &= ~PTE_A; 
      }
    }
  }
  // copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
  copyout(p->pagetable, user_addr, (char *) &abits, sizeof(abits));
  return 0;

}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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
