#include "smaug.h"

void eat() {
    semopChecked(semID, &WaitPNumMeal, 1);
    if (*numMeal > 0) {
        printf("Smaug finds a meal and eat it\n");
        semopChecked(semID, &WaitNMeal, 1);
        int i;
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &SignalSSheepWaiting, 1);
        }
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &SignalSCowWaiting, 1);
        }

        printf("Smaug waits to eat each sheep and cow in the meal\n");
        semopChecked(semID, &WaitSDragonEat, 1);
        printf("Smaug starts eating\n");

        int j;
        //deal with sheep
        for (j = 0; j < SHEEP_IN_MEAL; j++) {
            semopChecked(semID, &SignalSSheepEaten, 1);
            semopChecked(semID, &WaitSSheepDie, 1);
            semopChecked(semID, &WaitPSheepEaten, 1);
            *numSheepEaten = *numSheepEaten + 1;
            semopChecked(semID, &SignalPSheepEaten, 1);
            printf("Smaug eats one more Sheep, now %d sheep eaten\n", *numSheepEaten);
        }
        //deal with cows
        for (j = 0; j < COW_IN_MEAL; j++) {
            semopChecked(semID, &SignalSCowEaten, 1);
            semopChecked(semID, &WaitSCowDie, 1);
            semopChecked(semID, &WaitPCowEaten, 1);
            *numCowEaten = *numCowEaten + 1;
            semopChecked(semID, &SignalPCowEaten, 1);
            printf("Smaug eats one more Cow, now %d cows eaten\n", *numCowEaten);
        }

        *numMeal = *numMeal - 1;
        printf("smaug eats one meal, %d meals left\n", *numMeal);
    }
    else {
        printf("No meal lah~\n");
    }
    semopChecked(semID, &SignalPNumMeal, 1);
}
void swim() {
    printf("smaug starts swimming\n");
    if (usleep(1e6) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }
    printf("smaug finishes swimming\n");
}

void fight() {
    semopChecked(semID, &WaitPHunterPath, 1);
    if(*numHunterPath > 0) {
        semopChecked(semID, &WaitNHunterPath, 1);
        printf("Smaug is ready to interact with a hunter waiting in the path");
        semopChecked(semID, &SignalSHunterCave, 1);

    }
    semopChecked(semID, &SignalPHunterPath, 1);
    semopChecked(semID, &WaitSDragonFight, 1);
    printf("Smaug fights with the hunter\n");
    int fightrand = random();
    if(fightrand%2==1) {
        semopChecked(semID, &WaitPDragonJewel, 1);
        *numDragonJewel = *numDragonJewel + 5;
        printf("Smaug wins and get 5 jewels, now smaug has %d jewels\n", *numDragonJewel);
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    else {
        semopChecked(semID, &WaitPDragonJewel, 1);
        *numDragonJewel = *numDragonJewel - 10;
        printf("The hunter fights well and get rewarded with 10 jewels, now smaug has %d jewels\n", *numDragonJewel);
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    semopChecked(semID, &SignalSHunterFight, 1);
    semopChecked(semID, &WaitSHunterLeave, 1);
    semopChecked(semID, &WaitPHunterLeave, 1);
    *numHunterLeave = *numHunterLeave + 1;
    printf("Smaug has fought %d hunters\n", *numHunterLeave);
    semopChecked(semID, &SignalPHunterLeave, 1);
}

void play() {
    semopChecked(semID, &WaitPThiefPath, 1);
    if(*numThiefPath > 0) {
        semopChecked(semID, &WaitNThiefPath, 1);
        printf("Smaug is ready to interact with a thief waiting in the path");
        semopChecked(semID, &SignalSThiefCave, 1);
    }
    semopChecked(semID, &SignalPThiefPath, 1);
    semopChecked(semID, &WaitSThiefPlay, 1);
    printf("Smaug fights with the hunter\n");
    int fightrand = random();
    if(fightrand%2==1) {
        semopChecked(semID, &WaitPDragonJewel, 1);
        *numDragonJewel = *numDragonJewel + 20;
        printf("Smaug wins and get 20 jewels, now smaug has %d jewels\n", *numDragonJewel);
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    else {
        semopChecked(semID, &WaitPDragonJewel, 1);
        *numDragonJewel = *numDragonJewel - 8;
        printf("The thief wins and gets rewarded with 10 jewels, now smaug has %d jewels\n", *numDragonJewel);
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    semopChecked(semID, &SignalSThiefPlay, 1);
    semopChecked(semID, &WaitSThiefLeave, 1);
    semopChecked(semID, &WaitPThiefLeave, 1);
    *numThiefLeave = *numThiefLeave + 1;
    printf("Smaug has played with %d thieves\n", *numThiefLeave);
    semopChecked(semID, &SignalPThiefLeave, 1);
}

void smaug() {
    pid_t localid;
    localid = getpid();
    printf("ProcessID of smaug:%d\n", localid);
    printf("Smaug is sleeping\n");

    semopChecked(semID, &WaitPDragonWakeUp, 1);
    *DragonWakeUp = 0;
    semopChecked(semID, &SignalPDragonWakeUp, 1);

    semopChecked(semID, &WaitSDragonWakeUp, 1);
    int time = 0;
    while (1) {
        printf("Smaug is awake\n");


        // the dragon is about to fall in sleep
        semopChecked(semID, &WaitPDragonWakeUp, 1);
        *DragonWakeUp = 0;
        semopChecked(semID, &SignalPDragonWakeUp, 1);
        printf("Smaug goes to sleep\n");
        semopChecked(semID, &WaitSDragonWakeUp, 1);
    }
}