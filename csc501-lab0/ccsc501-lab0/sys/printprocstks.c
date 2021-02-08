#include<stdio.h>
#include<kernel.h>
#include<proc.h>
#include<lab0.h>

static unsigned long *esp;

void printprocstks(int priority){
printf("\n\nvoid printprocstks(int priority)\n\n\n");

       int i,count=0;
	for(i=0;i<NPROC;i++){
		
		if(proctab[i].pprio>priority  && proctab[i].pstate!=PRFREE){
		if(i!=currpid) esp= (unsigned long *)proctab[i].pesp;
		else  asm("movl %esp,esp;");
		kprintf("Process [%s]\n",proctab[i].pname);
		kprintf("\tpid:%d\n",i);
		kprintf("\tpriority:%d \n",proctab[i].pprio);
		kprintf("\tbase:0x%08x\n",proctab[i].pbase);
		kprintf("\tlimit: 0x%08x\n",proctab[i].plimit);
		kprintf("\tlen: %d \n",proctab[i].pstklen);
		kprintf("\tpointer:0x %08x\n",esp);
	        }

    
	}	


}
