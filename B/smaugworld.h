//
// Created by Bernard Yuan on 2016-07-12.
//

#ifndef SMAUGWORLD_SMAUGWORLD_H
#define SMAUGWORLD_SMAUGWORLD_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

#define MAX_SHEEP 14
#define MAX_COW 14
#define SHEEP_IN_MEAL 2
#define COW_IN_MEAL 2
#define ONCE_MEAL 2

//global variables
extern int numMeal;

extern int numMealSheep;
extern int numMealCow;

extern int numSheepInValley;
extern int numSheepWaiting;
extern int numSheepEaten;
extern int numCowInValley;
extern int numCowWaiting;
extern int numCowEaten;
extern int condTerminate;

//semaphores and mutexes
//dragon
extern sem_t semSDragonSleep;
extern sem_t semSDragonEat;
//meals
extern sem_t semNMeal;
extern sem_t mtxNumMeal;
//new for meals
extern sem_t semNMealSheep;
extern sem_t semNMealCow;
extern sem_t mtxNumMealSheep;
extern sem_t mtxNumMealCow;
extern sem_t semSMealDone;
//sheep
extern sem_t semNSheepInValley;
extern sem_t mtxNumSheepInValley;
extern sem_t semSSheepWaiting;
extern sem_t semNSheepWaiting;
extern sem_t mtxNumSheepWaiting;
extern sem_t semSSheepEaten;
extern sem_t mtxNumSheepEaten;
extern sem_t semNSheepEaten;
//cows
extern sem_t semNCowInValley;
extern sem_t mtxNumCowInValley;
extern sem_t semSCowWaiting;
extern sem_t semNCowWaiting;
extern sem_t mtxNumCowWaiting;
extern sem_t semSCowEaten;
extern sem_t mtxNumCowEaten;
extern sem_t semNCowEaten;
//termination condition
extern sem_t mtxCondTerminate;

struct beastarg{
    int time;
    int no;
};
int checkSheep();
int checkCow();
int checkTerminate();
void setTerminate();
void releaseResource();

#endif //SMAUGWORLD_SMAUGWORLD_H

