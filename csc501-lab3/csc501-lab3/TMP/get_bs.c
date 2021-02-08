#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>



int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
    STATWORD ps;
    disable(ps);
	
    if(npages<=0 || npages>128 || bs_id<0 || bs_id>15){
	restore(ps);
	return SYSERR;
    }

    if(bsm_tab[bs_id].bs_status==BSM_MAPPED && bsm_tab[bs_id].bs_shared==0){
	restore(ps);
	return SYSERR;}
    else if(bsm_tab[bs_id].bs_status==BSM_MAPPED){
	restore(ps);
	bsm_tab[bs_id].bs_count++;
	return bsm_tab[bs_id].bs_npages;
    }
    else {
		bsm_tab[bs_id].bs_count++;
bsm_tab[bs_id].bs_shared=1;
		bsm_tab[bs_id].bs_status=BSM_MAPPED;
		bsm_tab[bs_id].bs_npages=npages;
		restore(ps);
		return npages;
	}
	
    

    // need to add one case whenn the bsd_t is mapped but this process has 0 pages in this bsd_it
   
    return npages;

}


