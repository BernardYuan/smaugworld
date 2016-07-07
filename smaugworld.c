#include "smaugworld.h"
#include "sheep.h"
#include "smaug.h"
//global variables
int semID;
union semun{
  int val;
  struct semid_ds *buf;
  ushort *array;
} seminfo;
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

//function definitions
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
