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
#define SEM_P_DRAGONWAKEUP   2
extern int DragonWakeUpFlag;
extern int *DragonWakeUp;
extern struct sembuf WaitPDragonWakeUp;
extern struct sembuf SignalPDragonWakeUp;
#define SEM_P_DRAGONJEWEL    3
extern int numDragonJewelFlag;
extern int *numDragonJewel;
extern struct sembuf WaitPDragonJewel;
extern struct sembuf SignalPDragonJewel;
#define SEM_S_DRAGONFIGHT 4
extern struct sembuf WaitSDragonFight;
extern struct sembuf SignalSDragonFight;
#define SEM_S_DRAGONPLAY
extern struct sembuf WaitSDragonPlay;
extern struct sembuf SignalSDragonPlay;

//semahpores of meals
#define SEM_N_MEAL           4
extern struct sembuf WaitNMeal;
extern struct sembuf SignalNMeal;
#define SEM_P_NUMMEAL        5
extern int numMealFlag;
extern int *numMeal;
extern struct sembuf WaitPNumMeal;
extern struct sembuf SignalPNumMeal;
#define SEM_P_EATENMEAL      6
extern int numEatenMealFlag;
extern int *numEatenMeal;
extern struct sembuf WaitPEatenMeal;
extern struct sembuf SignalPEatenMeal;

//semaphores of sheep
#define SEM_N_SHEEPINVALLEY  7
extern struct sembuf WaitNSheepInValley;
extern struct sembuf SignalNSheepInValley;
#define SEM_P_SHEEPINVALLEY  8
extern int SheepInValleyFlag;
extern int *SheepInValley;
extern struct sembuf WaitPSheepInValley;
extern struct sembuf SignalPSheepInValley;
#define SEM_S_SHEEPWAITING   9
extern struct sembuf WaitSSheepWaiting;
extern struct sembuf SignalSSheepWaiting;
#define SEM_N_SHEEPTOEAT     10
extern struct sembuf WaitNSheepToEat;
extern struct sembuf SignalNSheepToEat;
#define SEM_P_SHEEPTOEAT     11
extern int numSheepToEatFlag;
extern int *numSheepToEat;
extern struct sembuf WaitPSheepToEat;
extern struct sembuf SignalPSheepToEat;
#define SEM_S_SHEEPEATEN     12
extern struct sembuf WaitSSheepEaten;
extern struct sembuf SignalSSheepEaten;
#define SEM_P_SHEEPEATEN     13
extern int numSheepEatenFlag;
extern int *numSheepEaten;
extern struct sembuf WaitPSheepEaten;
extern struct sembuf SignalPSheepEaten;
#define SEM_S_SHEEPDIE       14
extern struct sembuf WaitSSheepDie;
extern struct sembuf SignalSSheepDie;

//semaphores of cow
#define SEM_N_COWINVALLEY    15
extern struct sembuf WaitNCowInValley;
extern struct sembuf SignalNCowInValley;
#define SEM_P_COWINVALLEY    16
extern int CowInValleyFlag;
extern int *CowInValley;
extern struct sembuf WaitPCowInValley;
extern struct sembuf SignalPCowInValley;
#define SEM_S_COWWAITING     17
extern struct sembuf WaitSCowWaiting;
extern struct sembuf SignalSCowWaiting;
#define SEM_N_COWTOEAT       18
extern struct sembuf WaitNCowToEat;
extern struct sembuf SignalNCowToEat;
#define SEM_P_COWTOEAT       19
extern int numCowToEatFlag;
extern int *numCowToEat;
extern struct sembuf WaitPCowToEat;
extern struct sembuf SignalPCowToEat;
#define SEM_S_COWEATEN       20
extern struct sembuf WaitSCowEaten;
extern struct sembuf SignalSCowEaten;
#define SEM_P_COWEATEN       21
extern int numCowEatenFlag;
extern int *numCowEaten;
extern struct sembuf WaitPCowEaten;
extern struct sembuf SignalPCowEaten;
#define SEM_S_COWDIE         22
extern struct sembuf WaitSCowDie;
extern struct sembuf SignalSCowDie;

//semaphores of hunters
#define SEM_N_HUNTERPATH   23
extern struct sembuf WaitNHunterPath;
extern struct sembuf SignalNHunterPath;
#define SEM_P_HUNTERPATH   24
extern int numHunterPathFlag;
extern int *numHunterPath;
extern struct sembuf WaitPHunterPath;
extern struct sembuf SignalPHunterPath;
#define SEM_S_HUNTERCAVE   25
extern struct sembuf WaitSHunterCave;
extern struct sembuf SignalSHunterCave;
#define SEM_S_HUNTERFIGHT 28
extern struct sembuf WaitSHunterFight;
extern struct sembuf SignalSHunterFight;
#define SEM_S_HUNTERLEAVE 26
extern struct sembuf WaitSHunterLeave;
extern struct sembuf SignalSHunterLeave;
#define SEM_P_HUNTERLEAVE 27
extern int numHunterLeaveFlag;
extern int *numHunterLeave;
extern struct sembuf WaitPHunterLeave;
extern struct sembuf SignalPHunterLeave;

//semaphores of thieves
#define SEM_N_THIEFPATH
extern struct sembuf WaitNThiefPath;
extern struct sembuf SignalNThiefPath;
#define SEM_P_THIEFPATH
extern int numThiefPathFlag;
extern int *numThiefPath;
extern struct sembuf WaitPThiefPath;
extern struct sembuf SignalPThiefPath;
#define SEM_S_THIEFCAVE
extern struct sembuf WaitSThiefCave;
extern struct sembuf SignalSThiefCave;
#define SEM_S_THIEFPLAY
extern struct sembuf WaitSThiefPlay;
extern struct sembuf SignalSThiefPlay;
#define SEM_S_THIEFLEAVE
extern struct sembuf WaitSThiefLeave;
extern struct sembuf SignalSThiefLeave;
#define SEM_P_THIEFLEAVE
extern int numThiefLeaveFlag;
extern int *numThiefLeave;
extern struct sembuf WaitPThiefLeave;
extern struct sembuf SignalPThiefLeave;

//make variables external so that they are visible in other files
extern int semID;
extern union semun seminfo;

//function initialization
void initialize();

void releaseResource();

void semctlChecked(int semID, int semNum, int flag, union semun seminfo);

void semopChecked(int semID, struct sembuf *operation, unsigned num);

void shmAllocate(key_t key, size_t size, int shmflg1, const void *shmaddr, int shmflg2, int *flag, int **addr);
