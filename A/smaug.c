#include "smaug.h"
int newWakeup = 0;
void eat() {
    semopChecked(semID, &WaitNMeal, 1); //guarantee that there is really a meal
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
    }
    //deal with cows
    for (j = 0; j < COW_IN_MEAL; j++) {
        semopChecked(semID, &SignalSCowEaten, 1);
    }
    semopChecked(semID, &WaitSMealDone, 1);
    *numMeal = *numMeal - 1;
    semopChecked(semID, &WaitPEatenMeal, 1);
    *numEatenMeal = *numEatenMeal + 1;
    printf("smaug eats one more meal,%d meals eaten, %d meals left\n", *numEatenMeal, *numMeal);
    semopChecked(semID, &SignalPEatenMeal, 1);

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
        while (1) {  //swimming loop
            semopChecked(semID, &WaitPNumMeal, 1);
            //check whether there is meal
            if (*numMeal > 0) {

                if(newWakeup==0) newWakeup = 1;
                else semopChecked(semID, &WaitSDragonWakeUp, 1);

                printf("smaug finds one meal\n");
                eat();
                semopChecked(semID, &SignalPNumMeal, 1);
                //check whether there is another meal
                semopChecked(semID, &WaitPNumMeal, 1);
                if (*numMeal > 0) {
                    printf("Smaug finds the second meal\n");
                    eat();
                    semopChecked(semID, &SignalPNumMeal, 1);
                    swim();
                    continue;
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

                    if(newWakeup==0) newWakeup = 1;
                    else semopChecked(semID, &WaitSDragonWakeUp, 1);

                    printf("Smaug finds one thief\n");
                    play();
                    semopChecked(semID, &SignalPThiefPath, 1);
                    if (checkJewel()) {
                        terminateSimulation();
                    }
                    semopChecked(semID, &WaitPThiefPath, 1);
                    if (*numThiefPath > 0) {
                        printf("Smaug finds the second thief to play with");
                        play();
                        semopChecked(semID, &SignalPThiefPath, 1);
                        if (checkJewel()) {
                            terminateSimulation();
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

                        if(newWakeup==0) newWakeup = 1;
                        else semopChecked(semID, &WaitSDragonWakeUp, 1);

                        fight();
                        semopChecked(semID, &SignalPHunterPath, 1);
                        if (checkJewel()) {
                            terminateSimulation();
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
        newWakeup = 0;
    }
    exit(0);
}
