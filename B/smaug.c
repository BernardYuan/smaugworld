//
// Created by Bernard Yuan on 2016-07-12.
//
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
    }
    pthread_exit();
}