#include<kernel.h>
#include<proc.h>
#include<lock.h>
#include<q.h>

extern int ctr1000;
int max_priority_write(int ldesc1){

	// has to return the max process which wants this lock  and is about to write
	struct lentry *lptr;
	lptr=&locker[ldesc1];
	int start=lptr->ltail;
	int max_prio=-100000000;  //*set to int min
	while(q[start].qprev!=lptr->lhead){
		start=q[start].qprev;
		if(q[start].qkey>max_prio && locker[ldesc1].lock_type_process_req[start]==WRITE)
		return q[start].qkey;
	}
	return max_prio;
}


void update_priority(int ldesc1,int max_prio,int delet_prio){
	
	struct lentry *lptr;
	lptr=&locker[ldesc1];
	int i=0;
//	int max_prio=max_process_prority_inqueue(ldesc1);  should be passes as parameter
	
	for(i=0;i<NPROC;i++){
		
		if(lptr->lock_holders[i]==0)
			continue;
		else{
			if(proctab[i].pinh==-1)
				proctab[i].pinh=proctab[i].pprio;
			if(proctab[i].pprio<max_prio || proctab[i].pprio==delet_prio)
				proctab[i].pprio=max_prio;
			if(proctab[i].pprio<proctab[i].pinh)
				proctab[i].pprio=proctab[i].pinh;	
			if(proctab[i].lwait!=-1){
				update_priority(proctab[i].lwait,max_prio,delet_prio);		
			
			}
		}
	}


}

int max_process_priority_inqueue(int ldesc1){

	struct lentry *lptr;
	lptr=&locker[ldesc1];
	int start=lptr->ltail;
	int max_prio=0;
	if(isbadlock(ldesc1))
		return SYSERR;
	while(q[start].qprev!=lptr->lhead){
		
		start=q[start].qprev;
		if(max_prio<proctab[start].pprio)
			max_prio=proctab[start].pprio;
	}
	
	return max_prio;

}


void pint(int ldesc1){
	int item=locker[ldesc1].lhead;
	while(q[item].qnext!=locker[ldesc1].ltail){
		printf("%d \n",q[item].qnext);
		item=q[item].qnext;
	}

		
}

SYSCALL lock(int ldesc1,int type, int priority){
	
	STATWORD ps;
	
	disable(ps);
	struct lentry *lptr;
	lptr=&locker[ldesc1];
	if(isbadlock(ldesc1) || lptr->lstate==LFREE || proctab[currpid].createlockhold[ldesc1]==DELETED)
		{ restore(ps);
		return SYSERR;}
//	kprintf("%d  ",proctab[currpid].pwaitret);	
		
	if(lptr->locktype==0 ||(lptr->locktype==READ && max_priority_write(ldesc1)<priority && type==READ )){
		if(type==READ)
			lptr->numreaders++;
	//	kprintf("Inside read or write");
		lptr->lock_holders[currpid]=1;
		lptr->locktype=type;
		lptr->lock_type_process_req[currpid]=type;
		proctab[currpid].lockhold[ldesc1]=1;
		proctab[currpid].lwait=-1;	
		restore(ps);		
		return OK;
		
	}

	else if(max_priority_write(ldesc1)==priority && type==READ && lptr->lock_type_process_req[q[lptr->ltail].qprev]==WRITE && lptr->locktype!=WRITE ){
		int j=q[lptr->ltail].qprev;
	//	kprintf("inside time");
		if(ctr1000-proctab[j].queuewaittime<0.4){
			lptr->numreaders++;
			lptr->lock_holders[currpid]==1;
			lptr->locktype=type;		
			lptr->lock_type_process_req[currpid]=type;
			proctab[currpid].lockhold[ldesc1]=1;
			proctab[currpid].lwait=-1;
			restore(ps);
			return OK;
		}
		
	}
	else{
		insert(currpid,lptr->lhead,priority);
		proctab[currpid].lwait=ldesc1;
		proctab[currpid].pstate=PRWAIT;
		int max_prio=max_process_priority_inqueue(ldesc1);
		proctab[currpid].queuewaittime=ctr1000;
		
		update_priority(ldesc1,max_prio,-1);
		lptr->lock_type_process_req[currpid]=type;
	//	kprintf(" %d  %d\n",currpid,lptr->locktype);	
	//	resched();
	//	pint(ldesc1);
		restore(ps);
		resched();
		if(proctab[currpid].pwaitret==DELETED)
			return DELETED;
		return OK;


	}
	//should we check for all the process(read) that are waiting when we call this from release 
	


}
