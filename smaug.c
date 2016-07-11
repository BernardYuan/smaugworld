#include "smaug.h"

//the function which sets the termination
void setTerminate() {
    semopChecked(semID, &WaitPTermination, 1);
    *flagTermination = 1;
    semopChecked(semID, &SignalPTermination, 1);
}

int checkSheep() {
    semopChecked(semID, &WaitPSheepEaten, 1);
    if (*numSheepEaten >= MAX_SHEEP) {
        printf("Smaug has eaten more than maximum number of sheep, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPSheepEaten, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPSheepEaten, 1);
        return 0;
    }

}

int checkCow() {
    semopChecked(semID, &WaitPCowEaten, 1);
    if (*numCowEaten >= MAX_COW) {
        printf("Smaug has eaten more than the maximum number of cows, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPCowEaten, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPCowEaten, 1);
        return 0;
    }
}

int checkThief() {
    semopChecked(semID, &WaitPThiefLeave, 1);
    if (*numThiefLeave >= MAX_THIEF) {
        printf("Smaug has played with more than maximum number of thieves, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPThiefLeave, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPThiefLeave, 1);
        return 0;
    }
}

int checkHunter() {
    semopChecked(semID, &WaitPHunterLeave, 1);
    if (*numHunterLeave >= MAX_TREASUREHUNTER) {
        printf("Smaug has fought with more than maximum number of hunters, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPHunterLeave, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPHunterLeave, 1);
        return 0;
    }
}

int checkJewel() {
    semopChecked(semID, &WaitPDragonJewel, 1);
    if (*numDragonJewel >= MAX_JEWEL) {
        printf("Dragon has more than maximum number of jewels, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPDragonJewel, 1);
        return 1;
    }
    else if (*numDragonJewel <= MIN_JEWEL) {
        printf("Dragon has fewer than minimum number of jewels, the simulation will terminate\n");
        setTerminate();
        semopChecked(semID, &SignalPDragonJewel, 1);
        return 1;
    }
    else {
        semopChecked(semID, &SignalPDragonJewel, 1);
        return 0;
    }
}

void eat() {
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
    printf("smaug eats one more meal, %d meals left\n", *numMeal);
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

    semopChecked(semID, &WaitNHunterPath, 1);
    printf("Smaug is ready to interact with a hunter waiting in the path");
    semopChecked(semID, &SignalSHunterCave, 1);


    semopChecked(semID, &WaitSDragonFight, 1);
    printf("Smaug fights with the hunter\n");
    int fightrand = random();
    if (fightrand % 2 == 1) {
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

    semopChecked(semID, &WaitNThiefPath, 1);
    printf("Smaug is ready to interact with a thief waiting in the path");
    semopChecked(semID, &SignalSThiefCave, 1);

    semopChecked(semID, &WaitSThiefPlay, 1);
    printf("Smaug fights with the hunter\n");
    int fightrand = random();
    if (fightrand % 2 == 1) {
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
    pid_t localid = getpid();
    printf("ProcessID of smaug:%d\n", localid);
    printf("Smaug is sleeping\n");

    semopChecked(semID, &WaitPDragonWakeUp, 1);
    *DragonWakeUp = 0;
    semopChecked(semID, &SignalPDragonWakeUp, 1);

    semopChecked(semID, &WaitSDragonWakeUp, 1);
    int time = 0;
    while (1) {  //sleeping/waking loop
        printf("Smaug is awake\n");


        while (1) {
            semopChecked(semID, &WaitPNumMeal, 1);
            //check whether there is meal
            if (*numMeal > 0) {
                printf("smaug finds one meal\n");
                eat();
                semopChecked(semID, &SignalPNumMeal, 1);
                if (checkSheep() || checkCow()) {
                    break;
                }
                //check whether there is another meal
                semopChecked(semID, &WaitPNumMeal, 1);
                if (*numMeal > 0) {
                    printf("Smaug finds the second meal\n");
                    eat();
                    semopChecked(semID, &SignalPNumMeal, 1);
                    if (checkSheep() || checkCow()) {
                        break;
                    }
                    else {
                        swim();
                        continue;
                    }
                }
                else {
                    semopChecked(semID, &SignalPNumMeal, 1);
                    printf("There is no second meal\n");
                    swim();
                    continue;
                }
            }
            else {
                printf("No meals, smaug looks for thieves to play with\n");
                semopChecked(semID, &SignalPNumMeal, 1);
                semopChecked(semID, &WaitPThiefPath, 1);
                if (*numThiefPath > 0) {
                    printf("Smaug finds one thief\n");
                    play();
                    semopChecked(semID, &SignalPThiefPath, 1);
                    if (checkThief() || checkJewel()) {
                        break;
                    }
                    semopChecked(semID, &WaitPThiefPath, 1);
                    if (*numThiefPath > 0) {
                        printf("Smaug finds the second thief to play with");
                        play();
                        semopChecked(semID, &SignalPThiefPath, 1);
                        if (checkThief() || checkJewel()) {
                            break;
                        }
                        printf("Smaug plays with 2 thieves, and goes to sleep\n");
                    }
                    else {
                        semopChecked(semID, &SignalPThiefPath, 1);
                        printf("Smaug doesn't find the second thief to play with\n");
                        break;
                    }
                }
                else {
                    printf("No thieves smaug looks for treasure hunters to fight\n");
                    semopChecked(semID, &SignalPThiefPath, 1);

                    semopChecked(semID, &WaitPHunterPath, 1);
                    if (*numHunterPath > 0) {
                        fight();
                        semopChecked(semID, &SignalPHunterPath, 1);
                        if (checkHunter() || checkJewel()) {
                            break;
                        }
                        printf("Smaug fights a treasure hunter, and goes to swim\n");
                        swim();
                        continue;

                    }
                    else {
                        semopChecked(semID, &SignalPHunterPath, 1);
                        printf("No treasure hunter, smaug goes to sleep");
                        break;
                    }
                }

            }
        }

        if (checkTermination()) break;
        // the dragon is about to fall in sleep
        semopChecked(semID, &WaitPDragonWakeUp, 1);
        *DragonWakeUp = 0;
        semopChecked(semID, &SignalPDragonWakeUp, 1);
        printf("Smaug goes to sleep\n");
        semopChecked(semID, &WaitSDragonWakeUp, 1);
    }
    exit(0);
}
