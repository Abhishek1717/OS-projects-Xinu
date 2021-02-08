#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int check_for_wait_time(int item,int lock_no){
	int temp=item;
	while(q[q[temp].qprev].qkey==q[item].qkey){
		
		if(locker[lock_no].lock_type_process_req[q[temp].qprev]==READ && proctab[q[temp].qprev].queuewaittime-proctab[item].queuewaittime<0.4){
	item=q[temp].qprev;
	return item;
	}
	temp=q[temp].qprev;
	}

	return item;
}
void acquire_lock(int lock_no){

	struct lentry *lptr;
	lptr=&locker[lock_no];
	
	
	if(q[lptr->ltail].qprev== lptr->lhead)   //none of them are waititng for the lock should we delete that
		{ 
			lptr->locktype=0;

		return;}
	else{
		//check for the wait time as well for processs with same priority	***************
		int item=q[lptr->ltail].qprev;
		int key=q[item].qkey;
		if(lptr->lock_type_process_req[item]==WRITE)
			item=check_for_wait_time(item,lock_no);
		item=dequeue(item);	
		ready(item,RESCHNO);	
		lptr->locktype=lptr->lock_type_process_req[item];
		lptr->lock_holders[item]=1;
		proctab[item].lockhold[lock_no]=1;
		proctab[item].lwait=-1;
		item=q[lptr->ltail].qprev;
		if(lptr->locktype==READ){
			lptr->numreaders++;
		//	kprintf("%d  %d\n",item,q[item].qprev);	
			while(item!=lptr->lhead && lptr->lock_type_process_req[item]==READ){	
				item=dequeue(item);
				ready(item,RESCHNO);
				lptr->numreaders++;
				lptr->lock_holders[item]=1;
				proctab[item].lockhold[lock_no]=1;
				item=q[lptr->ltail].qprev;
				proctab[item].lwait=-1;
			}
		
		}
//		printf("hell");
		int max_prio=-1;
		max_prio=max_process_priority_inqueue(lock_no);
		if(max_prio>0)
			update_priority(lock_no,max_prio,-1);
	
	
	}

}

  

SYSCALL releaseall(int numlocks,int ldesc1,...)
{
	STATWORD ps;
	int output=OK;
	int i,lock_no;
	disable(ps);
	for(i=0;i<numlocks;i++){
		
		lock_no=*(&ldesc1 + i);
		
		if(locker[lock_no].lstate==LFREE || isbadlock(lock_no)|| proctab[currpid].lockhold[lock_no]==0)
			{	output=SYSERR;
				continue;
		}

		else if(locker[lock_no].locktype==READ){
			locker[lock_no].lock_holders[currpid]=0;
			locker[lock_no].numreaders--;
			proctab[currpid].lockhold[lock_no]==0;
			locker[lock_no].lock_type_process_req[currpid]=0;
			if(locker[lock_no].numreaders==0){
				acquire_lock(lock_no);		// allocate the lock to process which ahs highest priority in the queue
			}
			
		}
		else if(locker[lock_no].locktype==WRITE){
			locker[lock_no].lock_holders[currpid]=0;
			proctab[currpid].lockhold[lock_no]==0;
			locker[lock_no].lock_type_process_req[currpid]=0;
			acquire_lock(lock_no);			//allocate the lock to a new process;
							
				
		}
		if(i==0 && proctab[currpid].pinh!=-1 )
			proctab[currpid].pprio=proctab[currpid].pinh;

	}
 

	for(i=0;i<NLOCKS;i++){
		
		if(proctab[currpid].lockhold[i]==1){
			int max_prio=-1;
                max_prio=max_process_priority_inqueue(lock_no);
                if(max_prio>0)
                        update_priority(lock_no,max_prio,-1);
	
		
	}
		
	}
	restore(ps);
	
	
//	resched();


return output;

}
