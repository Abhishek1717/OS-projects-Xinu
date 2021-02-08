#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>



SYSCALL ldelete(int lock_no)
{
        STATWORD ps;
        int     pid;
        struct  lentry  *lptr;

        disable(ps);
        if (isbadlock(lock_no) || locker[lock_no].lstate==LFREE) {
                restore(ps);
                return(SYSERR);
        }
        lptr = &locker[lock_no];
        lptr->lstate = LFREE;
        if (nonempty(lptr->lhead)) {
                while( (pid=getfirst(lptr->lhead)) != EMPTY)
                  {
                    proctab[pid].pwaitret = DELETED;
                    ready(pid,RESCHNO);
                  }

             
        }
	int i;
	for(i=0;i<NPROC-1;i++){
		if(proctab[i].pstate != PRFREE){
			
			proctab[i].createlockhold[lock_no]=DELETED;
		
		}	
	}

	resched();
        restore(ps);
        return(OK);
}


