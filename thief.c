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
    printf("Thief %d enters the magic path, now %d thieves in path\n", lcoalpid, *numThiefPath);
    semopChecked(semID, &SignalPThiefPath, 1);

    semopChecked(semID, &WaitSThiefCave, 1);
    printf("Thief %d enters the cave\n", localpid);
    semopChecked(semID, &SignalSDragonPlay, 1);
    semopChecked(semID, &WaitSThiefPlay);
    printf("Thief finishes playings and leaves\n");
    semopChecked(semID, &SignalSThiefLeave, 1);
}