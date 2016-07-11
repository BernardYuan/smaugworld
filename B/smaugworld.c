//
// Created by Bernard Yuan on 2016-07-11.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_SHEEP 14
#define MAX_COW 14

#define SHEEP_IN_MEAL 2
#define COW_IN_MEAL 2
//global variables
int numMeal;
int numSheepInValley;
int numSheepWaiting;
int numSheepEaten;
int numCowInValley;
int numCowWaiting;
int numCowEaten;
int condTerminate;
//semaphores and mutexes
//dragon
sem_t semSDragonSleep;
sem_t semSDragonEat;

//meals
sem_t semNMeal;
sem_t mtxNumMeal;

//sheep
sem_t semNSheepInValley;
sem_t mtxNumSheepInValley;

sem_t semSSheepWaiting;
sem_t semNSheepWaiting;
sem_t mtxNumSheepWaiting;

sem_t semSSheepEaten;
sem_t mtxNumSheepEaten;
sem_t semSSheepDie;

//cows
sem_t semNCowInValley;
sem_t mtxNumCowInValley;

sem_t semSCowWaiting;
sem_t semNCowWaiting;
sem_t mtxNumCowWaiting;

sem_t semSCowEaten;
sem_t mtxNumCowEaten;
sem_t semSCowDie;

//termination condition
sem_t mtxCondTerminate;

void initialize() {
    // Dragon
    assert(sem_init(&semSDragonSleep, 1, 0) == 0);
    assert(sem_init(&semSDragonEat, 1, 0) == 0);
    //Meal
    assert(sem_init(&semNMeal, 1, 0) == 0);
    assert(sem_init(&mtxNumMeal, 1, 1) == 0);
    //Sheep
    assert(sem_init(&semNSheepInValley, 1, 0) == 0);
    assert(sem_init(&mtxNumSheepInValley, 1, 1) == 0);
    assert(sem_init(&semSSheepWaiting, 1, 0) == 0);
    assert(sem_init(&semNSheepWaiting, 1, 0) == 0);
    assert(sem_init(&mtxNumSheepWaiting, 1, 1) == 0);
    assert(sem_init(&semSSheepEaten, 1, 0) == 0);
    assert(sem_init(&mtxNumSheepEaten, 1, 1) == 0);
    assert(sem_init(&semSSheepDie, 1, 0) == 0);
    //Cow
    assert(sem_init(&semNCowInValley, 1, 0) == 0);
    assert(sem_init(&mtxNumCowInValley, 1, 1) == 0);

    assert(sem_init(&semSCowWaiting, 1, 0) == 0);
    assert(sem_init(&semNCowWaiting, 1, 0) == 0);
    assert(sem_init(&mtxNumCowWaiting, 1, 1) == 0);

    assert(sem_init(&semSCowEaten, 1, 0) == 0);
    assert(sem_init(&mtxNumCowEaten, 1, 1) == 0);
    assert(sem_init(&semSCowDie, 1, 0) == 0);
    //Termination Condition
    assert(sem_init(&mtxCondTerminate, 1, 1) == 0);
    printf("initialization finished\n");
}

void eat() {
    sem_wait(&semNMeal);
    numMeal = numMeal - 1;

    int i;
    for (i = 0; i < SHEEP_IN_MEAL; i++) {
        sem_post(&semSSheepWaiting);

    }
    for (i = 0; i < COW_IN_MEAL; i++) {
        sem_post(&semSCowWaiting);
    }
    sem_wait(&semSDragonEat);
    printf("Smaug starts eating\n");

    //keep in this order to avoid deadlock
    sem_wait(&mtxNumSheepEaten);
    sem_wait(&mtxNumCowEaten);
    for (i = 0; i < SHEEP_IN_MEAL; i++) {
        sem_post(&semSSheepEaten);
        sem_wait(&semSSheepDie);
        numSheepEaten++;
        printf("Smaug eats another sheep, now %d sheep eaten\n", numSheepEaten);
    }

    for (i = 0; i < COW_IN_MEAL; i++) {
        sem_post(&semSCowEaten);
        sem_wait(&semSCowDie);
        numCowEaten++;
        printf("Smaug eats another cow, now %d cows eaten\n", numCowEaten);
    }
    sem_post(&mtxNumCowEaten);
    sem_post(&mtxNumSheepEaten);
}

void swim() {
    printf("Smaug starts swimming\n");
    usleep(1e6);
    printf("Smaug finishes swimming\n");
}

int checkSheep() {
    sem_wait(&mtxNumSheepEaten);
    if(numSheepEaten >= MAX_SHEEP) {
        sem_wait(&mtxCondTerminate);
        condTerminate = 1;
        sem_post(&mtxCondTerminate);
        sem_post(&mtxNumSheepEaten);
        return 1;
    }
    else {
        sem_post(&mtxNumSheepEaten);
        return 0;
    }
}

int checkCow() {
    sem_wait(&mtxNumCowEaten);
    if(numCowEaten >= MAX_COW) {
        sem_wait(&mtxCondTerminate);
        condTerminate = 1;
        sem_post(&mtxCondTerminate);
        sem_post(&mtxNumCowEaten);
        return 1;
    }
    else {
        sem_post(&mtxNumCowEaten);
        return 0;
    }
}

void *smaug() {
    printf("Smaug sleeps\n");
    sem_wait(&semSDragonSleep);
    while (1) {  //sleeping loop
        printf("Smaug wakes up\n");
        while (1) { //swimming loop
            sem_wait(&mtxNumMeal);
            if (numMeal > 0) {
                printf("Smaug finds a meal\n");
                eat();
                sem_post(&mtxNumMeal);
                if(checkSheep() || checkCow()) {
                    break;
                }
                sem_wait(&mtxNumMeal);
                if (numMeal > 0) {
                    printf("Smaug finds the second meal\n");
                    eat();
                    sem_post(&mtxNumMeal);
                    if(checkSheep() || checkCow()) {
                        break;
                    }
                }
                else {
                    printf("There is no second meal, smaug goes to swim");
                    sem_post(&mtxNumMeal);
                    break;
                }
            }
            else {
                printf("Smaug finds no meal\n");
                sem_post(&mtxNumMeal);
                break;
            }
            swim();
        }

        if(checkTerminate()) break;
        sem_wait(&semSDragonSleep);
    }0
    pthread_exit();
}

void *sheep(int time) {
    usleep(time);
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

void *cow(int time) {
    usleep(time);
    // get the control of two shared variables
    // !!! always keep in this order, to avoid deadlock
    sem_wait(&mtxNumSheepInValley);
    sem_wait(&mtxNumCowInValley);

    numCowInValley += 1;
    sem_post(&semNCowInValley);

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
            sum_wait(&semNCowWaiting);
            numCowWaiting--;
        }
        printf("The last cow in the snack is ready. Smaug will eat\n");
        sem_post(&semSDragonEat);
    }
    sem_post(&mtxNumCowWaiting);
    sem_post(&mtxNumSheepWaiting);

    sem_wait(&semSCowEaten);
    printf("A cow is dead\n");
    sem_post(&semSCowDie);
}

int checkTerminate() {
    sem_wait(&mtxCondTerminate);
    if (condTerminate == 1) {
        sem_post(&mtxCondTerminate);
        return 1;
    }
    else {
        sem_post(&mtxCondTerminate);
        return 0;
    }
}

int main(void) {
    pthread_t tSmaug;
    pthread_t tSheep;
    pthread_t tCow;

    initialize();
    srand(time(NULL));
    pthread_create(&tSmaug, NULL, smaug, NULL);
    while (1) {
        if (checkTerminate()) break;

        int gr = rand() % 2;
        if (gr == 0) {
            printf("Spawn a new sheep\n");
            int time = rand() % 5555;
            pthread_create(&tSheep, NULL, sheep, NULL);
        }
        else {
            printf("Spawn a new cow\n");
            int time = rand() % 5555;
            pthread_create(&tCow, NULL, cow, NULL);
        }
        usleep(1e6);
    }
    pthread_exit();
}