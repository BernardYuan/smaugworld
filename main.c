#include<stdio.h>
#include<wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<curses.h>
#include<time.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/time.h>
#include<sys/resource.h>

// definitions of the semaphores
#define MTX_DRAGONWAKEUP 0

int semID;
union semun{
	int val;
	struct semid_ds *buf;
	ushort *array;
}seminfo;

struct timeval startTime;

struct sembuf WaitDragonWakeUp={MTX_DRAGONWAKEUP, -1, 0};
struct sembuf SignalDragonWakeUp={MTX_DRAGONWAKEUP, 1, 0};

void initialize();
void semctlChecked(int semID, int semNum, int flag, union semun seminfo); 
void semopChecked(int semID, struct sembuf *operation, unsigned num);

void initialize() {
	semID = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	seminfo.val = 0;
	semctlChecked(semID, MTX_DRAGONWAKEUP, SETVAL, seminfo);
}

void semctlChecked(int semID, int semNum, int flag, union semun seminfo) {
	if(semctl(semID, semNum, flag, seminfo)==-1) {
		if(errno != EIDRM) {
			printf("semaphore control failed: simulation terminating\n");
			printf("errno: %8d\n", errno);
			exit(2);
		}
		else {
			exit(3);
		}
	}
}

void semopChecked(int semID, struct sembuf *operation, unsigned num) {
	if(semop(semID, operation, num) == -1) {
		if(errno != EIDRM) {
			printf("semaphore operation failed: simulation terminating\n");
			printf("errno: %8d\n", errno);
			exit(2);
		}
		else {
			exit(3);
		}
	}
}

void smaug() {
	int time = 0;
	while(1) {
		printf("smaug is going to sleep\n");
		semopChecked(semID, &WaitDragonWakeUp, 1);
		printf("smaug wakes up and play for a while\n");
		time ++;
		usleep(1000000);
		if(time==3) exit(0);
	}
}
int main(void) {
	initialize();
	pid_t result = fork();

	if(result<0) {
		printf("fork error\n");
		exit(1);
	}
	if(result==0) {
		smaug();
	}
	else {	
		int i;
		for(i=0;i<5;i++) {
			usleep(2000000);
			semopChecked(semID, &SignalDragonWakeUp, 1);
		}
		int status;
		waitpid(-1,&status,0);
		printf("all processes has exited\n");
	}
}
