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
    printf("Hunter %d enters the magic path, now %d hunters in path\n", lcoalpid, *numHunterPath);
    semopChecked(semID, &SignalPHunterPath, 1);

    semopChecked(semID, &WaitSHunterCave, 1);
    printf("Hunter %d enters the cave\n", localpid);
    semopChecked(semID, &SignalSDragonFight, 1);
    semopChecked(semID, &WaitSHunterFight);
    printf("Hunter finishes fighting and leaves\n");
    semopChecked(semID, &SignalSHunterLeave, 1);

}