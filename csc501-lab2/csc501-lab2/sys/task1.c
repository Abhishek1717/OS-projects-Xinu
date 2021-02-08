/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>


extern int ctr1000;

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }


void reader3 (char *msg, int lck)
{
        int     ret;

        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 2s\n", msg);
        sleep(2);
	kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void writer3 (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 2s\n", msg);
//        kprintf("%d",proctab[46].pprio);
        sleep (1);
       kprintf ("  %s: to release lock\n", msg);	
       releaseall (1, lck);
}

void writer4 (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
	lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 2s\n", msg);
        sleep (2);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);


}	



void locktask1(){

	int lck,lck1;
	
	int rd1,rd2,wr1,wr2;
	
	lck=lcreate();
	lck1=lcreate();	


	rd1 = create(reader3, 2000, 5, "reader3", 2, "reader A", lck1);
        wr1 = create(writer3, 2000, 20, "writer3", 2, "writer B", lck);
	wr2 = create(writer4 ,2000, 50, "writer4", 2, "writer C", lck1 );

	resume(rd1);
	sleep(1);
	resume(wr1);
	resume(wr2);
	sleep(1);
	sleep(5);

        ldelete(lck);
        ldelete(lck1);


        kill(rd1);
        kill(wr1);
        kill(wr2);


}
//------------------------------------------------------------------------------

void reader3s (char *msg, int sem)
{
        int     ret;

        kprintf ("  %s: to acquire semaphore\n", msg);
	wait(sem);
        
        kprintf ("  %s: acquired semaphore,sleep 2s\n", msg);
        sleep(2);
	kprintf ("  %s: to release semaphore\n", msg);
	signal(sem);
       
}

void writer3s (char *msg, int sem)
{
        kprintf ("  %s: to acquire semaphore\n", msg);
        wait(sem);
	kprintf ("  %s: acquired semaphore, sleep 2s\n", msg);
	sleep(1);
	kprintf ("  %s: to release semaphore\n", msg);
        signal(sem);
}

void writer4s (char *msg, int sem, int sem1)
{
        kprintf ("  %s: to acquire lock\n", msg);
        wait(sem1);
        wait(sem);
        kprintf ("  %s: acquired semaphore, sleep 2s\n", msg);
        sleep (2);
        kprintf ("  %s: to release semaphore\n", msg);
        signal(sem1);
	signal(sem);

}


void semtask1(){

	int sem,sem1;

	sem=screate(1);
	sem1=screate(1);

	int rd1,wr1,wr2;

	rd1 = create(reader3s, 2000, 5, "reader3s", 2, "readers A", sem1);
        wr1 = create(writer3s, 2000, 20, "writer3s", 2, "writers B", sem);
        wr2 = create(writer4s ,2000, 50, "writer4s", 3, "writers C", sem1);

		
	resume(rd1);
	sleep(1);
        resume(wr1);
        resume(wr2);
        sleep(1);
        sleep(5);
	sdelete(sem);
	sdelete(sem1);


	kill(rd1);
	kill(wr1);
	kill(wr2);

}

void task1(){

	kprintf("Semaphore implementation :\n");
	semtask1();	


	kprintf("Lock implementation :\n");
	locktask1();
	
	

}

int main( )
{
	task1();
	sleep(15);
	


        shutdown();
}


