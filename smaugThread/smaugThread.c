#include "smaugThread.h"

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

void releaseResource() {
	printf("Release Sempahores\n");
    // Dragon
    assert(sem_destroy(&semSDragonSleep) == 0);
    assert(sem_destroy(&semSDragonEat) == 0);

    //Meal
    assert(sem_destroy(&semNMeal) == 0);
    assert(sem_destroy(&mtxNumMeal) == 0);

    // new for meal
    assert(sem_destroy(&semNMealSheep) == 0);
    assert(sem_destroy(&semNMealCow) == 0);
    assert(sem_destroy(&mtxNumMealSheep) == 0);
    assert(sem_destroy(&mtxNumMealCow) == 0);
    assert(sem_destroy(&semSMealDone) == 0);

    //Sheep
    assert(sem_destroy(&semNSheepInValley) == 0);
    assert(sem_destroy(&mtxNumSheepInValley) == 0);
    assert(sem_destroy(&semSSheepWaiting) == 0);
    assert(sem_destroy(&semNSheepWaiting) == 0);
    assert(sem_destroy(&mtxNumSheepWaiting) == 0);
    assert(sem_destroy(&semSSheepEaten) == 0);
    assert(sem_destroy(&mtxNumSheepEaten) == 0);

    //Cow
    assert(sem_destroy(&semNCowInValley) == 0);
    assert(sem_destroy(&mtxNumCowInValley) == 0);
    assert(sem_destroy(&semSCowWaiting) == 0);
    assert(sem_destroy(&semNCowWaiting) == 0);
    assert(sem_destroy(&mtxNumCowWaiting) == 0);
    assert(sem_destroy(&semSCowEaten) == 0);
    assert(sem_destroy(&mtxNumCowEaten) == 0);

    //Termination Condition
    assert(sem_destroy(&mtxCondTerminate) == 0);

    printf("All semaphores released\n");
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

//============================================
//SHEEP
//============================================
void *sheep(void* arg) {
    int time = ((struct beastarg *)arg)->time;
    int id = ((struct beastarg *)arg)->no;

    printf("SHEEPSHEEPSHEEP      Sheep[%d] is grazing for %d usec\n", id, time);
    usleep(time);

    // get the control of two shared variables
    // !!! always keep in this order, to avoid deadlock
    sem_wait(&mtxNumSheepInValley);
    sem_wait(&mtxNumCowInValley);
    numSheepInValley += 1;
    sem_post(&semNSheepInValley);
    printf("SHEEPSHEEPSHEEP      Sheep[%d] is enchanted, Now %d sheep and %d cows in valley\n", id, numSheepInValley, numCowInValley);
    
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

        sem_post(&mtxNumMeal);
        sem_post(&semSDragonSleep);
    }
    //release two shared variables
    sem_post(&mtxNumCowInValley);
    sem_post(&mtxNumSheepInValley);

    sem_wait(&semSSheepWaiting);
    printf("SHEEPSHEEPSHEEP      Sheep[%d] will be eaten\n", id);

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

        sem_post(&semSDragonEat);
    }
    sem_post(&mtxNumCowWaiting);
    sem_post(&mtxNumSheepWaiting);

    sem_wait(&semSSheepEaten);
    
    sem_wait(&mtxNumSheepEaten);
    numSheepEaten += 1;
    printf("SHEEPSHEEPSHEEP      Sheep[%d] is eaten, now %d sheep eaten\n", id, numSheepEaten);
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
    //printf("The last sheep %d dies and the snack is done\n", id);
    sem_post(&semSMealDone);
    pthread_exit(NULL);
}

//====================================================
//Smaug
//====================================================
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


//=============================================
//Cow
//=============================================
void *cow(void* arg) {
    int time = ((struct beastarg*)arg)->time;
    int id = ((struct beastarg*)arg)->no;

    printf("COWCOWCOWCOWCOWCOW      Cow[%d] is grazing for %d usec\n", id, time);
    usleep(time);

    // get the control of two shared variables
    // !!! always keep in this order, to avoid deadlock
    sem_wait(&mtxNumSheepInValley);
    sem_wait(&mtxNumCowInValley);
    numCowInValley += 1;
    sem_post(&semNCowInValley);
    printf("COWCOWCOWCOWCOWCOW      Cow[%d] is enchanted, Now there is %d sheep and %d cows in Valley\n", id, numSheepInValley, numCowInValley);

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
        printf("COWCOWCOWCOWCOWCOW      Cow[%d] results in a new meal, now there are %d meals\n", id, numMeal);
        sem_post(&mtxNumMeal);
        sem_post(&semSDragonSleep);
    }
    //release two shared variables
    sem_post(&mtxNumCowInValley);
    sem_post(&mtxNumSheepInValley);

    sem_wait(&semSCowWaiting);
    printf("COWCOWCOWCOWCOWCOW      Cow[%d] will be eaten\n", id);

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
        sem_post(&semSDragonEat);
    }
    sem_post(&mtxNumCowWaiting);
    sem_post(&mtxNumSheepWaiting);

    sem_wait(&semSCowEaten);

    sem_wait(&mtxNumCowEaten);
    numCowEaten += 1;
    printf("COWCOWCOWCOWCOWCOW      Cow[%d] is eaten, now %d cows eaten\n", id, numCowEaten);
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
    //printf("The last Cow %d dies and the snack is done\n", id);
    sem_post(&semSMealDone);
    pthread_exit(NULL);
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
            struct beastarg temp = sheeparg;
			pthread_create(&tSheep, NULL, sheep, (void *)(&temp));
			sheepTime += sheepInterval;
		}
		
		if(elapseTime > cowTime) {
            cowarg.time = rand() % cowGraze;
            cowarg.no ++;
            struct beastarg temp = cowarg;
			pthread_create(&tCow, NULL, cow, (void *)(&temp));
			cowTime += cowInterval;
		}
    }
	exit(0);
    //pthread_exit(NULL);
}
