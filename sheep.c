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
	semopChecked(semID, &WaitPSheepInValley, 1);
	*SheepInValley = *SheepInValley + 1;
	semopChecked(semID, &SignalNSheepInValley, 1);
	printf("Now %d sheep in the valley\n",*SheepInValley);
	// There is a meal in the Valley
	if(*SheepInValley >= SHEEP_IN_MEAL) {
		int i;
		for( i = 0 ; i < SHEEP_IN_MEAL ; i++) {
			semopChecked(semID, &WaitNSheepInValley, 1);
		}
		*SheepInValley = *SheepInValley - SHEEP_IN_MEAL;
		semopChecked(semID, &WaitPNumMeal, 1);
		*numMeal = *numMeal + 1;
		printf("A new meal is added, now number of meals:%d\n", *numMeal);
		semopChecked(semID, &SignalNMeal, 1);
		semopChecked(semID, &SignalPNumMeal, 1);

		semopChecked(semID, &SignalPSheepInValley, 1);
		printf("The last sheep in the meal %d wakes up the dragon\n", localpid);
		semopChecked(semID, &SignalSDragonWakeUp, 1);
	}
	else { //the sheep in the Valley is not enough for a meal
		semopChecked(semID, &SignalPSheepInValley, 1);
	}

	printf("Sheep %d waiting in the snack\n", localpid);
	semopChecked(semID, &WaitSSheepWaiting, 1);

	semopChecked(semID, &SignalNSheepToEat, 1);
	semopChecked(semID, &WaitPSheepToEat, 1);
	*numSheepToEat = *numSheepToEat + 1;
	if(*numSheepToEat >= SHEEP_IN_MEAL) {
		int i; 
		for( i = 0 ; i < SHEEP_IN_MEAL ; i++ ) {
			semopChecked(semID, &WaitNSheepToEat, 1);
		}
		*numSheepToEat = *numSheepToEat - SHEEP_IN_MEAL;	
		printf("The last sheep in a meal ready to be eaten signals smaug to eat\n");
		semopChecked(semID, &SignalPSheepToEat, 1);
		semopChecked(semID, &SignalSDragonEat, 1);
	}
	else {
		semopChecked(semID, &SignalPSheepToEat, 1);
	}

	printf("Sheep %d is about to be eaten\n", localpid);
	semopChecked(semID, &WaitSSheepEaten, 1);
	printf("Sheep %d is eaten\n", localpid);
	semopChecked(semID, &SignalSSheepDie, 1);
}

