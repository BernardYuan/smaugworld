// Created by Bernard Yuan on 2016-07-11.
//
#include "smaugworld.h"
#include "smaug.h"
#include "sheep.h"
#include "cow.h"

//global variables
int numMeal;
int numMealSheep;
int numMealCow;
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
//new for meals
sem_t semNMealSheep;
sem_t semNMealCow;
sem_t mtxNumMealSheep;
sem_t mtxNumMealCow;
sem_t semSMealDone;
//sheep
sem_t semNSheepInValley;
sem_t mtxNumSheepInValley;
sem_t semSSheepWaiting;
sem_t semNSheepWaiting;
sem_t mtxNumSheepWaiting;
sem_t semSSheepEaten;
sem_t mtxNumSheepEaten;
//cows
sem_t semNCowInValley;
sem_t mtxNumCowInValley;
sem_t semSCowWaiting;
sem_t semNCowWaiting;
sem_t mtxNumCowWaiting;
sem_t semSCowEaten;
sem_t mtxNumCowEaten;
//termination condition
sem_t mtxCondTerminate;

void initialize() {
    // Dragon
    assert(sem_init(&semSDragonSleep, 1, 0) == 0);
    assert(sem_init(&semSDragonEat, 1, 0) == 0);

    //Meal
    assert(sem_init(&semNMeal, 1, 0) == 0);
    assert(sem_init(&mtxNumMeal, 1, 1) == 0);

	// new for meal
	assert(sem_init(&semNMealSheep, 1, 0) == 0);
	assert(sem_init(&semNMealCow, 1, 0) == 0);
	assert(sem_init(&mtxNumMealSheep, 1, 1) == 0);
	assert(sem_init(&mtxNumMealCow, 1, 1) == 0);
	assert(sem_init(&semSMealDone, 1, 0) == 0);

    //Sheep
    assert(sem_init(&semNSheepInValley, 1, 0) == 0);
    assert(sem_init(&mtxNumSheepInValley, 1, 1) == 0);
    assert(sem_init(&semSSheepWaiting, 1, 0) == 0);
    assert(sem_init(&semNSheepWaiting, 1, 0) == 0);
    assert(sem_init(&mtxNumSheepWaiting, 1, 1) == 0);
    assert(sem_init(&semSSheepEaten, 1, 0) == 0);
    assert(sem_init(&mtxNumSheepEaten, 1, 1) == 0);
    //assert(sem_init(&semSSheepDie, 1, 0) == 0);

    //Cow
    assert(sem_init(&semNCowInValley, 1, 0) == 0);
    assert(sem_init(&mtxNumCowInValley, 1, 1) == 0);
    assert(sem_init(&semSCowWaiting, 1, 0) == 0);
    assert(sem_init(&semNCowWaiting, 1, 0) == 0);
    assert(sem_init(&mtxNumCowWaiting, 1, 1) == 0);
    assert(sem_init(&semSCowEaten, 1, 0) == 0);
    assert(sem_init(&mtxNumCowEaten, 1, 1) == 0);
    //assert(sem_init(&semSCowDie, 1, 0) == 0);

    //Termination Condition
    assert(sem_init(&mtxCondTerminate, 1, 1) == 0);
    printf("initialization finished\n");
}

int checkSheep() {
    sem_wait(&mtxNumSheepEaten);
    if (numSheepEaten >= MAX_SHEEP) {
		setTerminate();
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
    if (numCowEaten >= MAX_COW) {
		setTerminate();
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

void setTerminate() {
	sem_wait(&mtxCondTerminate);
	condTerminate = 1;
	sem_post(&mtxCondTerminate);

}

int main(void) {
    pthread_t tSmaug;
    pthread_t tSheep;
    pthread_t tCow;

    struct beastarg sheeparg;
    sheeparg.time = 0;
    sheeparg.no = 0;
    struct beastarg cowarg;
    cowarg.time = 0;
    cowarg.no = 0;

	long long elapseTime = 0;
	struct timeval lasttime;
    struct timeval curtime;
	gettimeofday(&lasttime, NULL);

	int sheepInterval = 0;
    int cowInterval = 0;
	int sheepGraze = 0;
	int cowGraze = 0;
	long long sheepTime = 0;
	long long cowTime = 0;

	printf("Input the maximum interval of spawning sheep:");
	scanf("%d", &sheepInterval);
	printf("Input the maximum grazing time of sheep:");
	scanf("%d", &sheepGraze);

	printf("Input the maximum interval of spawning cows:");
	scanf("%d", &cowInterval);
	printf("Input the maximum grazing time of cows:");
	scanf("%d", &cowGraze);

	sheepTime = sheepInterval;
	cowTime = cowInterval;

    int cowcount = 1;
    int sheepcount = 1;

    initialize();
    srand(time(NULL));
    pthread_create(&tSmaug, NULL, smaug, NULL);
    while (1) {
        if (checkTerminate()) {
			releaseResource();
			break;
		} 

		gettimeofday(&curtime, NULL);
		elapseTime+=(curtime.tv_sec-lasttime.tv_sec)*1000000 + (curtime.tv_usec - lasttime.tv_usec);
		lasttime = curtime;

		if(elapseTime > sheepTime) {
            sheeparg.time = rand() % sheepGraze;
            sheeparg.no ++;
			pthread_create(&tSheep, NULL, sheep, (void *)sheeparg);
			sheepTime += sheepInterval;
		}
		
		if(elapseTime > cowTime) {
            cowarg.time = rand() % cowGraze;
            cowarg.no ++;
			pthread_create(&tCow, NULL, cow, (void *)cowarg);
			cowTime += cowInterval;
		}
    }
    pthread_exit(NULL);
}
