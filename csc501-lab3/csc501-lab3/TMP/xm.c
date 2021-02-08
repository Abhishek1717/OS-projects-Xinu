/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  STATWORD ps;
  disable(ps);

  if(source<0 || source >15 || npages>128 ||npages<1 ) {
	restore(ps);
	return SYSERR;	
  }
 /* else if(bsm_tab[source].bs_shared==0){
	restore(ps);
	return SYSERR;
  }*/
  else{
	if(npages>bsm_tab[source].bs_npages)
		{restore(ps);return SYSERR;
		}
	if(bsm_map(currpid,virtpage,source,npages)==OK){
//		kprintf("\n\n\n\n %dmap successful",virtpage);
		restore(ps);
		return OK;
	}
	else{
		restore(ps);
		return SYSERR;
	}

  }
  restore(ps);
  return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
  STATWORD ps;
  disable(ps);

  if(bsm_unmap(currpid,virtpage,0)==OK){
	restore(ps);
	return OK;	
  }
  else{
  	restore(ps);
	return SYSERR;
  }
}
