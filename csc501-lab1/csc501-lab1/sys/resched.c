/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

int a1=0,b1=0,c1=0;
unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

void setschedclass(int sched){
	if(sched==1)
		sched_mode=1;
	
	else{
		sched_mode=2;
		assign_quant();
	}

}

int getschedclass(){
	if(sched_mode==1)
		return RANDOMSCHED;
	else if(sched_mode==2)
		return LINUXSCHED;
	else
		return sched_mode;
}

int random_prob(){
	
	struct pentry *tptr;
	int i,newpid=0,total_prio=0;
	
	for(i=0;i<NPROC;i++){
		tptr=&proctab[i];
		if((i==currpid && proctab[currpid].pstate==1 )|| tptr->pstate==PRREADY){
			
			total_prio+=tptr->pprio;          // caluclating sum of all priorities looping throught the proctab
			
		}
	}
		             

	if(proctab[currpid].pstate==1)	
		insert(currpid,rdyhead,proctab[currpid].pprio);
	if(q[rdyhead].qnext==rdytail|| q[q[rdytail].qprev].qkey==0)                              //handle null process
                return 0;


	int process=rand()%total_prio;    	
	i=rdytail;
	
			
	while(q[i].qprev!=rdyhead){                   //select the next process based on their priority and random nnumber
		
		if(process<q[q[i].qprev].qkey){
			newpid=q[i].qprev;
			if(proctab[currpid].pstate==1)
				dequeue(currpid);
			return newpid;
//			break;
		}
		process-=q[q[i].qprev].qkey;
		i=q[i].qprev;
	}
	
	return newpid;
}


void assign_quant(){
	int i;
//	printf("assign");
	for(i=0;i<NPROC;i++){
	//	if(proctab[i].pstate==2)                   not required but check     
		if(epoch_time[i].quantum>0)	
			epoch_time[i].quantum=proctab[i].pprio+epoch_time[i].quantum/2;
		else
			epoch_time[i].quantum=proctab[i].pprio;
		epoch_time[i].goodness=proctab[i].pprio+epoch_time[i].quantum;	
	
	}

}

int goodness_max(){
	

	int i=rdyhead;
	int max_final=0,newpid=0;
//	printf("\n");
	if(proctab[currpid].pstate==PRCURR)
		insert(currpid,rdyhead,proctab[currpid].pprio);
	while(q[i].qnext!=rdytail){
		if(epoch_time[q[i].qnext].goodness>max_final){
	
			max_final=epoch_time[q[i].qnext].goodness;
			newpid=q[i].qnext;
		}
//		printf("hello");	
//		printf("%d",q[i].qnext);	
		i=q[i].qnext;
	}
//	printf("\n");
	if(proctab[currpid].pstate==PRCURR )
		dequeue(currpid);
	if(max_final==0)
		return -1;	
	return newpid;
}


int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/
		
		if(sched_mode==1){
			int newpid=random_prob();   //returns the pid of the next process to be executed			
			optr= &proctab[currpid];	

			if(newpid==currpid || isempty(rdyhead))
				{

					preempt=QUANTUM;
					return(OK);}	
			
			/*context switching*/
			if(optr->pstate==PRCURR){
				optr->pstate=PRREADY;
				insert(currpid,rdyhead,optr->pprio);
			}
			
			nptr=&proctab[(currpid=newpid)];
			nptr->pstate=PRCURR;
			dequeue(newpid);			
			
			#ifdef  RTCLOCK
		        preempt = QUANTUM;              /* reset preemption counter     */
			#endif
 
         		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask); 
        		 /* The OLD process returns here when resumed. */
         		return OK;	
			
		}
		
		else if(sched_mode==2){
			int x;
			if(preempt<=0){	
				preempt=0;
			}
			
			int priority=epoch_time[currpid].goodness-epoch_time[currpid].quantum;
		//	printf("%d\n",priority);
			epoch_time[currpid].quantum=preempt;
			if(preempt!=0)
				epoch_time[currpid].goodness=priority+epoch_time[currpid].quantum;
			else
				epoch_time[currpid].goodness=0;		
	//	 printf("%d  %d  %d   %d\n",epoch_time[49].quantum,epoch_time[48].quantum,epoch_time[47].quantum,epoch_time[46].quantum);		
			if(goodness_max()==-1){
                               assign_quant();        //start a new epoch

                        }
		
			int newpid=goodness_max();
			optr=&proctab[currpid];
		
	//		printf("%d ",newpid);
			if(newpid==-1){              //null process
				
			
				newpid=0;
				
				if(newpid==currpid || isempty(rdyhead))
					{  preempt=QUANTUM;
					return(OK);}
				if(optr->pstate==PRCURR){
					optr->pstate=PRREADY;
					insert(currpid,rdyhead,optr->pprio);
				}
				nptr=&proctab[(currpid=newpid)];
                        	nptr->pstate=PRCURR;
                        	dequeue(newpid);
                        	#ifdef  RTCLOCK
                        	preempt = QUANTUM;             
                       		 #endif
				
                        	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                  
                        	return OK;			
			}
			else{ 
				
					
				if(newpid==currpid)
					{ preempt= epoch_time[newpid].quantum;
					return(OK);}
				
				if(optr->pstate==PRCURR){
					optr->pstate=PRREADY;
					insert(currpid,rdyhead,optr->pprio);
				}
				nptr=&proctab[(currpid=newpid)];
                                nptr->pstate=PRCURR;
                                dequeue(newpid);

				preempt=epoch_time[newpid].quantum;
		//		 printf("%d \n ",preempt);
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                	
				 return OK;		
			

			}	
				
				
	
		}
		
		else{
		
			if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   		(lastkey(rdytail)<optr->pprio)) {
				return(OK);
			}
	
			/* force context switch */

			if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid,rdyhead,optr->pprio);
			}

			/* remove highest priority process at end of ready list */

			nptr = &proctab[ (currpid = getlast(rdytail)) ];
			nptr->pstate = PRCURR;		/* mark it currently running	*/
	
	
			#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
			#endif

			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
			/* The OLD process returns here when resumed. */
			return OK;
		}
}
