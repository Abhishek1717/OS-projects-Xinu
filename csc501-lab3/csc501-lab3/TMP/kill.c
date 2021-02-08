/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>
extern int sc_frames[];
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	int i;
	for(i=0;i<NFRAMES;i++){
		if(frm_tab[i].fr_pid==pid){
			frm_tab[i].fr_pid=-1;
		frm_tab[i].fr_status=FRM_UNMAPPED;
  		frm_tab[i].fr_vpno=-1;
  		frm_tab[i].fr_refcnt=0;
  		frm_tab[i].fr_type=-1;
 		frm_tab[i].fr_dirty=0;
		int j;
		for(j=0;j<NFRAMES;j++){
			if(sc_frames[j]==i)
				sc_frames[j]=-1;
		}
  

		}
	
	}
for(i=0;i<16;i++){
	if(bsm_tab[i].bs_pid==pid)
		release_bs(i);
}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
