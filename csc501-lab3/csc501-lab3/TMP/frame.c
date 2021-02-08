/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
int sc_pointer=0;
int sc_frames[NFRAMES];
fr_map_t frm_tab[NFRAMES];
extern int page_replace_policy;
SYSCALL init_frm()
{
  STATWORD ps;
  disable(ps);
  int i=0;	
  for(i=0;i<NFRAMES;i++){
	frm_tab[i].fr_pid=-1;
	frm_tab[i].fr_status=FRM_UNMAPPED;
  	frm_tab[i].fr_vpno=-1;
  	frm_tab[i].fr_refcnt=0;
  	frm_tab[i].fr_type=-1;
 	frm_tab[i].fr_dirty=0;
	frm_tab[i].freq=0;
	sc_frames[i]=-1;
  }
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */

int check_ref(int frame_no){

	unsigned int vpno=frm_tab[frame_no].fr_vpno;	
	unsigned int vaddr=vpno*NBPG;
	virt_addr_t *vadd= (virt_addr_t *)&vaddr;
		unsigned int pt_off=vadd->pt_offset;
		unsigned int pd_off=vadd->pd_offset;

	int pid=frm_tab[frame_no].fr_pid;

	pd_t *pd=proctab[pid].pdbr+pd_off*sizeof(pd_t);
	pt_t *pt = pd->pd_base*NBPG+ sizeof(pt_t)*pt_off;
//	kprintf(" %d  --",frame_no);
	if(pt->pt_acc==0)
		{return 0;}
	else{
		pt->pt_acc=0;
		return 1;
	}

}
int free_SC(){
	int i=sc_pointer%NFRAMES;
	while(1){
//		kprintf("%d",i%NFRAMES);
		if(sc_frames[i%NFRAMES]!=-1  && frm_tab[sc_frames[i%NFRAMES]].fr_type==FR_PAGE && sc_frames[i%NFRAMES]>4)
		if(check_ref(sc_frames[i%NFRAMES])==0)
			{sc_pointer=i%NFRAMES;
			 sc_pointer=sc_pointer%NFRAMES;
				int y=sc_frames[i%NFRAMES];
//				kprintf(" %d",sc_pointer%NFRAMES);
				sc_frames[i%NFRAMES]=-1;
//				kprintf("%d   %dhello",y,i%NFRAMES);
				return y;
		}

		i++;
	}

}

int free_LFU(){

	int avail=-1;
	int freq=-1;
	int i=0;
	unsigned int vpno=0;
	
	for(i=8;i<12;i++){
//	kprintf(" %d ",frm_tab[i].freq);
}
	for(i=5;i<NFRAMES;i++){
		
		if(frm_tab[i].fr_type==FR_PAGE){
			
			if((freq>frm_tab[i].freq || freq==-1) || (frm_tab[i].freq==freq &&frm_tab[i].fr_vpno>vpno)){
//				kprintf("-- %d ",i);
				avail=i;
				freq=frm_tab[i].freq;
				vpno=frm_tab[i].fr_vpno;


			}

		}
	}
//	kprintf("\n %d \n",avail);
	//if(avail==-1)
	//	avail=10;
	return avail;

}

SYSCALL get_frm(int* avail)
{
  STATWORD ps;
  disable(ps);
  *avail=-1;
  int i=0;
  for(i=0;i<NFRAMES;i++){
       if(frm_tab[i].fr_status==FRM_UNMAPPED){
		*avail=i;

		break;
	}

  }
  if(*avail==-1){
	
	if(page_replace_policy==SC){
		*avail=free_SC();
		
		
	}
	else if(page_replace_policy==LFU){
		*avail=free_LFU();
		 

	}
	kprintf("Replaced FRAME: %d\n",*avail);
	free_frm(*avail);


  }
	frm_tab[*avail].freq++;
  // need to update this when there are page replacement piolicies;
  restore(ps);
  return OK;

}
/*


*/
/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
  STATWORD ps;
  disable(ps);
  if(i>NFRAMES || i<0 || frm_tab[i].fr_status==FRM_UNMAPPED){
  	restore(ps);
	return SYSERR;
  }
  else{
	int store,page;
	if(bsm_lookup(frm_tab[i].fr_pid,frm_tab[i].fr_vpno*NBPG,&store,&page)==OK){
		write_bs((i+FRAME0)*NBPG,store,page);

	}

	 page=frm_tab[i].fr_vpno;
	unsigned int vaddr=NBPG*page;
	virt_addr_t *vadd= (virt_addr_t *)&vaddr;
              
                unsigned int pt_off=vadd->pt_offset;
                unsigned int pd_off=vadd->pd_offset;

                pd_t *pd = proctab[currpid].pdbr + (pd_off)*sizeof(pd_t);
		pt_t  *pt= pd->pd_base*NBPG+ (pt_off)*sizeof(pt_t);
		
		pt->pt_base=0;
		pt->pt_pres=0;
		pt->pt_write=0;
		pt->pt_acc=0;
  		
	frm_tab[i].fr_status=FRM_UNMAPPED;
	frm_tab[i].fr_vpno=-1;
	frm_tab[i].fr_pid=-1;
	frm_tab[i].fr_type=-1;
	}
  restore(ps);
  return OK;
}



