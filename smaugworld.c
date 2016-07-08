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
int  numMealFlag = 0;
int *numMeal = NULL;
int  numEatenMealFlag = 0;
int *numEatenMeal = NULL;
int  SheepInValleyFlag = 0;
int *SheepInValley = NULL;
int  numSheepToEatFlag = 0;
int *numSheepToEat = NULL;
int  numSheepEatenFlag = 0;
int *numSheepEaten = NULL;

//semaphores of dragon
//Dragon Wake up
struct sembuf WaitSDragonWakeUp = {SEM_S_DRAGONWAKEUP, -1, 0};
struct sembuf SignalSDragonWakeUp = {SEM_S_DRAGONWAKEUP, 1, 0};
//Dragon Eat
struct sembuf WaitSDragonEat = {SEM_S_DRAGONEAT, -1, 0};
struct sembuf SignalSDragonEat = {SEM_S_DRAGONEAT, 1, 0};

//semaphores of meal
//number of meals
struct sembuf WaitNMeal = {SEM_N_MEAL, -1, 0};
struct sembuf SignalNMeal = {SEM_N_MEAL, 1, 0};
//protecting number of meal shared memory
struct sembuf WaitPNumMeal = {SEM_P_NUMMEAL, -1, 0};
struct sembuf SignalPNumMeal = {SEM_P_NUMMEAL, 1, 0};
//protecting shared memory variable number of eaten meals
struct sembuf WaitPEatenMeal = {SEM_P_EATENMEAL, -1, 0};
struct sembuf SignalPEatenMeal = {SEM_P_EATENMEAL, 1, 0};

//semaphores of sheep
//number of sheep in Valley
struct sembuf WaitNSheepInValley = {SEM_N_SHEEPINVALLEY, -1, 0};
struct sembuf SignalNSheepInValley = {SEM_N_SHEEPINVALLEY, 1, 0};
//protecting the shared variable of number of sheep in Valley
struct sembuf WaitPSheepInValley = {SEM_P_SHEEPINVALLEY, -1, 0};
struct sembuf SignalPSheepInValley = {SEM_P_SHEEPINVALLEY, 1, 0};
//sheep waiting to be eaten
struct sembuf WaitSSheepWaiting = {SEM_S_SHEEPWAITING, -1, 0};
struct sembuf SignalSSheepWaiting = {SEM_S_SHEEPWAITING, 1, 0};
// semaphore of sheep to eat
struct sembuf WaitNSheepToEat = {SEM_N_SHEEPTOEAT, -1, 0};
struct sembuf SignalNSheepToEat = {SEM_N_SHEEPTOEAT, 1, 0};
// protecting the shared variable sheep to eat
struct sembuf WaitPSheepToEat = {SEM_P_SHEEPTOEAT, -1, 0};
struct sembuf SignalPSheepToEat = {SEM_P_SHEEPTOEAT, 1, 0};
// sheep eaten
struct sembuf WaitSSheepEaten = {SEM_S_SHEEPEATEN, -1, 0};
struct sembuf SignalSSheepEaten = {SEM_S_SHEEPEATEN, 1, 0};
// sheep die
struct sembuf WaitSSheepDie = {SEM_S_SHEEPDIE, -1, 0};
struct sembuf SignalSSheepDie = {SEM_S_SHEEPDIE, 1, 0};

//function definitions
void initialize() {
	semID = semget(IPC_PRIVATE, 13, 0666 | IPC_CREAT);

	//initialize values of semaphore
	seminfo.val = 0;
	semctlChecked(semID, SEM_S_DRAGONWAKEUP, SETVAL, seminfo);
	semctlChecked(semID, SEM_S_DRAGONEAT, SETVAL, seminfo);
	semctlChecked(semID, SEM_N_MEAL, SETVAL, seminfo);
	semctlChecked(semID, SEM_N_SHEEPINVALLEY, SETVAL, seminfo);
	semctlChecked(semID, SEM_N_SHEEPWAITING, SETVAL, seminfo);
	semctlChecked(semID, SEM_N_SHEEPTOEAT, SETVAL, seminfo);
	semctlChecked(semID, SEM_S_SHEEPEATEN, SETVAL, seminfo);
	semctlChecked(semID, SEM_S_SHEEPDIE, SETVAL, seminfo);

	//initialize values of mutex
	seminfo.val = 1;
	semctlChecked(semID, SEM_P_NUMMEAL, SETVAL, seminfo);
	semctlChecked(semID, SEM_P_EATENMEAL, SETVAL, seminfo);
	semctlChecked(semID, SEM_P_SHEEPINVALLEY, SETVAL, seminfo);
	semctlChecked(semID, SEM_P_SHEEPTOEAT, SETVAL, seminfo);
	semctlChecked(semID, SEM_P_SHEEPEATEN, SETVAL, seminfo);
	
//int  numMealFlag = 0;
//int *numMeal = NULL;
//int  numEatenMealFlag = 0;
//int *numEatenMeal = NULL;
//int  SheepInValleyFlag = 0;
//int *SheepInValley = NULL;
//int  numSheepToEatFlag = 0;
//int *numSheepToEat = NULL;
//int  numSheepEatenFlag = 0;
//int *numSheepEaten = NULL;

	//allocate shared memory
	shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666, NULL, 0, &numMealFlag, &numMeal);
	shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666, NULL, 0, &numEatenMealFlag, &numEatenMeal);
	shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666, NULL, 0, &SheepInValleyFlag, &SheepInValley);
	shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666, NULL, 0, &numSheepToEatFlag, &numSheepToEat);
	shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666, NULL, 0, &numSheepEatenFlag, &numSheepEaten);
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
void shmAllocate(key_t key, size_t size, int shmflg1, const void *shmaddr, int shmflg2, int *flag, int **addr) {
	//allocate shared memory
	if((*flag=shmget(key, size, shmflg1)) < 0) {
		printf("CREATE shared memory error\n");
		exit(1);
	}else {
		printf("CREATE shared memory success, %d\n", *flag);
	}

	//attach shared memory
	if((*addr = shmat(*flag, shmaddr, shmflg2))==(int *) -1) {
		printf("Attach Shared Memory Error\n");
		exit(1);
	}else {
		printf("Attach Shared Memory Success\n");
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
