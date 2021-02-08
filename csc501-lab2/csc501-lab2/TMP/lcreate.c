#include<kernel.h>
#include<proc.h>
#include<lock.h>

LOCAL int newlock();


SYSCALL  lcreate(){
	
	STATWORD(ps);
	
	disable(ps);
	int lock;
	
	if((lock=newlock())==SYSERR){
		restore(ps);		
		return(SYSERR);
	}
		
	
	restore(ps);
	return lock;
}



LOCAL int newlock(){

	int lock;
	int i;

	for(i=0;i<NLOCKS;i++){
		lock=nextlock--;
		if(nextlock<0)
			nextlock=NLOCKS-1;
	
		if(locker[lock].lstate==LFREE){     //not only freecheck for deleted ones as well
			locker[lock].lstate=LUSED;
			return(lock);
		}
	}
	return(SYSERR);




}
