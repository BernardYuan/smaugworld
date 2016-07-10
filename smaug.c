#include "smaug.h"

void eat() {
    semopChecked(semID, &WaitPNumMeal, 1);
    if (*numMeal > 0) {
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
    semopChecked(semID, &SignalPNumMeal, 1);
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
        eat();
        // the dragon is about to fall in sleep
        semopChecked(semID, &WaitPDragonWakeUp, 1);
        *DragonWakeUp = 0;
        semopChecked(semID, &SignalPDragonWakeUp, 1);
        printf("Smaug goes to sleep\n");
        semopChecked(semID, &WaitSDragonWakeUp, 1);
    }
}