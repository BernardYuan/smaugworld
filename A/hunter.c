//
// Created by Bernard Yuan on 2016-07-10.
//
#include "hunter.h"

void hunter(int time) {
    pid_t localpid = getpid();
    setpgid(localpid, hunterGID);
    printf("HUNTERHUNTERHUNTER      Hunter[%d] is looking for the path\n", localpid);
    int pathtime = random();
    if (usleep(time) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }

    semopChecked(semID, &SignalNHunterPath, 1);
    semopChecked(semID, &WaitPHunterPath, 1);
    *numHunterPath = *numHunterPath + 1;
    semopChecked(semID, &SignalPHunterPath, 1);

    printf("HUNTERHUNTERHUNTER      Hunter[%d] enters the magic path, now %d hunters in path\n", localpid, *numHunterPath);
    semopChecked(semID, &SignalSDragonWakeUp, 1);

    semopChecked(semID, &WaitSHunterCave, 1);
    printf("HUNTERHUNTERHUNTER      Hunter[%d] enters the cave\n", localpid);
    semopChecked(semID, &SignalSDragonFight, 1);
    semopChecked(semID, &WaitSHunterFight, 1);

    semopChecked(semID, &WaitPHunterLeave, 1);
    *numHunterLeave = *numHunterLeave + 1;
    printf("HUNTERHUNTERHUNTER      Hunter[%d] leaves, now Smaug has fought %d hunters\n", localpid, *numHunterLeave);
    semopChecked(semID, &SignalPHunterLeave, 1);

    if(checkHunter()) {
        terminateSimulation();
    }
    exit(0);
}
