#include "cow.h"

void cow(int time) {
    pid_t localpid = getpid();
    setpgid(localpid, beastGID);
    printf("cow %d is grazing for %d usec\n", localpid, time);
    //grazing
    if (usleep(time) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR) exit(4);
    }
    printf("cow %d is enchanted\n", localpid);
    //the cow is enchanted
    //keep in this order in all files to avoid deadlock
    semopChecked(semID, &WaitPSheepInValley, 1); //use the number of sheep in valley
    semopChecked(semID, &WaitPCowInValley, 1);   //use the number of cows in valley
    *CowInValley = *CowInValley + 1;
    semopChecked(semID, &SignalNCowInValley, 1);
    printf("One more cow enters the valley, now %d cows and %d sheep\n", *CowInValley, *SheepInValley);
    // There is a meal in the Valley
    if (*CowInValley >= COW_IN_MEAL && *SheepInValley >= SHEEP_IN_MEAL) {
        int i;
        // decrement the counters only when the semaphores are decremented
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowInValley, 1);
            *CowInValley = *CowInValley - 1;
        }
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &WaitNSheepInValley, 1);
            *SheepInValley = *SheepInValley - 1;
        }

        //increment the number of meals
        semopChecked(semID, &WaitPNumMeal, 1);
        semopChecked(semID, &SignalNMeal, 1);
        *numMeal = *numMeal + 1;
        printf("A new meal is added, now number of meals:%d\n", *numMeal);
        semopChecked(semID, &SignalPNumMeal, 1);
        //release the use of shared variables
        semopChecked(semID, &SignalPCowInValley, 1);
        semopChecked(semID, &SignalPSheepInValley, 1);

        //send a wakeup to dragon
        semopChecked(semID, &SignalSDragonWakeUp, 1);
    }
    else { //the cow in the Valley is not enough for a meal
        semopChecked(semID, &SignalPCowInValley, 1);
        semopChecked(semID, &SignalPSheepInValley, 1);
    }
//    printf("Cow %d waiting in the snack\n", localpid);
    semopChecked(semID, &WaitSCowWaiting, 1);

    semopChecked(semID, &SignalNCowToEat, 1);

    //Wait in this order to avoid deadlock
    semopChecked(semID, &WaitPSheepToEat, 1);
    semopChecked(semID, &WaitPCowToEat, 1);
    *numCowToEat = *numCowToEat + 1;
    if (*numCowToEat >= COW_IN_MEAL && *numSheepToEat >= SHEEP_IN_MEAL) {
        int i;
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowToEat, 1);
            *numCowToEat = *numCowToEat - 1;
        }
        for (i = 0; i < SHEEP_IN_MEAL; i++) {
            semopChecked(semID, &WaitNSheepToEat, 1);
            *numSheepToEat = *numSheepToEat - 1;
        }
        printf("The last cow %d in a meal ready to be eaten signals smaug to eat\n", localpid);
        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalPSheepToEat, 1);
        semopChecked(semID, &SignalSDragonEat, 1);
    }
    else {
        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalPSheepToEat, 1);
    }

    // it waits to be eaten
    semopChecked(semID, &WaitSCowEaten, 1);

    semopChecked(semID, &WaitPCowEaten, 1);
    *numCowEaten = *numCowEaten + 1;
    semopChecked(semID, &SignalPCowEaten, 1);

    printf("Cow %d is eaten, now %d cows eaten\n", localpid, *numCowEaten);
    if(checkCow()) {
        terminateSimulation();
        exit(0);

    }
    semopChecked(semID, &SignalNMealCow, 1);

    //signal smaug that the meal is done only when all beasts are eaten
    //keep in this order to avoid deadlock
    semopChecked(semID, &WaitPMealSheep, 1);
    semopChecked(semID, &WaitPMealCow, 1);
    *numMealCow = *numMealCow + 1;
    if(*numMealSheep >= SHEEP_IN_MEAL && *numMealCow >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < SHEEP_IN_MEAL ; i++) {
            semopChecked(semID, &WaitNMealSheep, 1);
            *numMealSheep = *numMealSheep - 1;
        }

        for (i = 0; i < COW_IN_MEAL ; i++) {
            semopChecked(semID, &WaitNMealCow, 1);
            *numMealCow = *numMealCow - 1;
        }

        printf("The last Cow in the snack is eaten, smaug finishes the snack\n");
        semopChecked(semID, &SignalPMealCow, 1);
        semopChecked(semID, &SignalPMealSheep, 1);
        semopChecked(semID, &SignalSMealDone, 1);
    }
    else {
        semopChecked(semID, &SignalPMealCow, 1);
        semopChecked(semID, &SignalPMealSheep, 1);
    }
}