//
// Created by Bernard Yuan on 2016-07-10.
//
#include "hunter.h"

void hunter() {
    pid_t localpid = getpid();
    printf("Hunter %d is looking for the path\n", localpid);
    int pathtime = random();
    if (usleep(pathtime) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }

    semopChecked(semID, &SignalNHunterPath, 1);
    semopChecked(semID, &WaitPHunterPath, 1);
    *numHunterPath = *numHunterPath + 1;
    semopChecked(semID, &SignalPHunterPath, 1);

    printf("Hunter %d enters the magic path, now %d hunters in path\n", localpid, *numHunterPath);
    semopChecked(semID, &SignalSDragonWakeUp, 1);

    semopChecked(semID, &WaitPDragonWakeUp, 1);
    if(*DragonWakeUp == 0) {
        *DragonWakeUp = 1; //assign 1 means, there has already been a snack or a visitor waking up smaug
        semopChecked(semID, &SignalSDragonWakeUp, 1);
        printf("The hunter %d wakes up the dragon\n", localpid);
    }
    semopChecked(semID, &SignalPDragonWakeUp, 1);

    semopChecked(semID, &WaitSHunterCave, 1);
    printf("Hunter %d enters the cave\n", localpid);
    semopChecked(semID, &SignalSDragonFight, 1);
    semopChecked(semID, &WaitSHunterFight, 1);
    printf("Hunter finishes fighting and leaves\n");
    semopChecked(semID, &SignalSHunterLeave, 1);

    semopChecked(semID, &WaitPHunterLeave, 1);
    *numHunterLeave = *numHunterLeave + 1;
    printf("Smaug has fought %d hunters\n", *numHunterLeave);
    semopChecked(semID, &SignalPHunterLeave, 1);

    if(checkHunter()) {
        terminateSimulation();
    }
    exit(0);
}
