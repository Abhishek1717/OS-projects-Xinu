/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{
	STATWORD ps;
	disable(ps);
	if(nbytes==0 || proctab[currpid].vmemlist->mnext== (struct mblock *)NULL){
		restore(ps);
		return( SYSERR );
	}
	nbytes=(unsigned int)roundmb(nbytes);
//	kprintf("%d",nbytes);	
	struct mblock *q=proctab[currpid].vmemlist;
	struct mblock *p=q->mnext;
//	kprintf(" %d",q->mlen);
	 for (;
             p != (struct mblock *) NULL ;
             q=p,p=p->mnext){
//		kprintf("%d   ",p->mlen);
                if ( p->mlen == nbytes) {
                        q->mnext = p->mnext;
                        restore(ps);
                        return( (WORD *)p );
                } else if ( p->mlen > nbytes ) {
                        struct mblock *leftover;
			leftover = (struct mblock *)( (unsigned)p + nbytes );
                        q->mnext = leftover;
                        leftover->mnext = p->mnext;
                        leftover->mlen = p->mlen - nbytes;
                        restore(ps);
                        return( (WORD *)p );
                }

	}
	restore(ps);
	return (WORD*)(SYSERR);	
}
