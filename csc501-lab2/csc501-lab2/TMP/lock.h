# define NLOCKS	50

# define READ  2
# define WRITE 3

#define LFREE '1'
#define LUSED '2'

struct lentry{

	char lstate;
	int locktype;
	int lhead;
	int ltail;
	int max_prio_wait;
	int numreaders;
	int lock_holders[NPROC];        // procesess which are holding locks
	int lock_type_process_req[NPROC];   // is this required


};

extern struct lentry locker[];
extern int nextlock;
extern void update_priority(int ,int ,int );
extern int max_process_priority_inqueue(int ldesc1);
#define isbadlock(l)  (l<0||l>=NLOCKS)


