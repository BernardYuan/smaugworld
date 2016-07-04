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
