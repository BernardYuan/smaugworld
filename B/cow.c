//
// Created by Bernard Yuan on 2016-07-12.
//
#include "cow.h"
void *cow(void* time) {
    pthread_id_np_t tid = pthread_getthreadid_np();
    printf("Cow %d is grazing for %d usec\n", tid, (int)time);
    usleep((int)time);
	printf("Cow %d is enchanted\n", tid);

    // get the control of two shared variables
    // !!! always keep in this order, to avoid deadlock
    sem_wait(&mtxNumSheepInValley);
    sem_wait(&mtxNumCowInValley);
    numCowInValley += 1;
    sem_post(&semNCowInValley);
	printf("Now there is %d sheep and %d cows in Valley\n", numSheepInValley, numCowInValley);

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

    sem_wait(&semSCowWaiting);

    sem_post(&semNCowWaiting);
    //checking whether all beasts are ready to be eaten
    sem_wait(&mtxNumSheepWaiting);
    sem_wait(&mtxNumCowWaiting);
    numCowWaiting++;
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
        printf("The last cow in the snack is ready. Smaug will eat\n");
        sem_post(&semSDragonEat);
    }
    sem_post(&mtxNumCowWaiting);
    sem_post(&mtxNumSheepWaiting);

    sem_wait(&semSCowEaten);
	sem_wait(&mtxNumCowEaten);
	numCowEaten += 1;
    printf("A cow is dead, now %d cows eaten\n", numCowEaten);
	sem_post(&mtxNumCowEaten);

	if(checkCow()) {
		pthread_exit(NULL);
	}

	sem_post(&semNMealCow);

	sem_wait(&mtxNumMealSheep);
	sem_wait(&mtxNumMealCow);
	numMealCow += 1;
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
	printf("The last Cow %d dies and the snack is done\n", tid);
	sem_post(&semSMealDone);
	pthread_exit(NULL);
}
