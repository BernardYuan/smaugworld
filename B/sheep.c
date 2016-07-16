//
// Created by Bernard Yuan on 2016-07-12.
//
#include "sheep.h"
void *sheep(void* time) {
    usleep((int)time);
    // get the control of two shared variables
    // !!! always keep in this order, to avoid deadlock
    sem_wait(&mtxNumSheepInValley);
    sem_wait(&mtxNumCowInValley);

    numSheepInValley += 1;
    sem_post(&semNSheepInValley);

    if (numSheepInValley >= SHEEP_IN_MEAL && numCowInValley > COW_IN_MEAL) {
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
            sum_wait(&semNCowWaiting);
            numCowWaiting--;
        }
        printf("The last sheep in the snack is ready. Smaug will eat\n");
        sem_post(&semSDragonEat);
    }
    sem_post(&mtxNumCowWaiting);
    sem_post(&mtxNumSheepWaiting);

    sem_wait(&semSSheepEaten);
    printf("A sheep is dead");
    sem_post(&semSSheepDie);
}