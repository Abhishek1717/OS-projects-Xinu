#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
   STATWORD ps;
   disable(ps);

   if(bs_id<0 && bs_id>=16){
	restore(ps);
	return SYSERR;
   }
   if(bsm_tab[bs_id].bs_shared==0){
   	bsm_tab[bs_id].bs_status=BSM_UNMAPPED;
   	bsm_tab[bs_id].bs_npages=0;
   	bsm_tab[bs_id].bs_pid=-1;
	bsm_tab[bs_id].bs_vpno=-1;
	bsm_tab[bs_id].bs_count=0;
	bsm_tab[bs_id].bs_shared=-1;
	proctab[currpid].process_store_vpno[bs_id]=-1;
	proctab[currpid].process_bsm_npages[bs_id]=0;
   }
   else{
	bsm_tab[bs_id].bs_count--;
	proctab[currpid].process_store_vpno[bs_id]=-1;
        proctab[currpid].process_bsm_npages[bs_id]=0;
	if(bsm_tab[bs_id].bs_count==0){
		bsm_tab[bs_id].bs_status=BSM_UNMAPPED;
        	bsm_tab[bs_id].bs_npages=0;
        	bsm_tab[bs_id].bs_pid=-1;
       		bsm_tab[bs_id].bs_vpno=-1;
       		bsm_tab[bs_id].bs_shared=-1;

	}
   }


  // might need to UNMAP IN BSMUNMAP 
   
   
   restore(ps);
   return OK;

}
