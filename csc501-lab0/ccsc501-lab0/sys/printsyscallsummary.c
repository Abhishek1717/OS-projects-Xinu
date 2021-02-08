#include<stdio.h>
#include<kernel.h>
#include<proc.h>
#include<lab0.h>
int track;

char call_names[27][15]={"freemem","chprio","getpid","getprio","gettime","kill","receive",
               "recvclr","recvtim","resume","scount","sdelete","send",
               "setdev","setnok","screate","signal","signaln","sleep",
               "sleep10","sleep100","sleep1000","sreset","stacktrace",
               "suspend","unsleep","wait"};

void syscallsummary_stop(){
           
  	track=0;
   
}

void syscallsummary_start(){
	int pro,i;
	 for( pro=0;pro<NPROC;pro++){
                for( i=0;i<27;i++){
                 tracker[pro].freq[i]=0;
	         tracker[pro].time[i]=0;               
		 }
          }
        

	track=1;
        
}

int receivefreq=0;
unsigned long receiveavg=0;
void printsyscallsummary(){
	
	
      kprintf("void printsyscallsummary()\n");
	int pro,i;
        for( pro=0;pro<NPROC;pro++){
		 if(tracker[pro].freq[i])
                kprintf("Process [pid:%d]\n",pro);

		for( i=0;i<27;i++){    
	        if(tracker[pro].freq[i]){
 		kprintf("	Syscall: %s,",call_names[i]);
		kprintf(" count: %d, ",  tracker[pro].freq[i]);
		kprintf("average execution time: %d (ms)\n",  tracker[pro].time[i]/tracker[pro].freq[i]);
			}
		}
        }

}




