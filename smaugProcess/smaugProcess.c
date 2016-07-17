#include "smaugProcess.h"

//global variables
int semID;
union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
} seminfo;
struct timeval startTime;
//process group IDs
pid_t smaugID;
const pid_t dragonGID = 660;
const pid_t beastGID = 661;
const pid_t thiefGID = 662;
const pid_t hunterGID = 663;

//pointers to shared memory
// shared memory of dragon
//int DragonWakeUpFlag = 0;
//int *DragonWakeUp = NULL;
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
int numMealSheepFlag = 0;
int *numMealSheep = NULL;

//Shared Variables of Cow
int CowInValleyFlag = 0;
int *CowInValley = NULL;
int numCowToEatFlag = 0;
int *numCowToEat = NULL;
int numCowEatenFlag = 0;
int *numCowEaten = NULL;
int numMealCowFlag = 0;
int *numMealCow = NULL;

//shared variables of Hunters
int numHunterPathFlag = 0;
int *numHunterPath = NULL;
int numHunterLeaveFlag = 0;
int *numHunterLeave = NULL;

//shared variable of thieves
int numThiefPathFlag = 0;
int *numThiefPath = NULL;
int numThiefLeaveFlag = 0;
int *numThiefLeave = NULL;

//shared variable of the system
int flagTerminationFlag = 0;
int *flagTermination = NULL;

//semaphores of dragon
//Dragon Wake up
struct sembuf WaitSDragonWakeUp = {SEM_S_DRAGONWAKEUP, -1, 0};
struct sembuf SignalSDragonWakeUp = {SEM_S_DRAGONWAKEUP, 1, 0};
//Dragon Eat
struct sembuf WaitSDragonEat = {SEM_S_DRAGONEAT, -1, 0};
struct sembuf SignalSDragonEat = {SEM_S_DRAGONEAT, 1, 0};
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
//State semaphore of meal done
struct sembuf WaitSMealDone = {SEM_S_MEALDONE, -1, 0};
struct sembuf SignalSMealDone = {SEM_S_MEALDONE, 1, 0};

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
//protecting number of eaten sheep in a meal
struct sembuf WaitPMealSheep = {SEM_P_MEALSHEEP, -1, 0};
struct sembuf SignalPMealSheep = {SEM_P_MEALSHEEP, 1, 0};
//semaphore of eaten sheep in the meal
struct sembuf WaitNMealSheep = {SEM_N_MEALSHEEP, -1, 0};
struct sembuf SignalNMealSheep = {SEM_N_MEALSHEEP, 1, 0};

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
// protecting the number of cows in the meal
struct sembuf WaitPMealCow = {SEM_P_MEALCOW, -1, 0};
struct sembuf SignalPMealCow = {SEM_P_MEALCOW, 1, 0};
// number of cows in the meal
struct sembuf WaitNMealCow = {SEM_N_MEALCOW, -1, 0};
struct sembuf SignalNMealCow = {SEM_N_MEALCOW, 1, 0};

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
//struct sembuf WaitSHunterLeave = {SEM_S_HUNTERLEAVE, -1, 0};
//struct sembuf SignalSHunterLeave = {SEM_S_HUNTERLEAVE, 1, 0};
//counter of hunters dealt by smaug
struct sembuf WaitPHunterLeave = {SEM_P_HUNTERLEAVE, -1, 0};
struct sembuf SignalPHunterLeave = {SEM_P_HUNTERLEAVE, 1, 0};

//semaphore operations of thieves
//counter of thieves in the path
struct sembuf WaitNThiefPath = {SEM_N_THIEFPATH, -1, 0};
struct sembuf SignalNThiefPath = {SEM_N_THIEFPATH, 1, 0};
//protector of number of thieves in the path
struct sembuf WaitPThiefPath = {SEM_P_THIEFPATH, -1, 0};
struct sembuf SignalPThiefPath = {SEM_P_THIEFPATH, 1, 0};
//Thief enters the cave
struct sembuf WaitSThiefCave = {SEM_S_THIEFCAVE, -1, 0};
struct sembuf SignalSThiefCave = {SEM_S_THIEFCAVE, 1, 0};
//Thief plays with smaug
struct sembuf WaitSThiefPlay = {SEM_S_THIEFPLAY, -1, 0};
struct sembuf SignalSThiefPlay = {SEM_S_THIEFPLAY, 1, 0};
//Thief leaves
//struct sembuf WaitSThiefLeave = {SEM_S_THIEFLEAVE, -1, 0};
//struct sembuf SignalSThiefLeave = {SEM_S_THIEFLEAVE, 1, 0};
//protector of number of left thief
struct sembuf WaitPThiefLeave = {SEM_P_THIEFLEAVE, -1, 0};
struct sembuf SignalPThiefLeave = {SEM_P_THIEFLEAVE, 1, 0};

//semaphores of system
struct sembuf WaitPTermination = {SEM_P_TERMINATION, -1, 0};
struct sembuf SignalPTermination = {SEM_P_TERMINATION, 1, 0};

//===========================================================
// Utilities
//===========================================================
//function definitions
void initialize() {
    semID = semget(IPC_PRIVATE, 39, 0666 | IPC_CREAT);

    //initialize values of semaphore
    //semaphores of Dragon
    seminfo.val = 0;
    semctlChecked(semID, SEM_S_DRAGONWAKEUP, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_DRAGONEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_DRAGONFIGHT, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_DRAGONPLAY, SETVAL, seminfo);
    seminfo.val = 1;
//    semctlChecked(semID, SEM_P_DRAGONWAKEUP, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_DRAGONJEWEL, SETVAL, seminfo);

    //semaphores of meals
    seminfo.val = 0;
    semctlChecked(semID, SEM_N_MEAL, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_MEALDONE, SETVAL, seminfo);
    seminfo.val = 1;
    semctlChecked(semID, SEM_P_NUMMEAL, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_EATENMEAL, SETVAL, seminfo);

    //semaphores of sheep
    seminfo.val = 0;
    semctlChecked(semID, SEM_N_SHEEPINVALLEY, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_SHEEPWAITING, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_SHEEPTOEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_SHEEPEATEN, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_MEALSHEEP, SETVAL, seminfo);
    seminfo.val = 1;
    semctlChecked(semID, SEM_P_SHEEPINVALLEY, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_SHEEPTOEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_SHEEPEATEN, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_MEALSHEEP, SETVAL, seminfo);

    //semaphores of cows
    seminfo.val = 0;
    semctlChecked(semID, SEM_N_COWINVALLEY, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_COWWAITING, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_COWTOEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_COWEATEN, SETVAL, seminfo);
    semctlChecked(semID, SEM_N_MEALCOW, SETVAL, seminfo);
    seminfo.val = 1;
    semctlChecked(semID, SEM_P_COWINVALLEY, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_COWTOEAT, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_COWEATEN, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_MEALCOW, SETVAL, seminfo);

    //semaphores of hunters
    seminfo.val = 0;
    semctlChecked(semID, SEM_N_HUNTERPATH, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_HUNTERCAVE, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_HUNTERFIGHT, SETVAL, seminfo);
    seminfo.val = 1;
    semctlChecked(semID, SEM_P_HUNTERPATH, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_HUNTERLEAVE, SETVAL, seminfo);

    //semaphores of thieves
    seminfo.val = 0;
    semctlChecked(semID, SEM_N_THIEFPATH, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_THIEFCAVE, SETVAL, seminfo);
    semctlChecked(semID, SEM_S_THIEFPLAY, SETVAL, seminfo);
    seminfo.val = 1;
    semctlChecked(semID, SEM_P_THIEFPATH, SETVAL, seminfo);
    semctlChecked(semID, SEM_P_THIEFLEAVE, SETVAL, seminfo);

    //semaphores of the system
    seminfo.val = 1;
    semctlChecked(semID, SEM_P_TERMINATION, SETVAL, seminfo);

    //allocate shared memory
    //shared memory for dragon
//    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &DragonWakeUpFlag, &DragonWakeUp);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numDragonJewelFlag, &numDragonJewel);
    //shared memory for meal
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numMealFlag, &numMeal);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numEatenMealFlag, &numEatenMeal);
    //shared memory for sheep
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &SheepInValleyFlag, &SheepInValley);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numSheepToEatFlag, &numSheepToEat);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numSheepEatenFlag, &numSheepEaten);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numMealSheepFlag, &numMealSheep);
    //shared memory for cow
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &CowInValleyFlag, &CowInValley);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numCowToEatFlag, &numCowToEat);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numCowEatenFlag, &numCowEaten);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numMealCowFlag, &numMealCow);
    //shared memory for hunter
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numHunterPathFlag, &numHunterPath);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numHunterLeaveFlag, &numHunterLeave);
    //shared memory for thief
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numThiefPathFlag, &numThiefPath);
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numThiefLeaveFlag, &numThiefLeave);
    //shared memory for the system
    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &flagTerminationFlag, &flagTermination);
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

// deallocate memory
void shmDeallocate(int flg, int *ptr) {
    if (shmdt(ptr) == -1) {
        printf("Detach shared memory failed\n");
    }
    else {
        printf("Detach shared memory success\n");
    }

    if (shmctl(flg, IPC_RMID, NULL) == -1) {
        printf("Release shared memory failed\n");
    }
    else {
        printf("Release shared memory success\n");
    }
}

void terminateSimulation() {

    pid_t localpid = getpid();
    pid_t localgid = getpgid(localpid);
    printf("In process %d Terminate Simulation is executed now\n", localpid);
    if (localgid != dragonGID) {
        if (killpg(dragonGID, SIGKILL) == -1 && errno == EPERM) {
            printf("Dragon not killed\n");
        }
        else printf("Dragon killed\n");
    }

    if (localgid != beastGID) {
        if (killpg(beastGID, SIGKILL) == -1 && errno == EPERM) {
            printf("Beasts not killed\n");
        }
        else printf("Beasts killed\n");
    }

    if (localgid != thiefGID) {
        if (killpg(thiefGID, SIGKILL) == -1 && errno == EPERM) {
            printf("Thieves not killed\n");
        }
        else printf("Thieves killed\n");
    }

    if (localgid != hunterGID) {
        if (killpg(hunterGID, SIGKILL) == -1 && errno == EPERM) {
            printf("Hunters not killed\n");
        }
        else printf("Hunters killed\n");
    }
}

//release resource
void releaseResource() {
    printf("Releasing Resources\n");

    if (semctl(semID, 0, IPC_RMID) == -1) {
        printf("Release Semaphores Failed\n");
    }
    else {
        printf("Release Semaphores Success\n");
    }

    //shared memory for dragon
    shmDeallocate(numDragonJewelFlag, numDragonJewel);
    //shared memory for meal
    shmDeallocate(numMealFlag, numMeal);
    shmDeallocate(numEatenMealFlag, numEatenMeal);
    //shared memory for sheep
    shmDeallocate(SheepInValleyFlag, SheepInValley);
    shmDeallocate(numSheepToEatFlag, numSheepToEat);
    shmDeallocate(numSheepEatenFlag, numSheepEaten);
    shmDeallocate(numMealSheepFlag, numMealSheep);
    //shared memory for cow
    shmDeallocate(CowInValleyFlag, CowInValley);
    shmDeallocate(numCowToEatFlag, numCowToEat);
    shmDeallocate(numCowEatenFlag, numCowEaten);
    shmDeallocate(numMealCowFlag, numMealCow);
    //shared memory for hunter
    shmDeallocate(numHunterPathFlag, numHunterPath);
    shmDeallocate(numHunterLeaveFlag, numHunterLeave);
    //shared memory for thief
    shmDeallocate(numThiefPathFlag, numThiefPath);
    shmDeallocate(numThiefLeaveFlag, numThiefLeave);
    //shared memory for the system
    shmDeallocate(flagTerminationFlag, flagTermination);
}

//the function which sets the termination
void setTerminate() {
    semopChecked(semID, &WaitPTermination, 1);
    *flagTermination = 1;
    semopChecked(semID, &SignalPTermination, 1);
}

int checkSheep() {
    semopChecked(semID, &WaitPSheepEaten, 1);
    if (*numSheepEaten >= MAX_SHEEP) {
        printf("Smaug has eaten more than maximum number of sheep, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPSheepEaten, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPSheepEaten, 1);
        return 0;
    }

}

int checkCow() {
    semopChecked(semID, &WaitPCowEaten, 1);
    if (*numCowEaten >= MAX_COW) {
        printf("Smaug has eaten more than the maximum number of cows, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPCowEaten, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPCowEaten, 1);
        return 0;
    }
}

int checkThief() {
    semopChecked(semID, &WaitPThiefLeave, 1);
    if (*numThiefLeave >= MAX_THIEF) {
        printf("Smaug has played with more than maximum number of thieves, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPThiefLeave, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPThiefLeave, 1);
        return 0;
    }
}

int checkHunter() {
    semopChecked(semID, &WaitPHunterLeave, 1);
    if (*numHunterLeave >= MAX_TREASUREHUNTER) {
        printf("Smaug has fought with more than maximum number of hunters, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPHunterLeave, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPHunterLeave, 1);
        return 0;
    }
}

int checkJewel() {
    semopChecked(semID, &WaitPDragonJewel, 1);
    if (*numDragonJewel >= MAX_JEWEL) {
        printf("Dragon has more than maximum number of jewels, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPDragonJewel, 1);
        return 1;
    }
    else if (*numDragonJewel <= MIN_JEWEL) {
        printf("Dragon has fewer than minimum number of jewels, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPDragonJewel, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPDragonJewel, 1);
        return 0;
    }
}

int checkTermination() {
    semopChecked(semID, &WaitPTermination, 1);
    if (*flagTermination == 1) {
        semopChecked(semID, &SignalPTermination, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPTermination, 1);
        return 0;
    }
}
//========================================================
//Cow
//========================================================
void cow(int time) {
    pid_t localpid = getpid();
    setpgid(localpid, beastGID);
    printf("COWCOWCOWCOWCOWCOW      Cow[%d] is grazing for %d usec\n", localpid, time);
    //grazing
    if (usleep(time) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR) exit(4);
    }
    //the cow is enchanted
    //keep in this order in all files to avoid deadlock
    semopChecked(semID, &WaitPSheepInValley, 1); //use the number of sheep in valley
    semopChecked(semID, &WaitPCowInValley, 1);   //use the number of cows in valley
    *CowInValley = *CowInValley + 1;
    semopChecked(semID, &SignalNCowInValley, 1);
    printf("COWCOWCOWCOWCOWCOW      Cow[%d] is enchanted, now %d cows and %d sheep in Valley\n", localpid, *CowInValley, *SheepInValley);
    // There is a meal in the Valley
    if (*CowInValley >= COW_IN_MEAL && *SheepInValley >= SHEEP_IN_MEAL) {
        int i;
        // decrement the counters only when the semaphores are decremented
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowInValley, 1);
            *CowInValley = *CowInValley - 1;
        }
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &WaitNSheepInValley, 1);
            *SheepInValley = *SheepInValley - 1;
        }

        //increment the number of meals
        semopChecked(semID, &WaitPNumMeal, 1);
        semopChecked(semID, &SignalNMeal, 1);
        *numMeal = *numMeal + 1;
        printf("COWCOWCOWCOWCOWCOW      Cow[%d] results in a new meal, now number of meals:%d\n", localpid, *numMeal);
        semopChecked(semID, &SignalPNumMeal, 1);
        //release the use of shared variables
        semopChecked(semID, &SignalPCowInValley, 1);
        semopChecked(semID, &SignalPSheepInValley, 1);

        //send a wakeup to dragon
        semopChecked(semID, &SignalSDragonWakeUp, 1);
    }
    else { //the cow in the Valley is not enough for a meal
        semopChecked(semID, &SignalPCowInValley, 1);
        semopChecked(semID, &SignalPSheepInValley, 1);
    }
//    printf("Cow %d waiting in the snack\n", localpid);
    semopChecked(semID, &WaitSCowWaiting, 1);

    semopChecked(semID, &SignalNCowToEat, 1);

    //Wait in this order to avoid deadlock
    semopChecked(semID, &WaitPSheepToEat, 1);
    semopChecked(semID, &WaitPCowToEat, 1);
    *numCowToEat = *numCowToEat + 1;
    if (*numCowToEat >= COW_IN_MEAL && *numSheepToEat >= SHEEP_IN_MEAL) {
        int i;
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowToEat, 1);
            *numCowToEat = *numCowToEat - 1;
        }
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &WaitNSheepToEat, 1);
            *numSheepToEat = *numSheepToEat - 1;
        }
        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalPSheepToEat, 1);
        semopChecked(semID, &SignalSDragonEat, 1);
    }
    else {
        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalPSheepToEat, 1);
    }

    // it waits to be eaten
    semopChecked(semID, &WaitSCowEaten, 1);

    semopChecked(semID, &WaitPCowEaten, 1);
    *numCowEaten = *numCowEaten + 1;
    semopChecked(semID, &SignalPCowEaten, 1);

    printf("COWCOWCOWCOWCOWCOW      Cow[%d] is eaten, now %d cows eaten\n", localpid, *numCowEaten);
    if(checkCow()) {
        terminateSimulation();
        exit(0);

    }
    semopChecked(semID, &SignalNMealCow, 1);

    //signal smaug that the meal is done only when all beasts are eaten
    //keep in this order to avoid deadlock
    semopChecked(semID, &WaitPMealSheep, 1);
    semopChecked(semID, &WaitPMealCow, 1);
    *numMealCow = *numMealCow + 1;
    if(*numMealSheep >= SHEEP_IN_MEAL && *numMealCow >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL ; i++) {
            semopChecked(semID, &WaitNMealSheep, 1);
            *numMealSheep = *numMealSheep - 1;
        }

        for (i = 0; i < COW_IN_MEAL ; i++) {
            semopChecked(semID, &WaitNMealCow, 1);
            *numMealCow = *numMealCow - 1;
        }

        semopChecked(semID, &SignalPMealCow, 1);
        semopChecked(semID, &SignalPMealSheep, 1);
        semopChecked(semID, &SignalSMealDone, 1);
    }
    else {
        semopChecked(semID, &SignalPMealCow, 1);
        semopChecked(semID, &SignalPMealSheep, 1);
    }
}

//===================================================================
// SHEEP
//===================================================================
void sheep(int time) {
    pid_t localpid = getpid();
    setpgid(localpid, beastGID);
    printf("SHEEPSHEEPSHEEPSHEEP    Sheep[%d] is grazing for %d usec\n", localpid, time);
    //grazing
    if (usleep(time) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }
    //the sheep is enchanted

    //keep in this order to get use
    semopChecked(semID, &WaitPSheepInValley, 1);
    semopChecked(semID, &WaitPCowInValley, 1);
    *SheepInValley = *SheepInValley + 1;
    semopChecked(semID, &SignalNSheepInValley, 1);
    printf("SHEEPSHEEPSHEEPSHEEP    Sheep[%d] is enchanted, now %d cows and %d sheep in Valley\n", localpid, *CowInValley, *SheepInValley);
    // There is a meal in the Valley
    if (*SheepInValley >= SHEEP_IN_MEAL && *CowInValley >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &WaitNSheepInValley, 1);
            *SheepInValley = *SheepInValley - 1;
        }
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowInValley, 1);
            *CowInValley = *CowInValley - 1;
        }

        semopChecked(semID, &WaitPNumMeal, 1);
        *numMeal = *numMeal + 1;
        printf("SHEEPSHEEPSHEEPSHEEP    Sheep[%d] results in a new meal, now number of meals:%d\n", localpid, *numMeal);
        semopChecked(semID, &SignalNMeal, 1);
        semopChecked(semID, &SignalPNumMeal, 1);

        semopChecked(semID, &SignalPCowInValley, 1);
        semopChecked(semID, &SignalPSheepInValley, 1);

        //send a wake up to the dragon
        semopChecked(semID, &SignalSDragonWakeUp, 1);
    }
    else { //the sheep in the Valley is not enough for a meal
        semopChecked(semID, &SignalPCowInValley, 1);
        semopChecked(semID, &SignalPSheepInValley, 1);
    }

    // sheep waiting
    semopChecked(semID, &WaitSSheepWaiting, 1);

    semopChecked(semID, &SignalNSheepToEat, 1);

    // smaug starts eating only when the the meal is ready
    semopChecked(semID, &WaitPSheepToEat, 1);
    semopChecked(semID, &WaitPCowToEat, 1);
    *numSheepToEat = *numSheepToEat + 1;
    if (*numSheepToEat >= SHEEP_IN_MEAL && *numCowToEat >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &WaitNSheepToEat, 1);
            *numSheepToEat = *numSheepToEat - 1;
        }
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowToEat, 1);
            *numCowToEat = *numCowToEat - 1;
        }

        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalPSheepToEat, 1);
        semopChecked(semID, &SignalSDragonEat, 1);
    }
    else {
        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalPSheepToEat, 1);
    }
//    printf("Sheep %d is about to be eaten\n", localpid);
    semopChecked(semID, &WaitSSheepEaten, 1);

    semopChecked(semID, &WaitPSheepEaten, 1);
    *numSheepEaten = *numSheepEaten + 1;
    semopChecked(semID, &SignalPSheepEaten, 1);
    printf("SHEEPSHEEPSHEEPSHEEP    Sheep[%d] is eaten, now %d sheep eaten\n", localpid, *numSheepEaten);
    if(checkSheep()) {
        terminateSimulation();
        exit(0);
    }

    semopChecked(semID, &SignalNMealSheep, 1);
    //keep in this order to avoid deadlock
    semopChecked(semID, &WaitPMealSheep, 1);
    semopChecked(semID, &WaitPMealCow, 1);
    *numMealSheep = *numMealSheep + 1;
    if(*numMealSheep >= SHEEP_IN_MEAL && *numMealCow >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL ; i++) {
            semopChecked(semID, &WaitNMealSheep, 1);
            *numMealSheep = *numMealSheep - 1;
        }

        for (i = 0; i < COW_IN_MEAL ; i++) {
            semopChecked(semID, &WaitNMealCow, 1);
            *numMealCow = *numMealCow - 1;
        }

        semopChecked(semID, &SignalPMealCow, 1);
        semopChecked(semID, &SignalPMealSheep, 1);
        semopChecked(semID, &SignalSMealDone, 1);
    }
    else {
        semopChecked(semID, &SignalPMealCow, 1);
        semopChecked(semID, &SignalPMealSheep, 1);
    }
}

//===================================================================
// HUNTER
//===================================================================
void hunter(int time) {
    pid_t localpid = getpid();
    setpgid(localpid, hunterGID);
    printf("HUNTERHUNTERHUNTER      Hunter[%d] is looking for the path\n", localpid);
    int pathtime = random();
    if (usleep(time) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }

    semopChecked(semID, &SignalNHunterPath, 1);
    semopChecked(semID, &WaitPHunterPath, 1);
    *numHunterPath = *numHunterPath + 1;
    semopChecked(semID, &SignalPHunterPath, 1);

    printf("HUNTERHUNTERHUNTER      Hunter[%d] enters the magic path, now %d hunters in path\n", localpid, *numHunterPath);
    semopChecked(semID, &SignalSDragonWakeUp, 1);

    semopChecked(semID, &WaitSHunterCave, 1);
    printf("HUNTERHUNTERHUNTER      Hunter[%d] enters the cave\n", localpid);
    semopChecked(semID, &SignalSDragonFight, 1);
    semopChecked(semID, &WaitSHunterFight, 1);

    semopChecked(semID, &WaitPHunterLeave, 1);
    *numHunterLeave = *numHunterLeave + 1;
    printf("HUNTERHUNTERHUNTER      Hunter[%d] leaves, now Smaug has fought %d hunters\n", localpid, *numHunterLeave);
    semopChecked(semID, &SignalPHunterLeave, 1);

    if(checkHunter()) {
        terminateSimulation();
    }
    exit(0);
}

//===================================================================
// THIEF
//===================================================================
void thief(int time) {
    pid_t localpid = getpid();
    setpgid(localpid, thiefGID);
    printf("THIEFTHIEFTHIEFTHIEF    Thief[%d] is looking for the path\n", localpid);

    if (usleep(time) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }

    semopChecked(semID, &SignalNThiefPath, 1);
    semopChecked(semID, &WaitPThiefPath, 1);
    *numThiefPath = *numThiefPath + 1;
    semopChecked(semID, &SignalPThiefPath, 1);

    printf("THIEFTHIEFTHIEFTHIEF    Thief[%d] enters the magic path, now %d thieves in path\n", localpid, *numThiefPath);
    semopChecked(semID, &SignalSDragonWakeUp, 1);

    semopChecked(semID, &WaitSThiefCave, 1);
    printf("THIEFTHIEFTHIEFTHIEF    Thief[%d]enters the cave\n", localpid);
    semopChecked(semID, &SignalSDragonPlay, 1);

    semopChecked(semID, &WaitSThiefPlay, 1);

    semopChecked(semID, &WaitPThiefLeave, 1);
    *numThiefLeave = *numThiefLeave + 1;
    printf("THIEFTHIEFTHIEFTHIEF    Thief[%d] leaves, now Smaug has played with %d thieves\n", localpid, *numThiefLeave);
    semopChecked(semID, &SignalPThiefLeave, 1);
    if(checkThief()) {
        terminateSimulation();
    }
    exit(0);
}
//=========================================================
//Smaug
//=========================================================
//smaug eats a meal
void eat() {
    semopChecked(semID, &WaitNMeal, 1); //guarantee that there is really a meal
    int i;
    // let the sheep and cows be ready to be eaten
    for (i = 0; i < SHEEP_IN_MEAL; i++) {
        semopChecked(semID, &SignalSSheepWaiting, 1);
    }
    for (i = 0; i < COW_IN_MEAL; i++) {
        semopChecked(semID, &SignalSCowWaiting, 1);
    }

    //printf("Smaug waits to eat each sheep and cow in the meal\n");
    semopChecked(semID, &WaitSDragonEat, 1);
    printf("DRAGONDRAGONDRAGON      Smaug starts eating\n");
    int j;
    //deal with sheep
    for (j = 0; j < SHEEP_IN_MEAL; j++) {
        semopChecked(semID, &SignalSSheepEaten, 1);
    }
    //deal with cows
    for (j = 0; j < COW_IN_MEAL; j++) {
        semopChecked(semID, &SignalSCowEaten, 1);
    }

    //waiting the meal to be done
    semopChecked(semID, &WaitSMealDone, 1);
    printf("DRAGONDRAGONDRAGON      Smaug finishes eating\n");
    //decrement the number of meals
    *numMeal = *numMeal - 1;
    semopChecked(semID, &WaitPEatenMeal, 1);
    *numEatenMeal = *numEatenMeal + 1;
    printf("DRAGONDRAGONDRAGON      Smaug eats one more snack,%d snacks eaten, %d snacks left\n", *numEatenMeal, *numMeal);
    semopChecked(semID, &SignalPEatenMeal, 1);

}
//smaug swims
void swim() {
    printf("DRAGONDRAGONDRAGON      Smaug starts swimming\n");
    if (usleep(1e6) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }
    printf("DRAGONDRAGONDRAGON      Smaug finishes swimming\n");
}

//smaug fights the treasure hunter
void fight() {

    semopChecked(semID, &WaitNHunterPath, 1);
    *numHunterPath = *numHunterPath - 1;
    printf("DRAGONDRAGONDRAGON      Smaug is ready to interact with a hunter waiting in the path\n");
    semopChecked(semID, &SignalSHunterCave, 1);

    semopChecked(semID, &WaitSDragonFight, 1);
    printf("DRAGONDRAGONDRAGON      Smaug starts fighting with the hunter\n");
    int fightrand = (double)rand()/RAND_MAX;
    if (fightrand > THIEF_WIN) {
        semopChecked(semID, &WaitPDragonJewel, 1);
        *numDragonJewel = *numDragonJewel + 5;
        printf("DRAGONDRAGONDRAGON      Smaug wins and gets 5 jewels, now smaug has %d jewels\n", *numDragonJewel);
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    else {
        semopChecked(semID, &WaitPDragonJewel, 1);

        printf("DRAGONDRAGONDRAGON      The hunter fights well and will get rewarded with 10 jewels\n");
    if(*numDragonJewel >= 10 ) {
        *numDragonJewel = *numDragonJewel - 10; 
        printf("DRAGONDRAGONDRAGON      Smaug gives 10 jewels, now it has %d jewels\n", *numDragonJewel);
    }
        else {
        printf("DRAGONDRAGONDRAGON      Smaug does not have enough jewels, the simulation will terminate\n");
        setTerminate();
        terminateSimulation();
        
    }
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    if(checkJewel()) {
        terminateSimulation();
    }
    semopChecked(semID, &SignalSHunterFight, 1);
}

//smaug plays with the thief
void play() {

    semopChecked(semID, &WaitNThiefPath, 1);
    *numThiefPath = *numThiefPath - 1;
    printf("DRAGONDRAGONDRAGON      Smaug is ready to interact with a thief waiting in the path\n");
    semopChecked(semID, &SignalSThiefCave, 1);

    semopChecked(semID, &WaitSDragonPlay, 1);
    printf("DRAGONDRAGONDRAGON      Smaug fights with the hunter\n");
    int fightrand = (double)rand()/RAND_MAX;
    if (fightrand > HUNTER_WIN) {
        semopChecked(semID, &WaitPDragonJewel, 1);
        *numDragonJewel = *numDragonJewel + 20;
        printf("DRAGONDRAGONDRAGON      Smaug wins and gets 20 jewels, now smaug has %d jewels\n", *numDragonJewel);
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    else {
        semopChecked(semID, &WaitPDragonJewel, 1);
        printf("DRAGONDRAGONDRAGON      Smaug loses and loses 8 jewels\n");
    if(*numDragonJewel >= 8) {
        *numDragonJewel = *numDragonJewel - 8;
        printf("DRAGONDRAGONDRAGON      Smaug loses 8 jewels, now it has %d jewels\n", *numDragonJewel);
    }
    else {
        printf("DRAGONDRAGONDRAGON      Smaug has only %d jewels, now simulation terminates\n", *numDragonJewel);
        setTerminate();
        terminateSimulation();
    }
    semopChecked(semID, &SignalPDragonJewel, 1);
    }
    if(checkJewel()) {
        terminateSimulation();
    }

    semopChecked(semID, &SignalSThiefPlay, 1);
}

void smaug() {
    int newWakeup = 1;
    pid_t localid = getpid();
    smaugID = localid;
    printf("DRAGONDRAGONDRAGON      Smaug[%d] is born\n", smaugID);
    setpgid(smaugID, dragonGID);
    printf("DRAGONDRAGONDRAGON      Smaug[%d] goes to sleep\n", smaugID);

    semopChecked(semID, &WaitPDragonJewel, 1);
    *numDragonJewel = INIT_JEWEL;
    semopChecked(semID, &SignalPDragonJewel, 1);

    semopChecked(semID, &WaitSDragonWakeUp, 1);
    newWakeup = 1;
    int time = 0;
    while (1) {  //sleeping/waking loop
        printf("DRAGONDRAGONDRAGON      Smaug[%d] wakes up\n", smaugID);
        while (1) {  //swimming loop
            // the meal, thief, hunter process in this loop
            int onceMeal = 0;
            int onceThief = 0;
            int onceHunter = 0;

            //looking for meals
            semopChecked(semID, &WaitPNumMeal, 1);
            while(*numMeal > 0 && onceMeal < MEAL_ONCE) {
                if(newWakeup==1) newWakeup = 0;
                else semopChecked(semID, &WaitSDragonWakeUp, 1);

                printf("DRAGONDRAGONDRAGON      Smaug[%d] finds the %d-th snack\n", smaugID, onceMeal+1);
                eat();
                onceMeal ++;
            }
            semopChecked(semID, &SignalPNumMeal, 1);

            //check meal this round
            if(onceMeal > 0) {  //if the
                swim();
                continue;
            }
            else printf("DRAGONDRAGONDRAGON      Smaug[%d] finds no snacks, it looks for thieves\n", smaugID);

            //looking for thieves
            semopChecked(semID, &WaitPThiefPath, 1);
            while(*numThiefPath > 0 && onceThief < THIEF_ONCE) {
                if(newWakeup==1) newWakeup = 0;
                else semopChecked(semID, &WaitSDragonWakeUp, 1);

                printf("DRAGONDRAGONDRAGON      Smaug[%d] finds the %d-th thief\n", smaugID, onceThief+1);
                play();
                onceThief ++;
            }
            semopChecked(semID, &SignalPThiefPath, 1);

            //check thief this round
            if(onceThief > 0) {
                printf("DRAGONDRAGONDRAGON      Smaug[%d] is a happy dragon now, it curls up its jewels and goes to sleep\n", smaugID);
                break;
            }
            else printf("DRAGONDRAGONDRAGON      Smaug[%d] finds no thieves, it looks for hunters\n", smaugID);


            //looking for hunters
            semopChecked(semID, &WaitPHunterPath, 1);
            while(*numHunterPath > 0 && onceHunter < HUNTER_ONCE) {
                if(newWakeup==1) newWakeup = 0;
                else semopChecked(semID, &WaitSDragonWakeUp, 1);

                printf("DRAGONDRAGONDRAGON      Smaug[%d] finds the %d-th hunter\n", smaugID, onceHunter + 1);
                fight();
                onceHunter++;
            }
            semopChecked(semID, &SignalPHunterPath, 1);

            //check hunter
            if(onceHunter > 0) {
                printf("DRAGONDRAGONDRAGON      Smaug[%d] fought with the treasure hunter and goes to swim\n", smaugID);
                swim();
                continue;
            }
            else {
                printf("DRAGONDRAGONDRAGON      Smaug[%d] finds no hunters, there is nothing left, smaug goes to sleep\n", smaugID);
                break;
            }
        }

        printf("DRAGONDRAGONDRAGON      Smaug[%d] goes to sleep\n", smaugID);
        semopChecked(semID, &WaitSDragonWakeUp, 1);
        newWakeup = 1;
    }
}



int main(void) {
    initialize();
    srand(time(NULL));

    int SHEEP_INTERVAL = 0;
    int COW_INTERVAL = 0;
    int THIEF_INTERVAL = 0;
    int HUNTER_INTERVAL = 0;

    long long sheep_time = 0;
    long long cow_time = 0;
    long long thief_time = 0;
    long long hunter_time = 0;

    int sheepSleepTime = 0;
    int cowSleepTime = 0;
    int thiefPathTime = 0;
    int hunterPathTime = 0;

    //========================================================
    printf("Input the Sheep Interval:");
    scanf("%d", &SHEEP_INTERVAL);
    printf("Input the maximum time that a sheep grazes:");
    scanf("%d", &sheepSleepTime);
    //=======================================================
    printf("Input the Cow Interval:");
    scanf("%d", &COW_INTERVAL);
    printf("Input the maximum time that a cow grazes:");
    scanf("%d", &cowSleepTime);
    //=======================================================
    printf("Input the Thief Interval:");
    scanf("%d", &THIEF_INTERVAL);
    printf("Input the maximum time that a thief looks for the path:");
    scanf("%d", &thiefPathTime);
    //========================================================
    printf("Input the Hunter Interval:");
    scanf("%d", &HUNTER_INTERVAL);
    printf("Input the maximum time that a hunter looks for the path:");
    scanf("%d", &hunterPathTime);
    //========================================================

    sheep_time += SHEEP_INTERVAL;
    cow_time += COW_INTERVAL;
    thief_time += THIEF_INTERVAL;
    hunter_time += HUNTER_INTERVAL;
    int genflag = 0; // the flag tells what to generate

    long long elapsetime = 0;
    long long lastelapsetime = 0;
    struct timeval lasttime;
    struct timeval curtime;
    gettimeofday(&lasttime, NULL);

    //produce smaug
    pid_t result = fork();
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
            gettimeofday(&curtime, NULL);
            elapsetime += (curtime.tv_sec - lasttime.tv_sec) * 1000000 + (curtime.tv_usec - lasttime.tv_usec);
//	        pid_t localpid = getpid();
            //printf("In process: %d Elapsed time: %lld\n", localpid,elapsetime);
//            if(elapsetime - lastelapsetime >= 500000)
            lasttime = curtime;

            if (checkTermination()) {

                printf("****************************terminating in parent process**************************************************\n");

                terminateSimulation();
                int status;
                // block till all children exits
                waitpid(-1, &status, 0);
                printf("****************************In main: all children have exited\n");
                releaseResource();
                exit(0);
            }

            if (elapsetime > sheep_time) {
                genflag = 0;
                sheep_time += SHEEP_INTERVAL;
                r = fork();
                if (r == 0) break;
            }

            if (elapsetime > cow_time) {
                genflag = 1;
                cow_time += COW_INTERVAL;
                r = fork();
                if (r == 0) break;
            }

            if (elapsetime > thief_time) {
                genflag = 2;
                thief_time += THIEF_INTERVAL;
                r = fork();
                if (r == 0) break;
            }

            if (elapsetime > hunter_time) {
                genflag = 3;
                hunter_time += HUNTER_INTERVAL;
                r = fork();
                if (r == 0) break;
            }
        }
        //=============================================================
        if (genflag == 0) sheep(rand() % sheepSleepTime);
        else if (genflag == 1) cow(rand() % cowSleepTime);
        else if (genflag == 2) thief(rand() % thiefPathTime);
        else if (genflag == 3) hunter(rand() % hunterPathTime);
        exit(0);
    }
}
