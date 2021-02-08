/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
bs_map_t bsm_tab[16];
SYSCALL init_bsm()
{	int i=0;
	STATWORD ps;
	disable(ps);
	for(;i<16;i++){
		bsm_tab[i].bs_status=BSM_UNMAPPED;
		bsm_tab[i].bs_pid=-1;
		bsm_tab[i].bs_vpno=-1;
		bsm_tab[i].bs_npages=0;
		bsm_tab[i].bs_vpno=-1;
		bsm_tab[i].bs_sem=-1;
		bsm_tab[i].bs_shared=-1;
		bsm_tab[i].bs_count=0;
	}
	
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{	
	int i;
	STATWORD ps;
	disable(ps);
	for(i=0;i<=15;i++){
		if(bsm_tab[i].bs_status!=BSM_MAPPED){
			*avail=i;
			enable(ps);
			return OK;}
	}
	restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	STATWORD ps;
	disable(ps);
	if(i<0 || i>15){
		enable(ps);
		return SYSERR;
	}
	else{
                bsm_tab[i].bs_status=BSM_UNMAPPED;
                bsm_tab[i].bs_pid=-1;
                bsm_tab[i].bs_vpno=-1;
                bsm_tab[i].bs_npages=0;
                bsm_tab[i].bs_vpno=-1;
                bsm_tab[i].bs_sem=-1;
                bsm_tab[i].bs_shared=-1;
                bsm_tab[i].bs_count=0;
        }
	restore(ps);
	return OK;
	
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, unsigned long vaddr, int* store, int* pageth)
{
	int i;
	STATWORD ps;
	disable(ps);
	
	long  vpno= vaddr/NBPG;
//	kprintf("%ld",vaddr);
//	kprintf("virtual page no: %d ",vpno);
//	kprintf("actial %d",proctab[pid].process_store_vpno[1]);
//	kprintf("----%d----",proctab[pid].process_bsm_npages[1]);
	for(i=0;i<=15;i++){
		if(bsm_tab[i].bs_status==BSM_MAPPED){
			if(proctab[pid].process_store_vpno[i]<=vpno && proctab[pid].process_bsm_npages[i]+proctab[pid].process_store_vpno[i]>vpno )
				break;
		}	

	}
	if(i==16){
//		kprintf("are you here");
		*store=-1;
		*pageth=-1;
		restore(ps);
		return SYSERR;
	}
	else{
	*store=i;
	*pageth= vpno-proctab[pid].process_store_vpno[i];	
	restore(ps);
	return OK;
	}
	
	
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
		
	STATWORD ps;
	disable(ps);
//	kprintf("in bsm");
	if(bsm_tab[source].bs_status!=BSM_MAPPED){
//		kprintf("hi");
		bsm_tab[source].bs_status=BSM_MAPPED;
		bsm_tab[source].bs_pid=pid;
		bsm_tab[source].bs_npages=npages;
	//	bsm_tab[source].bs_shared=1;
		bsm_tab[source].bs_count++;
		proctab[pid].process_store_vpno[source]=vpno;
		proctab[pid].process_bsm_npages[source]=npages;
//		kprintf("done bsm");
		restore(ps);
		return OK;
	}
	else if(bsm_tab[source].bs_status==BSM_MAPPED && bsm_tab[source].bs_shared!=0) {
		bsm_tab[source].bs_count++;
		bsm_tab[source].bs_shared=1;
		proctab[pid].process_store_vpno[source]=vpno;
		proctab[pid].process_bsm_npages[source]=npages;
		restore(ps);
		return OK;
	
	}
	else{
		restore(ps);
		return SYSERR;
	}
		
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	STATWORD ps;
	disable(ps);
	int page_no=vpno*NBPG;
	int store,page;
	bsm_lookup(pid,page_no,&store,&page);
	//kprintf("here ");	
	int i;
	for(i=0;i<NFRAMES;i++){
		
		if(frm_tab[i].fr_status==FRM_MAPPED && frm_tab[i].fr_pid==pid)// && frm_tab[i].fr_vpno>=proctab[pid].process_store_vpno[store])
				{// if( proctab[pid].process_bsm_npages[store]+proctab[pid].process_store_vpno[store]>frm_tab[i].fr_vpno && frm_tab[i].fr_type==FR_PAGE){	
	//	kprintf("ikkada");
		int st,pt;
		bsm_lookup(pid,frm_tab[i].fr_vpno*NBPG,&st,&pt);
		write_bs((i+FRAME0)*NBPG,st,pt);

//}
	
	}



}
	
	bsm_tab[store].bs_count--;
	if(bsm_tab[store].bs_count==0){
		bsm_tab[store].bs_status=BSM_UNMAPPED;
		bsm_tab[store].bs_shared=-1;
		bsm_tab[store].bs_npages=0;
		bsm_tab[store].bs_vpno=-1;
		bsm_tab[store].bs_pid=-1;

	}
		
	
	
	restore(ps);
	return(OK);

}


