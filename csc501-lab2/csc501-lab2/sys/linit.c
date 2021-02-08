#include<kernel.h>
#include<proc.h>
#include<lock.h>
#include<q.h>

struct lentry locker[NLOCKS]; 
int nextlock;
	
void linit(){
	struct lentry *lptr;	
	int i,j;
	
	for(i=0;i<NLOCKS;i++){
		
		(lptr=&locker[i])->lstate=LFREE;
	
		lptr->ltail=1+ (lptr->lhead=newqueue());
		lptr->numreaders=0;
		lptr->locktype=0;
		lptr->max_prio_wait=0;	
		for(j=0;j<NPROC;j++){

			lptr->lock_holders[j]=0;
			lptr->lock_type_process_req[j]=0;
		}
	}
	nextlock=NLOCKS-1;	

}

