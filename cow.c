#include "cow.h"

void cow(int time) {
    pid_t localpid = getpid();
    printf("cow %d is grazing for %d usec\n", localpid, time);
    //grazing
    if (usleep(time) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }
    printf("cow %d is enchanted\n", localpid);
    //the cow is enchanted
    semopChecked(semID, &WaitPCowInValley, 1);
    *CowInValley = *CowInValley + 1;
    semopChecked(semID, &SignalNCowInValley, 1);
    printf("Now %d cows in the valley\n", *CowInValley);
    // There is a meal in the Valley
    if (*CowInValley >= COW_IN_MEAL) {
        int i;
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
        printf("The last cow %d in the meal wakes up the dragon\n", localpid);
        semopChecked(semID, &SignalSDragonWakeUp, 1);
    }
    else { //the cow in the Valley is not enough for a meal
        semopChecked(semID, &SignalPCowInValley, 1);
    }

    printf("Cow %d waiting in the snack\n", localpid);
    semopChecked(semID, &WaitSCowWaiting, 1);

    semopChecked(semID, &SignalNCowToEat, 1);
    semopChecked(semID, &WaitPCowToEat, 1);
    *numCowToEat = *numCowToEat + 1;
    if (*numCowToEat >= COW_IN_MEAL) {
        int i;
        for (i = 0; i < COW_IN_MEAL; i++) {
            semopChecked(semID, &WaitNCowToEat, 1);
        }
        *numCowToEat = *numCowToEat - COW_IN_MEAL;
        printf("The last cow %d in a meal ready to be eaten signals smaug to eat\n", localpid);
        semopChecked(semID, &SignalPCowToEat, 1);
        semopChecked(semID, &SignalSDragonEat, 1);
    }
    else {
        semopChecked(semID, &SignalPCowToEat, 1);
    }
//    printf("Cow %d is about to be eaten\n", localpid);
    semopChecked(semID, &WaitSCowEaten, 1);
    printf("Cow %d is eaten\n", localpid);
    semopChecked(semID, &SignalSCowDie, 1);
}