//
// Created by Bernard Yuan on 2016-07-10.
//
#include "thief.h"
void thief() {
    pid_t localpid = getpid();
    printf("Thief %d is looking for the path\n", localpid);
    int pathtime = random();
    if (usleep(pathtime) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }

    semopChecked(semID, &SignalNThiefPath, 1);
    semopChecked(semID, &WaitPThiefPath, 1);
    *numThiefPath = *numThiefPath + 1;
    semopChecked(semID, &SignalPThiefPath, 1);

    printf("Thief %d enters the magic path, now %d thieves in path\n", localpid, *numThiefPath);
    semopChecked(semID, &SignalSDragonWakeUp, 1);



    semopChecked(semID, &WaitPDragonWakeUp, 1);
    if(*DragonWakeUp == 0) {
        *DragonWakeUp = 1; //assign 1 means the dragon is waken up once now
        semopChecked(semID, &SignalSDragonWakeUp, 1);
        printf("The thief %d wakes up the dragon\n", localpid);
    }
    semopChecked(semID, &SignalPDragonWakeUp, 1);

    semopChecked(semID, &WaitSThiefCave, 1);
    printf("Thief %d enters the cave\n", localpid);
    semopChecked(semID, &SignalSDragonPlay, 1);
    semopChecked(semID, &WaitSThiefPlay, 1);
    printf("Thief finishes playings and leaves\n");

    semopChecked(semID, &WaitPThiefLeave, 1);
    *numThiefLeave = *numThiefLeave + 1;
    printf("Smaug has played with %d thieves\n", *numThiefLeave);
    semopChecked(semID, &SignalPThiefLeave, 1);
    if(checkThief()) {
        terminateSimulation();
    }
    exit(0);
}
