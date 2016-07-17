//
// Created by Bernard Yuan on 2016-07-12.
//
#include "sheep.h"
void *sheep(void* arg) {
    int time = ((struct beastarg *)arg)->time;
    int id = ((struct beastarg *)arg)->no;

    printf("Sheep %d is grazing for %d usec\n", id, time);
    usleep(time);
    printf("Sheep %d is enchanted\n", id);

    // get the control of two shared variables
    // !!! always keep in this order, to avoid deadlock
    sem_wait(&mtxNumSheepInValley);
    sem_wait(&mtxNumCowInValley);
    numSheepInValley += 1;
    sem_post(&semNSheepInValley);
    printf("Now %d sheep and %d cows in valley\n", numSheepInValley, numCowInValley);
	
    if (numSheepInValley >= SHEEP_IN_MEAL && numCowInValley >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            sem_wait(&semNSheepInValley);
            numSheepInValley -= 1;
        }
        for (i = 0; i < COW_IN_MEAL; i++) {
            sem_wait(&semNCowInValley);
            numCowInValley -= 1;
        }

        sem_wait(&mtxNumMeal);
        sem_post(&semNMeal);
        numMeal += 1;
        printf("A new meal is added, now there are %d meals\n", numMeal);
        sem_post(&mtxNumMeal);
        sem_post(&semSDragonSleep);
    }
    //release two shared variables
    sem_post(&mtxNumCowInValley);
    sem_post(&mtxNumSheepInValley);

    sem_wait(&semSSheepWaiting);

    sem_post(&semNSheepWaiting);

    //checking whether all beasts are ready to be eaten
    sem_wait(&mtxNumSheepWaiting);
    sem_wait(&mtxNumCowWaiting);
    numSheepWaiting++;
    if (numSheepWaiting >= SHEEP_IN_MEAL && numCowWaiting >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            sem_wait(&semNSheepWaiting);
            numSheepWaiting--;
        }
        for (i = 0; i < COW_IN_MEAL; i++) {
            sem_wait(&semNCowWaiting);
            numCowWaiting--;
        }
        printf("The last sheep in the snack is ready. Smaug will eat\n");
        sem_post(&semSDragonEat);
    }
    sem_post(&mtxNumCowWaiting);
    sem_post(&mtxNumSheepWaiting);

    sem_wait(&semSSheepEaten);
	
	sem_wait(&mtxNumSheepEaten);
	numSheepEaten += 1;
	printf("Sheep %d is dead, now %d sheep eaten\n", id, numSheepEaten);
	sem_post(&mtxNumSheepEaten);

	if(checkSheep()) {
		pthread_exit(NULL);
	}

	sem_post(&semNMealSheep);

	sem_wait(&mtxNumMealSheep);
	sem_wait(&mtxNumMealCow);
	numMealSheep += 1;
	if(numMealSheep >= SHEEP_IN_MEAL && numMealCow >= COW_IN_MEAL) {
		int i = 0;
		for(i = 0 ; i < SHEEP_IN_MEAL ; i++ ) {
			sem_wait(&semNMealSheep);
			numMealSheep -= 1;
		}
		for(i = 0 ; i < COW_IN_MEAL ; i++ ) {
			sem_wait(&semNMealCow);
			numMealCow -= 1;
		}
	}
	sem_post(&mtxNumMealCow);
	sem_post(&mtxNumMealSheep);
	printf("The last sheep %d dies and the snack is done\n", id);
	sem_post(&semSMealDone);
	pthread_exit(NULL);
}
