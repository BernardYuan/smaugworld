#include "sheep.h"
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
	// There is a meal in the Valley
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
		semopChecked(semID, &SignalMutexSheepInValley, 1);
		semopChecked(semID, &SignalMutexDragonWakeUp, 1);
	}
	else { //the sheep in the Valley is not enough for a meal
		semopChecked(semID, &SignalMutexSheepInValley, 1);
	}
	//sheep goes into the queue waiting to be eaten
	semopChecked(semID, &WaitSheepEaten, 1);
	semopChecked(semID, &WaitMutexSheepEaten, 1);
	*numSheepEaten = *numSheepEaten + 1;
	semopChecked(semID, &SignalMutexSheepEaten, 1);
	printf("Sheep %d is eaten\n", localpid);
}

