#include <errno.h> 
#include <wait.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/resource.h> 

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
#define MTX_SHEEPINVALLEY 3
#define SEM_SHEEPINVALLEY 4

int semID;
union semun{
  int val;
  struct semid_ds *buf;
  ushort *array;
}seminfo;

struct timeval startTime;

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

//mutex sheep in valley
struct sembuf WaitMutexSheepInValley = {MTX_SHEEPINVALLEY, -1, 0};
struct sembuf SignalMutexSheepInValley = {MTX_SHEEPINVALLEY, 1, 0};

//semaphore sheep in valley
struct sembuf WaitSheepInValley={SEM_SHEEPINVALLEY, -1, 0};
struct sembuf SignalSheepInValley={SEM_SHEEPINVALLEY, 1, 0};

//function initialization
void initialize();
void releaseResource();
void semctlChecked(int semID, int semNum, int flag, union semun seminfo); 
void semopChecked(int semID, struct sembuf *operation, unsigned num);


void initialize() {
  semID = semget(IPC_PRIVATE, 5, 0666 | IPC_CREAT);

  //initialize values of semaphore
  seminfo.val = 0;
  semctlChecked(semID, MTX_DRAGONWAKEUP, SETVAL, seminfo);
  semctlChecked(semID, SEM_SHEEPINVALLEY, SETVAL, seminfo);
  //initialize values of mutex
  seminfo.val = 1;
  semctlChecked(semID, MTX_DRAGONEAT, SETVAL, seminfo);
  semctlChecked(semID, MTX_MEAL, SETVAL, seminfo);
  semctlChecked(semID, MTX_SHEEPINVALLEY, SETVAL, seminfo);

  //allocate shared memory
  if((SheepInValleyFlag=shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666)) < 0) {
    printf("CREATE SheepInValleyFlag error\n");
    exit(1);
  }
  else {
    printf("CREATE SheepInValleyFlag Success, %d\n", SheepInValleyFlag);
  }

  if((numMealsFlag = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666)) < 0) {
    printf("CREATE numMealsFlag error\n");
    exit(1);
  }else {
    printf("CREATE numMealsFlag Success, %d\n", numMealsFlag);
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
	pid_t localid;
	localid = getpid();
	printf("ProcessID of smaug:%d\n", localid);
	printf("Smaug is sleeping\n");
	semopChecked(semID, &WaitDragonWakeUp, 1);
	int time = 0;
	while(1) {
		printf("Smaug is awake\n");
		semopChecked(semID, &WaitMeal, 1);
		if(*numMeals>0) {
			*numMeals = *numMeals - 1;
			printf("smaug eats one meal, %d meals left\n", *numMeals);
		}
		if(*numMeals>0) {
			*numMeals = *numMeals - 1;
			printf("smaug eats another meal, %d meals left\n", *numMeals);
		}
		semopChecked(semID, &SignalMeal, 1);
		printf("Smaug finishes snacks and goes sleeping\n");
		semopChecked(semID, &WaitDragonWakeUp, 1);
	}
}
void graze(int time) {
  if( usleep(time) == -1){
    /* exit when usleep interrupted by kill signal */
    if(errno==EINTR)exit(4);
  }
}

void sheep(int time) {
  pid_t localpid = getpid();
  printf("sheep %d is grazing for %d usec\n", localpid, time);
  graze(time);
  printf("sheep is enchanted:%d\n", localpid);
  //the sheep is enchanted
  semopChecked(semID, &WaitMutexSheepInValley, 1);
  semopChecked(semID, &SignalSheepInValley, 1);
  *SheepInValley = *SheepInValley + 1;
  printf("Now %d sheep in the valley\n",*SheepInValley);
  if(*SheepInValley >= SHEEP_IN_MEAL) {
    int i;
    for(i=0;i<SHEEP_IN_MEAL;i++) {
      semopChecked(semID, &WaitSheepInValley, 1);
    }
    *SheepInValley = *SheepInValley - SHEEP_IN_MEAL;
    semopChecked(semID, &WaitMeal, 1);
    *numMeals = *numMeals + 1;
    printf("A new meal is added, now number of meals:%d\n", *numMeals);
    semopChecked(semID, &SignalMeal, 1);
    semopChecked(semID, &SignalDragonWakeUp, 1);
  }
  semopChecked(semID, &SignalMutexSheepInValley, 1);


}
int main(void) {
	initialize();
	pid_t result = fork();
        srand(time(NULL));

	if(result<0) {
		printf("fork error\n");
		exit(1);
	}
	if(result==0) {
		smaug();
	}
	else {	
		pid_t r;
		while(1) {
			r = fork();
			if(r==0) break;
			else {
				int pr = random(); 
				usleep(pr%5555+1e6);
			}
	
		}
		int rn = random();
		sheep(1e6+rn%5555);
	  }
}
