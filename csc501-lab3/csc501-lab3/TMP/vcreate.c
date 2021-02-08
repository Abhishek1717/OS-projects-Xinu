/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{	
	STATWORD ps;
	disable(ps);	
	int pid=create(procaddr,ssize,priority,name,nargs,args);
        int store=get_bsm();
	if(store<0 ||store>15){
		restore(ps);
		return SYSERR;
	}
	if(bsm_map(pid,4096,store,hsize)==OK){
		bsm_tab[store].bs_status=BSM_MAPPED;
		bsm_tab[store].bs_shared=0;
		proctab[pid].vmemlist->mnext=(struct mblock*)(4096*NBPG);
		proctab[pid].vmemlist->mlen =hsize*NBPG;
		struct mblock *bs;
		bs= 0x00800000 +( 0x00080000*store);
		bs->mnext=NULL;
		bs->mlen=hsize*NBPG;
		//proctab[pid].vmemlist->mnext=NULL;
		proctab[pid].store=store;
		proctab[pid].vhpno=4096;
		proctab[pid].vhpnpages=hsize;
		
	}
	else{
		restore(ps);
		return SYSERR;
	}

	restore(ps);
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process idSE_VIRTUAL_PAGE_NUM
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
