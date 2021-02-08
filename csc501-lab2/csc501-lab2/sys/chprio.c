/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}


	int del_prio=pptr->pprio; // if previous priority is more need to replace that even if it is less than max_prio
	pptr->pprio = newprio;
	pptr->pinh=-1;
	int i;
	for(i=0;i<NLOCKS;i++){
		if(proctab[pid].lockhold[i]==1){
			int max_prio=max_process_priority_inqueue(i);
			update_priority(i, max_prio, del_prio);
					
		}
	}
	
	int max_prio=max_process_priority_inqueue(proctab[pid].lwait);
	update_priority(proctab[pid].lwait, max_prio, del_prio);	

	//all the locks it holds have to be checked and increase the process priorrity according
	//and also increase the priorit of the processes qhich having the lock for which this process is waititng
	
	restore(ps);
	return(newprio);
}
