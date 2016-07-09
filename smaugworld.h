//necessary header files
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

// semaphores of the state of the dragon
#define SEM_S_DRAGONWAKEUP   0
extern struct sembuf WaitSDragonWakeUp;
extern struct sembuf SignalSDragonWakeUp;
#define SEM_S_DRAGONEAT      1
extern struct sembuf WaitSDragonEat;
extern struct sembuf SignalSDragonEat;

//semahpores of meals
#define SEM_N_MEAL           2
extern struct sembuf WaitNMeal;
extern struct sembuf SignalNMeal;
#define SEM_P_NUMMEAL           3
extern int numMealFlag;
extern int *numMeal;
extern struct sembuf WaitPNumMeal;
extern struct sembuf SignalPNumMeal;
#define SEM_P_EATENMEAL      4
extern int numEatenMealFlag;
extern int *numEatenMeal;
extern struct sembuf WaitPEatenMeal;
extern struct sembuf SignalPEatenMeal;

//semaphores of sheep
#define SEM_N_SHEEPINVALLEY  5
extern struct sembuf WaitNSheepInValley;
extern struct sembuf SignalNSheepInValley;
#define SEM_P_SHEEPINVALLEY  6
extern int SheepInValleyFlag;
extern int *SheepInValley;
extern struct sembuf WaitPSheepInValley;
extern struct sembuf SignalPSheepInValley;
#define SEM_S_SHEEPWAITING   7
extern struct sembuf WaitSSheepWaiting;
extern struct sembuf SignalSSheepWaiting;
#define SEM_N_SHEEPTOEAT    8
extern struct sembuf WaitNSheepToEat;
extern struct sembuf SignalNSheepToEat;
#define SEM_P_SHEEPTOEAT    9
extern int numSheepToEatFlag;
extern int *numSheepToEat;
extern struct sembuf WaitPSheepToEat;
extern struct sembuf SignalPSheepToEat;
#define SEM_S_SHEEPEATEN     10
extern struct sembuf WaitSSheepEaten;
extern struct sembuf SignalSSheepEaten;
#define SEM_P_SHEEPEATEN     11
extern int numSheepEatenFlag;
extern int *numSheepEaten;
extern struct sembuf WaitPSheepEaten;
extern struct sembuf SignalPSheepEaten;
#define SEM_S_SHEEPDIE         12
extern struct sembuf WaitSSheepDie;
extern struct sembuf SignalSSheepDie;

//make variables external so that they are visible in other files
extern int semID;
extern union semun seminfo;

//function initialization
void initialize();

void releaseResource();

void semctlChecked(int semID, int semNum, int flag, union semun seminfo);

void semopChecked(int semID, struct sembuf *operation, unsigned num);

void shmAllocate(key_t key, size_t size, int shmflg1, const void *shmaddr, int shmflg2, int *flag, int **addr);
