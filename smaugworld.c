#include "smaugworld.h"
#include "sheep.h"
#include "smaug.h"
#include "cow.h"
//global variables
int semID;
union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
} seminfo;
struct timeval startTime;

//pointers to shared memory
// shared memory of dragon
int DragonWakeUpFlag = 0;
int *DragonWakeUp = NULL;
int numDragonJewelFlag = 0;
int *numDragonJewel = NULL;

//shared memory meals
int numMealFlag = 0;
int *numMeal = NULL;
int numEatenMealFlag = 0;
int *numEatenMeal = NULL;

//Shared Variables of Sheep
int SheepInValleyFlag = 0;
int *SheepInValley = NULL;
int numSheepToEatFlag = 0;
int *numSheepToEat = NULL;
int numSheepEatenFlag = 0;
int *numSheepEaten = NULL;

//Shared Variables of Cow
int CowInValleyFlag = 0;
int *CowInValley = NULL;
int numCowToEatFlag = 0;
int *numCowToEat = NULL;
int numCowEatenFlag = 0;
int *numCowEaten = NULL;

//shared variables of Hunters
int numHunterPathFlag = 0;
int *numHunterPath = NULL;
int numHunterLeaveFlag = 0;
int numHunterLeave = NULL;

//semaphores of dragon
//Dragon Wake up
struct sembuf WaitSDragonWakeUp = {SEM_S_DRAGONWAKEUP, -1, 0};
struct sembuf SignalSDragonWakeUp = {SEM_S_DRAGONWAKEUP, 1, 0};
//Dragon Eat
struct sembuf WaitSDragonEat = {SEM_S_DRAGONEAT, -1, 0};
struct sembuf SignalSDragonEat = {SEM_S_DRAGONEAT, 1, 0};
//protecting the shared memory of dragon wakeup
struct sembuf WaitPDragonWakeUp = {SEM_P_DRAGONWAKEUP, -1, 0};
struct sembuf SignalPDragonWakeUp = {SEM_P_DRAGONWAKEUP, 1, 0};
//protecting number of jewels dragon has
struct sembuf WaitPDragonJewel = {SEM_P_DRAGONJEWEL, -1, 0};
struct sembuf SignalPDragonJewel = {SEM_P_DRAGONJEWEL, 1, 0};
//Dragon Fight
struct sembuf WaitSDragonFight = {SEM_S_DRAGONFIGHT, -1, 0};
struct sembuf SignalSDragonFight = {SEM_S_DRAGONFIGHT, 1, 0};
//Dragon Play
struct sembuf WaitSDragonPlay = {SEM_S_DRAGONPLAY, -1, 0};
struct sembuf SignalSDragonPlay = {SEM_S_DRAGONPLAY, 1, 0};

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
// protecting sheep eaten
struct sembuf WaitPSheepEaten = {SEM_P_SHEEPEATEN, -1, 0};
struct sembuf SignalPSheepEaten = {SEM_P_SHEEPEATEN, 1, 0};
// sheep die
struct sembuf WaitSSheepDie = {SEM_S_SHEEPDIE, -1, 0};
struct sembuf SignalSSheepDie = {SEM_S_SHEEPDIE, 1, 0};

//semaphores of Cow
//Cow in Valley
struct sembuf WaitNCowInValley = {SEM_N_COWINVALLEY, -1, 0};
struct sembuf SignalNCowInValley = {SEM_N_COWINVALLEY, 1, 0};
//protector of Cow In Valley
struct sembuf WaitPCowInValley = {SEM_P_COWINVALLEY, -1, 0};
struct sembuf SignalPCowInValley = {SEM_P_COWINVALLEY, 1, 0};
//cow waiting
struct sembuf WaitSCowWaiting = {SEM_S_COWWAITING, -1, 0};
struct sembuf SignalSCowWaiting = {SEM_S_COWWAITING, 1, 0};
//cow to eat
struct sembuf WaitNCowToEat = {SEM_N_COWTOEAT, -1, 0};
struct sembuf SignalNCowToEat = {SEM_N_COWTOEAT, 1, 0};
//protector cow to eat
struct sembuf WaitPCowToEat = {SEM_P_COWTOEAT, -1, 0};
struct sembuf SignalPCowToEat = {SEM_P_COWTOEAT, 1, 0};
//cow eaten
struct sembuf WaitSCowEaten = {SEM_S_COWEATEN, -1, 0};
struct sembuf SignalSCowEaten = {SEM_S_COWEATEN, 1, 0};
//protector of Cow eaten
struct sembuf WaitPCowEaten = {SEM_P_COWEATEN, -1, 0};
struct sembuf SignalPCowEaten = {SEM_P_COWEATEN, 1, 0};
//cow die
struct sembuf WaitSCowDie = {SEM_S_COWDIE, -1, 0};
struct sembuf SignalSCowDie = {SEM_S_COWDIE, 1, 0};

//semaphores of hunters
//hunters in the path
struct sembuf WaitNHunterPath = {SEM_N_HUNTERPATH, -1, 0};
struct sembuf SignalNHunterPath = {SEM_N_HUNTERPATH, 1, 0};
//protecting the memory of hunters in path
struct sembuf WaitPHunterPath = {SEM_P_HUNTERPATH, -1, 0};
struct sembuf SignalPHunterPath = {SEM_P_HUNTERPATH, 1, 0};
//hunters entering the cave
struct sembuf WaitSHunterCave = {SEM_S_HUNTERCAVE, -1, 0};
struct sembuf SignalSHunterCave = {SEM_S_HUNTERCAVE, 1, 0};
// hunters fighting
struct sembuf WaitSHunterFight = {SEM_S_HUNTERFIGHT, -1, 0};
struct sembuf SignalSHunterFight = {SEM_S_HUNTERFIGHT, 1, 0};
//hunter leaving the Cave
struct sembuf WaitingSHunterLeave = {SEM_S_HUNTERLEAVE, -1, 0};
struct sembuf SignalSHunterLeave = {SEM_S_HUNTERLEAVE, 1, 0};
//counter of hunters dealt by smaug
struct sembuf WaitPHunterLeave = {SEM_P_HUNTERLEAVE, -1, 0};
struct sembuf SignalPHunterLeave = {SEM_P_HUNTERLEAVE, 1, 0};

//semaphore operations of thieves
struct sembuf WaitNThiefPath = {SEM_N_THIEFPATH, -1, 0};
struct sembuf SignalNThiefPath = {SEM_N_THIEFPATH, 1, 0};

struct sembuf WaitPThiefPath = {SEM_P_THIEFPATH, -1, 0};
struct sembuf SignalPThiefPath = {SEM_P_THIEFPATH, 1, 0};

struct sembuf WaitSThiefCave = {SEM_S_THIEFCAVE, -1, 0};
struct sembuf SignalSThiefCave = {SEM_S_THIEFCAVE, 1, 0};

struct sembuf WaitSThiefPlay = {SEM_S_THIEFPLAY, -1, 0};
struct sembuf SignalSThiefPlay = {SEM_S_THIEFPLAY, 1, 0};

struct sembuf WaitSThiefLeave = {SEM_S_THIEFLEAVE, -1, 0};
struct sembuf SignalSThiefLeave = {SEM_S_THIEFLEAVE, 1, 0};

struct sembuf WaitPThiefLeave = {SEM_P_THIEFLEAVE, -1, 0};
struct sembuf SignalPThiefLeave = {SEM_P_THIEFLEAVE, 1, 0};
//function definitions
void initialize() {
    semID = semget(IPC_PRIVATE, 23, 0666 | IPC_CREAT);

    //initialize values of semaphore
    seminfo.val = 0;
    semctlChecked(semID, SEM_S_DRAGONWAKEUP, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_DRAGONEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_MEAL, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_SHEEPINVALLEY, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_SHEEPWAITING, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_SHEEPTOEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_SHEEPEATEN, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_SHEEPDIE, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_COWINVALLEY, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_COWWAITING, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_COWTOEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_COWEATEN, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_COWDIE, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_DRAGONFIGHT, SETVAL, seminfo);

    //initialize values of mutex
    seminfo.val = 1;
    semctlChecked(semID, SEM_P_NUMMEAL, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_EATENMEAL, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_SHEEPINVALLEY, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_SHEEPTOEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_SHEEPEATEN, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_COWINVALLEY, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_COWTOEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_COWEATEN, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_DRAGONWAKEUP, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_DRAGONJEWEL, SETVAL, seminfo);
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
    //shared memory for dragon
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &DragonWakeUpFlag, &DragonWakeUp);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numDragonJewelFlag, &numDragonJewel);
    //shared memory for meal
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numMealFlag, &numMeal);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numEatenMealFlag, &numEatenMeal);
    //shared memory for sheep
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &SheepInValleyFlag, &SheepInValley);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numSheepToEatFlag, &numSheepToEat);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numSheepEatenFlag, &numSheepEaten);
    //shared memory for cow
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &CowInValleyFlag, &CowInValley);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numCowToEatFlag, &numCowToEat);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numCowEatenFlag, &numCowEaten);
}

void semctlChecked(int semID, int semNum, int flag, union semun seminfo) {
    if (semctl(semID, semNum, flag, seminfo) == -1) {
        if (errno != EIDRM) {
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
    if (semop(semID, operation, num) == -1) {
        if (errno != EIDRM) {
            printf("semaphore operation failed: simulation terminating\n");
            printf("errno: %8d\n", errno);
            exit(2);
        }
        else {
            exit(3);
        }
    }
}

// function for allocating shared memory and attach it to the address provided
void shmAllocate(key_t key, size_t size, int shmflg1, const void *shmaddr, int shmflg2, int *flag, int **addr) {
    //allocate shared memory
    if ((*flag = shmget(key, size, shmflg1)) < 0) {
        printf("CREATE shared memory error\n");
        exit(1);
    } else {
        printf("CREATE shared memory success, %d\n", *flag);
    }

    //attach shared memory
    if ((*addr = shmat(*flag, shmaddr, shmflg2)) == (int *) -1) {
        printf("Attach Shared Memory Error\n");
        exit(1);
    } else {
        printf("Attach Shared Memory Success\n");
    }
}

int main(void) {
    initialize();
    pid_t result = fork();
    srand(time(NULL));

    if (result < 0) {
        printf("fork error\n");
        exit(1);
    }
    if (result == 0) {
        smaug();
    }
    else {
        pid_t r;
        while (1) {
            r = fork();
            if (r == 0) break;
            else {
                int pr = random();
                usleep(pr % 5555 + 5e5);
            }

        }
        int rn = random();
        if(rn%2==0) sheep(1e6 + rn % 5555);
        else cow(1e6 + rn%6666);
    }
}
