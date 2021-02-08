#include<stdio.h>
#include<kernel.h>
#include<proc.h>
#include<lab0.h>

static unsigned long *stkptr,*esp;

void printtos(){
  
   asm("movl %ebp, stkptr;"
 	"movl %esp, esp;");
  kprintf("\n\nvoid printtos()\n\n");
    
  kprintf("Before[0x%08x]: 0x%08x\n",stkptr+2,*(stkptr+2));
  kprintf("After[0x%08x]: 0x%08x\n",esp,*(esp));
 
  int i;
  for(i=1;i<=4 && esp+i<stkptr;i++){                    
	
       kprintf("\telement[0x%08x]: 0x%08x\n",esp+i,*(esp+i));
	}

}
