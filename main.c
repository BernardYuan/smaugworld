#include<stdio.h>
#include<wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<curses.h>
#include<time.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/time.h>
#include<sys/resource.h>

int semID;

union semun{
  int val;
  struct semid_ds *buf;
  ushort *array;
}seminfo;

struct timeval startTime;

//termination condition
#define MAX_COWSHEEP 14
#define MAX_TREASUREHUNTER 12
#define MAX_THIEF 15
#define MAX_JEWEL 800

//# of sheep and cows in one meal
#define SHEEP_IN_MEAL 2
#define COW_IN_MEAL 2
// definitions of the semaphores
#define MTX_DRAGONWAKEUP 0
#define MTX_DRAGONEAT 1
#define MTX_MEAL 2
#define SEM_SHEEPINVALLEY 3

//pointers to shared memory
int *SheepInValley = NULL;
int  SheepInValleyFlag = 0;
int *numMeals = NULL;
int  numMealsFlag = 0;

// semaphore buffers 
// dragon sleep:
struct sembuf WaitDragonWakeUp={MTX_DRAGONWAKEUP, -1, 0};
struct sembuf SignalDragonWakeUp={MTX_DRAGONWAKEUP, 1, 0};

//dragon eat:
struct sembuf WaitDragonEat = {MTX_DRAGONEAT, -1, 0};
struct sembuf SignalDragonEat = {MTX_DRAGONEAT, 1, 0};

//mutex on meal
struct sembuf WaitMeal = {MTX_MEAL, -1, 0};
struct sembuf SignalMeal = {MTX_MEAL, 1, 0};

// sheep in valley
struct sembuf WaitSheepInValley={SEM_SHEEPINVALLEY, -1, 0};
struct sembuf SignalSheepInValley={SEM_SHEEPINVALLEY, 1, 0};


void initialize();
void releaseResource();
void semctlChecked(int semID, int semNum, int flag, union semun seminfo); 
void semopChecked(int semID, struct sembuf *operation, unsigned num);

void initialize() {
	semID = semget(IPC_PRIVATE, 4, 0666 | IPC_CREAT);
	seminfo.val = 0;
  semctlChecked(semID, SEM_SHEEPINVALLEY, SETVAL, seminfo);

  seminfo.val = 1;
	semctlChecked(semID, MTX_DRAGONWAKEUP, SETVAL, seminfo);
  semctlChecked(semID, MTX_DRAGONEAT, SETVAL, seminfo);
  semctlChecked(semID, MTX_MEAL, SETVAL, seminfo);

  //allocate shared memory
  if(SheepInValleyFlag=shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666) < 0) {
    printf("CREATE SheepInValleyFlag error\n");
    exit(1);
  }
  else {
    printf("CREATE SheepInValleyFlag Success\n");
  }

  if(numMealsFlag = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666) < 0) {
    printf("CREATE numMealsFlag error\n");
    exit(1);
  }

  //attach shared memory
  if((SheepInValley = shmat(SheepInValleyFlag, NULL, 0))==(int *) -1) {
    printf("Attach Shared Memory SheepInValley Error\n");
    exit(1);
  }
  else {
    printf("Attach Shared Memory SheepInValley Success\n");
  }

 if((numMeals = shmat(numMealsFlag, NULL, 0)) == (int *) -1) {
   printf("Attach Shared Memory numMeals Error\n");
   exit(1);
 }
 else {
   printf("Attach Shared Memory numMeals Success\n");
 }
}

void semctlChecked(int semID, int semNum, int flag, union semun seminfo) {
	if(semctl(semID, semNum, flag, seminfo)==-1) {
		if(errno != EIDRM) {
			printf("semaphore control failed: simulation terminating\n");
			printf("errno: %8d\n", errno);
			exit(2);
		}
		else {
			exit(3);
		}
	}
}

void semopChecked(int semID, struct sembuf *operation, unsigned num) {
	if(semop(semID, operation, num) == -1) {
		if(errno != EIDRM) {
			printf("semaphore operation failed: simulation terminating\n");
			printf("errno: %8d\n", errno);
			exit(2);
		}
		else {
			exit(3);
		}
	}
}

void smaug() {
	int time = 0;
	while(1) {
		printf("smaug is going to sleep\n");
		semopChecked(semID, &WaitDragonWakeUp, 1);
		printf("smaug wakes up and play for a while\n");
		time ++;
		usleep(1000000);
		if(time==3) exit(0);
	}
}

void sheep() {
}
int main(void) {
	initialize();
	pid_t result = fork();

	if(result<0) {
		printf("fork error\n");
		exit(1);
	}
	if(result==0) {
		smaug();
	}
	else {	
		int i;
		for(i=0;i<5;i++) {
			usleep(2000000);
			semopChecked(semID, &SignalDragonWakeUp, 1);
		}
		int status;
		waitpid(-1,&status,0);
		printf("all processes has exited\n");
	}
}
