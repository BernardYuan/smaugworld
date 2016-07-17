#include "smaug.h"
//smaug eats a meal
int newWakeup = 1;

void eat() {
    semopChecked(semID, &WaitNMeal, 1); //guarantee that there is really a meal
    int i;
    // let the sheep and cows be ready to be eaten
    for (i = 0; i < SHEEP_IN_MEAL; i++) {
        semopChecked(semID, &SignalSSheepWaiting, 1);
    }
    for (i = 0; i < COW_IN_MEAL; i++) {
        semopChecked(semID, &SignalSCowWaiting, 1);
    }

    //printf("Smaug waits to eat each sheep and cow in the meal\n");
    semopChecked(semID, &WaitSDragonEat, 1);
    printf("DRAGONDRAGONDRAGON      Smaug starts eating\n");
    int j;
    //deal with sheep
    for (j = 0; j < SHEEP_IN_MEAL; j++) {
        semopChecked(semID, &SignalSSheepEaten, 1);
    }
    //deal with cows
    for (j = 0; j < COW_IN_MEAL; j++) {
        semopChecked(semID, &SignalSCowEaten, 1);
    }

    //waiting the meal to be done
    semopChecked(semID, &WaitSMealDone, 1);
    printf("DRAGONDRAGONDRAGON      Smaug finishes eating\n");
    //decrement the number of meals
    *numMeal = *numMeal - 1;
    semopChecked(semID, &WaitPEatenMeal, 1);
    *numEatenMeal = *numEatenMeal + 1;
    printf("DRAGONDRAGONDRAGON      Smaug eats one more snack,%d snacks eaten, %d snacks left\n", *numEatenMeal, *numMeal);
    semopChecked(semID, &SignalPEatenMeal, 1);

}
//smaug swims
void swim() {
    printf("DRAGONDRAGONDRAGON      Smaug starts swimming\n");
    if (usleep(1e6) == -1) {
        /* exit when usleep interrupted by kill signal */
        if (errno == EINTR)exit(4);
    }
    printf("DRAGONDRAGONDRAGON      Smaug finishes swimming\n");
}

//smaug fights the treasure hunter
void fight() {

    semopChecked(semID, &WaitNHunterPath, 1);
    *numHunterPath = *numHunterPath - 1;
    printf("DRAGONDRAGONDRAGON      Smaug is ready to interact with a hunter waiting in the path");
    semopChecked(semID, &SignalSHunterCave, 1);

    semopChecked(semID, &WaitSDragonFight, 1);
    printf("DRAGONDRAGONDRAGON      Smaug starts fighting with the hunter\n");
    int fightrand = (double)rand()/RAND_MAX;
    if (fightrand > THIEF_WIN) {
        semopChecked(semID, &WaitPDragonJewel, 1);
        *numDragonJewel = *numDragonJewel + 5;
        printf("DRAGONDRAGONDRAGON      Smaug wins and gets 5 jewels, now smaug has %d jewels\n", *numDragonJewel);
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    else {
        semopChecked(semID, &WaitPDragonJewel, 1);

        printf("DRAGONDRAGONDRAGON      The hunter fights well and will get rewarded with 10 jewels\n");
	if(*numDragonJewel >= 10 ) {
		*numDragonJewel = *numDragonJewel - 10; 
		printf("DRAGONDRAGONDRAGON      Smaug gives 10 jewels, now it has %d jewels\n", *numDragonJewel);
	}
        else {
		printf("DRAGONDRAGONDRAGON      Smaug does not have enough jewels, the simulation will terminate\n", *numDragonJewel);
        setTerminate();
		terminateSimulation();
		
	}
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    if(checkJewel()) {
        terminateSimulation();
    }
    semopChecked(semID, &SignalSHunterFight, 1);
}

//smaug plays with the thief
void play() {

    semopChecked(semID, &WaitNThiefPath, 1);
    *numThiefPath = *numThiefPath - 1;
    printf("DRAGONDRAGONDRAGON      Smaug is ready to interact with a thief waiting in the path");
    semopChecked(semID, &SignalSThiefCave, 1);

    semopChecked(semID, &WaitSDragonPlay, 1);
    printf("DRAGONDRAGONDRAGON      Smaug fights with the hunter\n");
    int fightrand = (double)rand()/RAND_MAX;
    if (fightrand > HUNTER_WIN) {
        semopChecked(semID, &WaitPDragonJewel, 1);
        *numDragonJewel = *numDragonJewel + 20;
        printf("DRAGONDRAGONDRAGON      Smaug wins and gets 20 jewels, now smaug has %d jewels\n", *numDragonJewel);
        semopChecked(semID, &SignalPDragonJewel, 1);
    }
    else {
        semopChecked(semID, &WaitPDragonJewel, 1);
        printf("DRAGONDRAGONDRAGON      Smaug loses and loses 8 jewels\n");
	if(*numDragonJewel >= 8) {
		*numDragonJewel = *numDragonJewel - 8;
		printf("DRAGONDRAGONDRAGON      Smaug loses 8 jewels, now it has %d jewels\n", *numDragonJewel);
	}
	else {
		printf("DRAGONDRAGONDRAGON      Smaug has only %d jewels, now simulation terminates\n", *numDragonJewel);
		setTerminate();
        terminateSimulation();
	}
	semopChecked(semID, &SignalPDragonJewel, 1);
    }
    if(checkJewel()) {
        terminateSimulation();
    }

    semopChecked(semID, &SignalSThiefPlay, 1);
}

void smaug() {
    pid_t localid = getpid();
    smaugID = localid;
    printf("DRAGONDRAGONDRAGON      Smaug[%d] is born\n", smaugID);
    setpgid(smaugID, dragonGID);
    printf("DRAGONDRAGONDRAGON      Smaug[%d] goes to sleep\n", smaugID);

    semopChecked(semID, &WaitPDragonJewel, 1);
    *numDragonJewel = INIT_JEWEL;
    semopChecked(semID, &SignalPDragonJewel, 1);

    semopChecked(semID, &WaitSDragonWakeUp, 1);
    newWakeup = 1;
    int time = 0;
    while (1) {  //sleeping/waking loop
        printf("DRAGONDRAGONDRAGON      Smaug[%d] wakes up\n", smaugID);
        while (1) {  //swimming loop
            // the meal, thief, hunter process in this loop
            int onceMeal = 0;
            int onceThief = 0;
            int onceHunter = 0;

            //looking for meals
            semopChecked(semID, &WaitPNumMeal, 1);
            while(*numMeal > 0 && onceMeal < MEAL_ONCE) {
                if(newWakeup==1) newWakeup = 0;
                else semopChecked(semID, &WaitSDragonWakeUp, 1);

                printf("DRAGONDRAGONDRAGON      Smaug[%d] finds the %d-th snack\n", smaugID, onceMeal+1);
                eat();
                onceMeal ++;
            }
            semopChecked(semID, &SignalPNumMeal, 1);

            //check meal this round
            if(onceMeal > 0) {  //if the
                swim();
                continue;
            }
            else printf("DRAGONDRAGONDRAGON      Smaug[%d] finds no snacks, it looks for thieves\n", smaugID);

            //looking for thieves
            semopChecked(semID, &WaitPThiefPath, 1);
            while(*numThiefPath > 0 && onceThief < THIEF_ONCE) {
                if(newWakeup==1) newWakeup = 0;
                else semopChecked(semID, &WaitSDragonWakeUp, 1);

                printf("DRAGONDRAGONDRAGON      Smaug[%d] finds the %d-th thief\n", smaugID, onceThief+1);
                play();
                onceThief ++;
            }
            semopChecked(semID, &SignalPThiefPath, 1);

            //check thief this round
            if(onceThief > 0) {
                printf("DRAGONDRAGONDRAGON      Smaug[%d] is a happy dragon now, it curls up its jewels and goes to sleep\n", smaugID);
                break;
            }
            else printf("DRAGONDRAGONDRAGON      Smaug[%d] finds no thieves, it looks for hunters\n", smaugID);


            //looking for hunters
            semopChecked(semID, &WaitPHunterPath, 1);
            while(*numHunterPath > 0 && onceHunter < HUNTER_ONCE) {
                if(newWakeup==1) newWakeup = 0;
                else semopChecked(semID, &WaitSDragonWakeUp, 1);

                printf("DRAGONDRAGONDRAGON      Smaug[%d] finds the %d-th hunter\n", smaugID, onceHunter + 1);
                fight();
                onceHunter++;
            }
            semopChecked(semID, &SignalPHunterPath, 1);

            //check hunter
            if(onceHunter > 0) {
                printf("DRAGONDRAGONDRAGON      Smaug[%d] fought with the treasure hunter and goes to swim\n", smaugID);
                swim();
                continue;
            }
            else {
                printf("DRAGONDRAGONDRAGON      Smaug[%d] finds no hunters, there is nothing left, smaug goes to sleep\n", smaugID);
                break;
            }
        }

        printf("DRAGONDRAGONDRAGON      Smaug[%d] goes to sleep\n", smaugID);
        semopChecked(semID, &WaitSDragonWakeUp, 1);
        newWakeup = 1;
    }
}
