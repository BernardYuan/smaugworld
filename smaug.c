#include "smaug.h"
void smaug() {
	pid_t localid;
	localid = getpid();
	printf("ProcessID of smaug:%d\n", localid);
	printf("Smaug is sleeping\n");
	semopChecked(semID, &WaitMutexDragonWakeUp, 1);
	int time = 0;
	while(1) {
		printf("Smaug is awake\n");
		semopChecked(semID, &WaitMutexMeal, 1);
		if(*numMeals>0) {
			*numMeals = *numMeals - 1;
			printf("smaug eats one meal, %d meals left\n", *numMeals);
			semopChecked(semID, &WaitMutexDragonEat, 1);

			semopChecked(semID, &WaitMutexMealEaten, 1);	
			*numMealsEaten = *numMealsEaten + 1;
			int i=0;
			for (i = 0 ; i < SHEEP_IN_MEAL ; i++) {
				semopChecked(semID, &SignalSheepEaten, 1);
			}
			semopChecked(semID, &SignalMutexMealEaten, 1);
		}
		semopChecked(semID, &SignalMutexMeal, 1);


		semopChecked(semID, &WaitMutexMeal, 1);
		if(*numMeals>0) {
			*numMeals = *numMeals - 1;
			printf("smaug eats another meal, %d meals left\n", *numMeals);
		}
		semopChecked(semID, &SignalMutexMeal, 1);
		printf("Smaug finishes snacks and goes sleeping\n");
		
		semopChecked(semID, &WaitMutexDragonWakeUp, 1);
	}
}

