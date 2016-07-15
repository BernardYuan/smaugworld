#include "sheep.h"

void sheep(int time) {
    pid_t localpid = getpid();
    setpgid(localpid, beastGID);
    printf("sheep %d is grazing for %d usec\n", localpid, time);
    //grazing
    if (usleep(time) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }
    printf("sheep %d is enchanted\n", localpid);
    //the sheep is enchanted

    //keep in this order to get use
    semopChecked(semID, &WaitPSheepInValley, 1);
    semopChecked(semID, &WaitPCowInValley, 1);
    *SheepInValley = *SheepInValley + 1;
    semopChecked(semID, &SignalNSheepInValley, 1);
    printf("One more sheep enters the valley, now %d cows and %d sheep\n", *CowInValley, *SheepInValley);
    // There is a meal in the Valley
    if (*SheepInValley >= SHEEP_IN_MEAL && *CowInValley >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &WaitNSheepInValley, 1);
            *SheepInValley = *SheepInValley - 1;
        }
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowInValley, 1);
            *CowInValley = *CowInValley - 1;
        }

        semopChecked(semID, &WaitPNumMeal, 1);
        *numMeal = *numMeal + 1;
        printf("A new meal is added, now number of meals:%d\n", *numMeal);
        semopChecked(semID, &SignalNMeal, 1);
        semopChecked(semID, &SignalPNumMeal, 1);

        semopChecked(semID, &SignalPCowInValley, 1);
        semopChecked(semID, &SignalPSheepInValley, 1);

        //send a wake up to the dragon
        semopChecked(semID, &SignalSDragonWakeUp, 1);
    }
    else { //the sheep in the Valley is not enough for a meal
        semopChecked(semID, &SignalPCowInValley, 1);
        semopChecked(semID, &SignalPSheepInValley, 1);
    }

    // sheep waiting
    semopChecked(semID, &WaitSSheepWaiting, 1);

    semopChecked(semID, &SignalNSheepToEat, 1);

    // smaug starts eating only when the the meal is ready
    semopChecked(semID, &WaitPSheepToEat, 1);
    semopChecked(semID, &WaitPCowToEat, 1);
    *numSheepToEat = *numSheepToEat + 1;
    if (*numSheepToEat >= SHEEP_IN_MEAL && *numCowToEat >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &WaitNSheepToEat, 1);
            *numSheepToEat = *numSheepToEat - 1;
        }
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowToEat, 1);
            *numCowToEat = *numCowToEat - 1;
        }

        printf("The last sheep %d in a meal ready to be eaten signals smaug to eat\n", localpid);
        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalPSheepToEat, 1);
        semopChecked(semID, &SignalSDragonEat, 1);
    }
    else {
        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalPSheepToEat, 1);
    }
//    printf("Sheep %d is about to be eaten\n", localpid);
    semopChecked(semID, &WaitSSheepEaten, 1);

    semopChecked(semID, &WaitPSheepEaten, 1);
    *numSheepEaten = *numSheepEaten + 1;
    semopChecked(semID, &SignalPSheepEaten, 1);
    printf("Sheep %d is eaten, now %d sheep eaten\n", localpid, *numSheepEaten);
    if(checkSheep()) {
        terminateSimulation();
        exit(0);
    }

    semopChecked(semID, &SignalNMealSheep, 1);
    //keep in this order to avoid deadlock
    semopChecked(semID, &WaitPMealSheep, 1);
    semopChecked(semID, &WaitPMealCow, 1);
    *numMealSheep = *numMealSheep + 1;
    if(*numMealSheep >= SHEEP_IN_MEAL && *numMealCow >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL ; i++) {
            semopChecked(semID, &WaitNMealSheep, 1);
            *numMealSheep = *numMealSheep - 1;
        }

        for (i = 0; i < COW_IN_MEAL ; i--) {
            semopChecked(semID, &WaitNMealCow, 1);
            *numMealCow = *numMealCow - 1;
        }

        printf("The last sheep in the snack is eaten, smaug finishes the snack\n");
        semopChecked(semID, &SignalPMealCow, 1);
        semopChecked(semID, &SignalPMealSheep, 1);
        semopChecked(semID, &SignalSMealDone, 1);
    }
    else {
        semopChecked(semID, &SignalPMealCow, 1);
        semopChecked(semID, &SignalPMealSheep, 1);
    }
}

