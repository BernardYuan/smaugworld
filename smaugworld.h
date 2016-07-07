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

// definitions of the semaphores
#define MTX_DRAGONWAKEUP 0 //wake up the dragon
#define MTX_DRAGONEAT 1    //the dragon eats
#define MTX_MEAL 2         //num of meals in the valley
#define MTX_SHEEPEATEN 3   //number of eaten sheep
#define MTX_MEALEATEN 4    //number of eaten meal
#define MTX_SHEEPINVALLEY 5//number of sheep in valley
#define SEM_SHEEPINVALLEY 6//semaphore of sheep in valley
#define SEM_SHEEPEATEN 7   //semaphore of eaten sheep
//function initialization
void initialize();
void releaseResource();
void semctlChecked(int semID, int semNum, int flag, union semun seminfo); 
void semopChecked(int semID, struct sembuf *operation, unsigned num);

//make variables external so that they are visible in other files
extern int semID;
extern int *SheepInValley;
extern int *numMeals;
extern int *numMealsEaten;
extern int *numSheepEaten;

extern union semun seminfo;
// semaphore buffers 
// dragon sleep:
extern struct sembuf WaitMutexDragonWakeUp;
extern struct sembuf SignalMutexDragonWakeUp;

//dragon eat:
extern struct sembuf WaitMutexDragonEat;
extern struct sembuf SignalMutexDragonEat;

//mutex on meal
extern struct sembuf WaitMutexMeal;
extern struct sembuf SignalMutexMeal;

//mutex on sheep eaten
extern struct sembuf WaitMutexSheepEaten;
extern struct sembuf SignalMutexSheepEaten;

//mutex on meal eaten
extern struct sembuf WaitMutexMealEaten;
extern struct sembuf SignalMutexMealEaten;

//mutex sheep in valley
extern struct sembuf WaitMutexSheepInValley;
extern struct sembuf SignalMutexSheepInValley;

//semaphore sheep in valley
extern struct sembuf WaitSheepInValley;
extern struct sembuf SignalSheepInValley;

//semaphore sheep eaten by dragon
extern struct sembuf WaitSheepEaten;
extern struct sembuf SignalSheepEaten;

