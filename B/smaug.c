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
	printf("Smaug waits to eat\n");
    sem_wait(&semSDragonEat);
    printf("Smaug starts eating\n");

    for (i = 0; i < SHEEP_IN_MEAL; i++) {
        sem_post(&semSSheepEaten);
    }
    for (i = 0; i < COW_IN_MEAL; i++) {
        sem_post(&semSCowEaten);
    }
	sem_wait(&semSMealDone);
	numMeal = numMeal - 1;
	printf("Smaug eats another meal\n");
}

void swim() {
    printf("Smaug starts swimming\n");
    usleep(1e6);
    printf("Smaug finishes swimming\n");
}

void* smaug(void *arg) {
    printf("Smaug goes to sleep\n");
    sem_wait(&semSDragonSleep);
    while (1) {  //sleeping loop
        printf("Smaug wakes up\n");
        while (1) { //swimming loop
			int onceMeal = 0;
            sem_wait(&mtxNumMeal);
			while(numMeal > 0 && onceMeal < ONCE_MEAL) {
				printf("Smaug finds the %d-th meal\n", onceMeal);
				if(onceMeal > 0) sem_wait(&semSDragonSleep);
				eat();
				onceMeal ++;
			}
			sem_post(&mtxNumMeal);
			if(onceMeal > 0) {
				swim();
				continue;
			}
			else {
				printf("Smaug finds nothing\n");
				break;
			}
        }
		if(checkTerminate()) break;
		printf("Smaug goes to sleep\n");
        sem_wait(&semSDragonSleep);
    }
    pthread_exit(NULL);
}
