#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int globalpagetable[4] ;

int page_table(int pid){
	
	int frame_no;
	STATWORD ps;
	disable(ps);
	if(get_frm(&frame_no)==SYSERR){
		restore(ps);
		return SYSERR;
	}
	else{
			
		frm_tab[frame_no].fr_status=FRM_MAPPED;
		frm_tab[frame_no].fr_pid=pid;
		frm_tab[frame_no].fr_vpno=-1;
		frm_tab[frame_no].fr_refcnt=1;
		frm_tab[frame_no].fr_type=FR_TBL;
		frm_tab[frame_no].fr_dirty=CLEAN;
	
		int page_no=FRAME0+frame_no;
		pt_t *page_table;
		int i;
		for(i=0;i<1024;i++){
			page_table=page_no*NBPG + i*sizeof(pt_t);
			page_table->pt_pres= 0;
			page_table->pt_write= 1;     
			page_table->pt_user= 0;           
			page_table->pt_pwt= 0;       
			page_table->pt_pcd=0;  
			page_table->pt_acc=0;  
			page_table->pt_dirty=0;      
			page_table->pt_mbz=0;       
			page_table->pt_global=0;   
			page_table->pt_avail=0;    
			page_table->pt_base=0;    	
			
		}
	}
	enable(ps);
	return frame_no;

}


int page_directory(int pid){
	 int frame_no;
        STATWORD ps;
        disable(ps);
        if(get_frm(&frame_no)==SYSERR){
                restore(ps);
                return SYSERR;
        }
        else{

                frm_tab[frame_no].fr_status=FRM_MAPPED;
                frm_tab[frame_no].fr_pid=pid;
                frm_tab[frame_no].fr_vpno=-1;
                frm_tab[frame_no].fr_refcnt=1;
                frm_tab[frame_no].fr_type=FR_DIR;
                frm_tab[frame_no].fr_dirty=CLEAN;

                int page_no=FRAME0+frame_no;
		proctab[pid].pdbr=page_no*NBPG;
                pd_t *page_table;
		int i;
                for(i=0;i<1024;i++){
                        page_table=page_no*NBPG + i*sizeof(pd_t);
                       
			if(i<4){
				page_table->pd_pres=1 ;
	                        page_table->pd_write= 1;
				page_table->pd_base=i+FRAME0;
			}
			else{
			page_table->pd_pres= 0;
                        page_table->pd_write= 1;
                        page_table->pd_user= 0;
                        page_table->pd_pwt= 0;
                        page_table->pd_pcd=0;
                        page_table->pd_acc=0;
                        page_table->pd_fmb=0;
                        page_table->pd_mbz=0;
                        page_table->pd_global=0;
                        page_table->pd_avail=0;
                        page_table->pd_base=0;
			}
                }
        }
        restore(ps);
        return frame_no;


}

int page_global(){

	int i;
	for(i=0;i<4;i++){
		int frame_no;
		
		frame_no=page_table(0);
		int j;
		int page_no=frame_no+FRAME0;
		pd_t  *p_d;
		p_d=proctab[0].pdbr+ i*sizeof(pd_t);
		p_d->pd_pres=1;
		p_d->pd_base=page_no;
		pt_t *page_table;
		for(j=0;j<1024;j++){
			
			page_table=page_no*NBPG + j*sizeof(pt_t);
                        page_table->pt_pres= 1;
                        page_table->pt_global=1;
                        page_table->pt_base=i*FRAME0+j;
		
		}


	}
return OK;

}
