//
// Created by Bernard Yuan on 2016-07-12.
//
#include "smaug.h"
void eat() {
    sem_wait(&semNMeal);
    int i;
    for (i = 0; i < SHEEP_IN_MEAL; i++) {
        sem_post(&semSSheepWaiting);

    }
    for (i = 0; i < COW_IN_MEAL; i++) {
        sem_post(&semSCowWaiting);
    }

    sem_wait(&semSDragonEat);
    printf("DRAGONDRAGONDRAGON      Smaug starts eating\n");

    for (i = 0; i < SHEEP_IN_MEAL; i++) {
        sem_post(&semSSheepEaten);
    }
    for (i = 0; i < COW_IN_MEAL; i++) {
        sem_post(&semSCowEaten);
    }
	sem_wait(&semSMealDone);
	numMeal = numMeal - 1;
	printf("DRAGONDRAGONDRAGON      Smaug finishes eating\n");
}

void swim() {
    printf("DRAGONDRAGONDRAGON      Smaug starts swimming\n");
    usleep(1e6);
    printf("DRAGONDRAGONDRAGON      Smaug finishes swimming\n");
}

void* smaug(void *arg) {
    printf("DRAGONDRAGONDRAGON      Smaug is created\n");
    printf("DRAGONDRAGONDRAGON      Smaug goes to sleep\n");
    sem_wait(&semSDragonSleep);
    int newWakeup = 1;
    while (1) {  //sleeping loop
        printf("DRAGONDRAGONDRAGON      Smaug wakes up\n");
        while (1) { //swimming loop
			int onceMeal = 0;
            sem_wait(&mtxNumMeal);
			while(numMeal > 0 && onceMeal < ONCE_MEAL) {
				printf("DRAGONDRAGONDRAGON      Smaug finds the %d-th meal\n", onceMeal+1);
				if(newWakeup) newWakeup = 0;
                else sem_wait(&semSDragonSleep);

				eat();
				onceMeal ++;
			}
			sem_post(&mtxNumMeal);
			if(onceMeal > 0) {
				swim();
				continue;
			}
			else {
				printf("DRAGONDRAGONDRAGON      Smaug finds nothing and goes to sleep\n");
				break;
			}
        }
		if(checkTerminate()) break;
		printf("DRAGONDRAGONDRAGON      Smaug goes to sleep\n");
        sem_wait(&semSDragonSleep);
        newWakeup = 1;
    }
    pthread_exit(NULL);
}
