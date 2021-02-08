/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * ");
 *
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
extern int page_replace_policy;
SYSCALL pfint()
{
	STATWORD ps;
	disable(ps);
//	kprintf("\niittttiiiiiiiiiiii");
	unsigned long vaddr=read_cr2();
	int store,pageth;
	//kprintf("cr3: %d    ",read_cr3());
	//kprintf(" %d  ",proctab[0].pdbr);
	bsm_lookup(currpid,vaddr,&store,&pageth);
//	kprintf("\n\n\nn\\n%d   %d\n\n\nn\n",store,pageth);
	///if mapping doesn't exist kill the process
	if(store==-1||pageth==-1){
		kill(currpid);
		restore(ps);
		return SYSERR;
	}
	else{
		virt_addr_t *vadd= (virt_addr_t *)&vaddr;
		unsigned int pg_off= vaddr;
		unsigned int pt_off=vadd->pt_offset;
		unsigned int pd_off=vadd->pd_offset;
	
		pd_t *pd = proctab[currpid].pdbr + (pd_off)*sizeof(pd_t);
		

		if(pd->pd_pres!=1){
		//	kprintf("\n\n\ninside dire");	
			int frame_no=page_table(currpid);
			pd->pd_base=FRAME0+ frame_no;
			pd->pd_pres=1;
			frm_tab[frame_no].fr_type=FR_TBL;
//			kprintf("\n %d \n",frame_no);
			frm_tab[frame_no].fr_refcnt++;
		}
		
		pt_t *pt = pd->pd_base*NBPG + (pt_off)*sizeof(pt_t);

		if(pd->pd_pres==1){
			
	
			int frame_no;
			if(pt->pt_pres!=1){
			
				get_frm(&frame_no);
		//		kprintf("\n  %d \n",frame_no);				
				pt->pt_pres=1;
				pt->pt_write=1;
				pt->pt_base= FRAME0+frame_no;
				
			}
			else{
				frame_no =pt->pt_base -FRAME0;
				frm_tab[frame_no].fr_refcnt++;
	//			kprintf("page table");
			}
				
				frame_no=pt->pt_base -FRAME0;
				frm_tab[frame_no].fr_status=FRM_MAPPED;
				frm_tab[frame_no].fr_pid=currpid;
				frm_tab[frame_no].fr_type=FR_PAGE;
			
				frm_tab[frame_no].fr_vpno=vaddr/NBPG;

				if(page_replace_policy==SC ){
		//			kprintf(" %d  %d\n",frame_no,sc_pointer%NFRAMES);
					sc_frames[sc_pointer%NFRAMES]=frame_no;
					sc_pointer++;	
					
				}
		}
	//	kprintf("\n%d",(pt->pt_base)*NBPG);	
		
		read_bs((char *)((pt->pt_base)*NBPG),store, pageth);

		
		
		write_cr3(proctab[currpid].pdbr);
			restore(ps);
		return OK;
	}

  return OK;
}


