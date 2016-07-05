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
#define MTX_DRAGONWAKEUP 0 //wake up the dragon
#define MTX_DRAGONEAT 1    //the dragon eats
#define MTX_MEAL 2         //num of meals in the valley
#define MTX_SHEEPEATEN 3   //number of eaten sheep
#define MTX_MEALEATEN 4    //number of eaten meal
#define MTX_SHEEPINVALLEY 5//number of sheep in valley
#define SEM_SHEEPINVALLEY 6//semaphore of sheep in valley
#define SEM_SHEEPEATEN 7   //semaphore of eaten sheep

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
int *numMealsEaten = NULL;
int  numMealsEatenFlag = 0;
int *numSheepEaten = NULL;
int  numSheepEatenFlag = 0;

// semaphore buffers 
// dragon sleep:
struct sembuf WaitMutexDragonWakeUp={MTX_DRAGONWAKEUP, -1, 0};
struct sembuf SignalMutexDragonWakeUp={MTX_DRAGONWAKEUP, 1, 0};

//dragon eat:
struct sembuf WaitMutexDragonEat = {MTX_DRAGONEAT, -1, 0};
struct sembuf SignalMutexDragonEat = {MTX_DRAGONEAT, 1, 0};

//mutex on meal
struct sembuf WaitMutexMeal = {MTX_MEAL, -1, 0};
struct sembuf SignalMutexMeal = {MTX_MEAL, 1, 0};

//mutex on sheep eaten
struct sembuf WaitMutexSheepEaten = {MTX_SHEEPEATEN, -1, 0};
struct sembuf SignalMutexSheepEaten = {MTX_SHEEPEATEN, 1, 0};

//mutex on meal eaten
struct sembuf WaitMutexMealEaten = {MTX_MEALEATEN, -1, 0};
struct sembuf SignalMutexMealEaten = {MTX_MEALEATEN, 1, 0};

//mutex sheep in valley
struct sembuf WaitMutexSheepInValley = {MTX_SHEEPINVALLEY, -1, 0};
struct sembuf SignalMutexSheepInValley = {MTX_SHEEPINVALLEY, 1, 0};

//semaphore sheep in valley
struct sembuf WaitSheepInValley={SEM_SHEEPINVALLEY, -1, 0};
struct sembuf SignalSheepInValley={SEM_SHEEPINVALLEY, 1, 0};

//semaphore sheep eaten by dragon
struct sembuf WaitSheepEaten = {SEM_SHEEPEATEN, -1, 0};
struct sembuf SignalSheepEaten = {SEM_SHEEPEATEN, 1, 0};

//function initialization
void initialize();
void releaseResource();
void semctlChecked(int semID, int semNum, int flag, union semun seminfo); 
void semopChecked(int semID, struct sembuf *operation, unsigned num);


void initialize() {
	semID = semget(IPC_PRIVATE, 8, 0666 | IPC_CREAT);

	//initialize values of semaphore
	seminfo.val = 0;
	semctlChecked(semID, MTX_DRAGONWAKEUP, SETVAL, seminfo);
	semctlChecked(semID, SEM_SHEEPINVALLEY, SETVAL, seminfo);
	semctlChecked(semID, SEM_SHEEPEATEN, SETVAL, seminfo);

	//initialize values of mutex
	seminfo.val = 1;
	semctlChecked(semID, MTX_DRAGONEAT, SETVAL, seminfo);
	semctlChecked(semID, MTX_MEAL, SETVAL, seminfo);
	semctlChecked(semID, MTX_MEALEATEN, SETVAL, seminfo);
	semctlChecked(semID, MTX_SHEEPEATEN, SETVAL, seminfo);
	semctlChecked(semID, MTX_SHEEPINVALLEY, SETVAL, seminfo);

	//allocate shared memory
	if((SheepInValleyFlag=shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666)) < 0) {
		printf("CREATE SheepInValleyFlag error\n");
		exit(1);
	}else {
		printf("CREATE SheepInValleyFlag Success, %d\n", SheepInValleyFlag);
	}
	if((numMealsFlag = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666)) < 0) {
		printf("CREATE numMealsFlag error\n");
		exit(1);
	}else {
		printf("CREATE numMealsFlag Success, %d\n", numMealsFlag);
	}
	if((numMealsEatenFlag = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666)) < 0) {
		printf("CREATE numMealsEatenFlag error\n");
		exit(1);
	}else {
		printf("CREATE numMealsEatenFlag Success, %d\n", numMealsEatenFlag);
	}
	if((numSheepEatenFlag = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666)) < 0) {
		printf("CREATE numSheepEatenFlag error\n");
		exit(1);
	}else {
		printf("CREATE numSheepEatenFlag Success, %d\n", numSheepEatenFlag);
	}

	//attach shared memory
	if((SheepInValley = shmat(SheepInValleyFlag, NULL, 0))==(int *) -1) {
		printf("Attach Shared Memory SheepInValley Error\n");
		exit(1);
	}else {
		printf("Attach Shared Memory SheepInValley Success\n");
	}
	if((numMeals = shmat(numMealsFlag, NULL, 0)) == (int *) -1) {
		printf("Attach Shared Memory numMeals Error\n");
		exit(1);
	}else {
		printf("Attach Shared Memory numMeals Success\n");
	}
	if((numMealsEaten = shmat(numMealsEatenFlag, NULL, 0)) == (int *) -1) {
		printf("Attach Shared Memory numMealsEaten Error\n");
		exit(1);
	}else {
		printf("Attach Shared Memory numMealsEaten Success\n");
	}
	if((numSheepEaten = shmat(numSheepEatenFlag, NULL, 0)) == (int *) -1) {
		printf("Attach Shared Memory numSheepEaten Error\n");
		exit(1);
	}else {
		printf("Attach Shared Memory numSheepEaten Success\n");
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
	semopChecked(semID, &WaitMutexDragonWakeUp, 1);
	int time = 0;
	while(1) {
		printf("Smaug is awake\n");
		semopChecked(semID, &WaitMutexMeal, 1);
		if(*numMeals>0) {
			*numMeals = *numMeals - 1;
			printf("smaug eats one meal, %d meals left\n", *numMeals);
			semopChecked(semID, &WaitMutexDragonEat, 1);

			semopChecked(semID, &WaitMutexMealEaten, 1);	
			*numMealsEaten = *numMealsEaten + 1;
			int i=0;
			for (i = 0 ; i < SHEEP_IN_MEAL ; i++) {
				semopChecked(semID, &SignalSheepEaten, 1);
			}
			semopChecked(semID, &SignalMutexMealEaten, 1);
		}
		semopChecked(semID, &SignalMutexMeal, 1);


		semopChecked(semID, &WaitMutexMeal, 1);
		if(*numMeals>0) {
			*numMeals = *numMeals - 1;
			printf("smaug eats another meal, %d meals left\n", *numMeals);
		}
		semopChecked(semID, &SignalMutexMeal, 1);
		printf("Smaug finishes snacks and goes sleeping\n");
		
		semopChecked(semID, &WaitMutexDragonWakeUp, 1);
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
		semopChecked(semID, &WaitMutexMeal, 1);
		*numMeals = *numMeals + 1;
		printf("A new meal is added, now number of meals:%d\n", *numMeals);
		semopChecked(semID, &SignalMutexMeal, 1);
		semopChecked(semID, &SignalMutexDragonWakeUp, 1);
	}
	semopChecked(semID, &SignalMutexSheepInValley, 1);
	
	//sheep goes into the queue waiting to be eaten
	semopChecked(semID, &WaitSheepEaten, 1);
	semopChecked(semID, &WaitMutexSheepEaten, 1);
	*numSheepEaten = *numSheepEaten + 1;
	semopChecked(semID, &SignalMutexSheepEaten, 1);
	printf("Sheep %d is eaten\n", localpid);
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
				int pr = random(); usleep(pr%5555+1e6);
			}
	
		}
		int rn = random();
		sheep(1e6+rn%5555);
	  }
}
