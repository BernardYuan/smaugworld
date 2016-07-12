//
// Created by Bernard Yuan on 2016-07-11.
//
#include "smaugworld.h"

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