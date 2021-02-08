#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <stdio.h>
#include<lab1.h>
#define LOOP	50

int prA, prB, prC;
int proc_a(char), proc_b(char), proc_c(char);
int proc(char);
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;
volatile int s = 0;
extern int a1,b1,c1;
extern int ctr1000;
int main() {
	int i;
	int count = 0;
	char buf[8];
//	sched_mode=2;
	srand(1234);
//	preempt=0;
	kprintf("Please Input:\n");
	while ((i = read(CONSOLE, buf, sizeof(buf))) < 1)
		;
	buf[i] = 0;
	s = atoi(buf);
	kprintf("Get %d\n", s);
	int j;

	// RANDOMSCHED
	if (s < 2) {
		setschedclass(RANDOMSCHED);
		printf("%d",getschedclass());
		prA = create(proc_a, 2000, 10, "proc A", 1, 'A');
		prB = create(proc_b, 2000, 20, "proc B", 1, 'B');
		prC = create(proc_c, 2000, 30, "proc C", 1, 'C');
		resume(prA);
		resume(prB);
		resume(prC);
		sleep(10);
		kill(prA);
		kill(prB);
		kill(prC);

		kprintf("\nTest Result: A = %d, B = %d, C = %d\n", a_cnt, b_cnt, c_cnt);
	}
//	 LINUXSCHED
	else {printf("%d",getschedclass());
		setschedclass(LINUXSCHED);
printf("%d",getschedclass());
		resume(prA = create(proc, 2000, 50, "proc A", 1, 'A'));
		resume(prB = create(proc, 2000, 50, "proc B", 1, 'B'));
		resume(prC = create(proc, 2000, 50, "proc C", 1, 'C'));

		while (count++ < LOOP) {
			kprintf("M");
			for (i = 0; i < 1000000; i++)
				;
		}
        kprintf("\n");
	}
// kprintf("%d  %d %d",a1,b1,c1);
	return 0;
}

int proc_a(char c) {
	int i;
	kprintf("Start... %c\n", c);
	b_cnt = 0;
	c_cnt = 0;

	while (1) {
		
		for (i = 0; i < 10000; i++)
			;
		a_cnt++;
	}
	return 0;
}

int proc_b(char c) {
	int i;
	kprintf("Start... %c\n", c);
	a_cnt = 0;
	c_cnt = 0;

	while (1) {
		
		for (i = 0; i < 10000; i++)
			;
		b_cnt++;		

	}
	return 0;
}

int proc_c(char c) {
	int i;
	kprintf("Start... %c\n", c);
	a_cnt = 0;
	b_cnt = 0;

	while (1) {
		
		for (i = 0; i < 10000; i++)
			;
		c_cnt++;
	}
	return 0;
}


int proc(char c) {
	int i;
	int count = 0;

	while (count++ < LOOP) {
		kprintf("%c", c);
//		kprintf(" %d ",preempt);
		for (i = 0; i < 1000000; i++)
			;
	}
	return 0;
}
