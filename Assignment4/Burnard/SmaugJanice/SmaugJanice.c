#include <errno.h>
#include <wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

#define MAX_TREASUREHUNTER 4
#define MAX_THIEF 3
#define MAX_JEWEL 80
#define MIN_JEWEL 0
#define INIT_JEWEL 30
#define REWARD_HUNTER 10
#define REWARD_THIEF 8
#define BRIBE_HUNTER 5
#define BRIBE_THIEF 20
#define LENGTH_INTERVAL 5
#define MAX_ZOMBIE 10
#define THIEF_ONCE 1
#define HUNTER_ONCE 1

float winProb = 0.0f;

extern pid_t smaugID;
extern const pid_t dragonGID;
extern const pid_t thiefGID;
extern const pid_t hunterGID;

#define SEM_S_DRAGONWAKEUP   0
extern struct sembuf WaitSDragonWakeUp;
extern struct sembuf SignalSDragonWakeUp;
#define SEM_S_DRAGONEAT      1
extern struct sembuf WaitSDragonEat;
extern struct sembuf SignalSDragonEat;
#define SEM_P_DRAGONJEWEL    3
extern int numDragonJewelFlag;
extern int *numDragonJewel;
extern struct sembuf WaitPDragonJewel;
extern struct sembuf SignalPDragonJewel;
#define SEM_S_DRAGONFIGHT    4
extern struct sembuf WaitSDragonFight;
extern struct sembuf SignalSDragonFight;
#define SEM_S_DRAGONPLAY     5
extern struct sembuf WaitSDragonPlay;
extern struct sembuf SignalSDragonPlay;

#define SEM_N_HUNTERPATH     6
extern struct sembuf WaitNHunterPath;
extern struct sembuf SignalNHunterPath;
#define SEM_P_HUNTERPATH     7
extern int numHunterPathFlag;
extern int *numHunterPath;
extern struct sembuf WaitPHunterPath;
extern struct sembuf SignalPHunterPath;
#define SEM_S_HUNTERCAVE     8
extern struct sembuf WaitSHunterCave;
extern struct sembuf SignalSHunterCave;
#define SEM_S_HUNTERFIGHT    9
extern struct sembuf WaitSHunterFight;
extern struct sembuf SignalSHunterFight;
#define SEM_P_HUNTERLEAVE    10
extern int numHunterLeaveFlag;
extern int *numHunterLeave;
extern struct sembuf WaitPHunterLeave;
extern struct sembuf SignalPHunterLeave;

#define SEM_N_THIEFPATH      11
extern struct sembuf WaitNThiefPath;
extern struct sembuf SignalNThiefPath;
#define SEM_P_THIEFPATH      12
extern int numThiefPathFlag;
extern int *numThiefPath;
extern struct sembuf WaitPThiefPath;
extern struct sembuf SignalPThiefPath;
#define SEM_S_THIEFCAVE      13
extern struct sembuf WaitSThiefCave;
extern struct sembuf SignalSThiefCave;
#define SEM_S_THIEFPLAY      14
extern struct sembuf WaitSThiefPlay;
extern struct sembuf SignalSThiefPlay;
#define SEM_P_THIEFLEAVE     15
extern int numThiefLeaveFlag;
extern int *numThiefLeave;
extern struct sembuf WaitPThiefLeave;
extern struct sembuf SignalPThiefLeave;

#define SEM_P_TERMINATION    16
extern int flagTerminationFlag;
extern int *flagTermination;
extern struct sembuf WaitPTermination;
extern struct sembuf SignalPTermination;

extern int semID;
extern union semun seminfo;

int semID;

union semun {

    int val;

    struct semid_ds *buf;

    unsigned short *array;

} seminfo;

struct timeval startTime;

pid_t smaugID;
const pid_t dragonGID = 660;
const pid_t thiefGID = 661;
const pid_t hunterGID = 662;

int numDragonJewelFlag = 0;
int *numDragonJewel = NULL;
int numHunterPathFlag = 0;
int *numHunterPath = NULL;
int numHunterLeaveFlag = 0;
int *numHunterLeave = NULL;
int numThiefPathFlag = 0;
int *numThiefPath = NULL;
int numThiefLeaveFlag = 0;
int *numThiefLeave = NULL;
int flagTerminationFlag = 0;
int *flagTermination = NULL;

struct sembuf WaitSDragonWakeUp = {SEM_S_DRAGONWAKEUP, -1, 0};
struct sembuf SignalSDragonWakeUp = {SEM_S_DRAGONWAKEUP, 1, 0};
struct sembuf WaitPDragonJewel = {SEM_P_DRAGONJEWEL, -1, 0};
struct sembuf SignalPDragonJewel = {SEM_P_DRAGONJEWEL, 1, 0};
struct sembuf WaitSDragonFight = {SEM_S_DRAGONFIGHT, -1, 0};
struct sembuf SignalSDragonFight = {SEM_S_DRAGONFIGHT, 1, 0};
struct sembuf WaitSDragonPlay = {SEM_S_DRAGONPLAY, -1, 0};
struct sembuf SignalSDragonPlay = {SEM_S_DRAGONPLAY, 1, 0};
struct sembuf WaitNHunterPath = {SEM_N_HUNTERPATH, -1, 0};
struct sembuf SignalNHunterPath = {SEM_N_HUNTERPATH, 1, 0};
struct sembuf WaitPHunterPath = {SEM_P_HUNTERPATH, -1, 0};
struct sembuf SignalPHunterPath = {SEM_P_HUNTERPATH, 1, 0};
struct sembuf WaitSHunterCave = {SEM_S_HUNTERCAVE, -1, 0};
struct sembuf SignalSHunterCave = {SEM_S_HUNTERCAVE, 1, 0};
struct sembuf WaitSHunterFight = {SEM_S_HUNTERFIGHT, -1, 0};
struct sembuf SignalSHunterFight = {SEM_S_HUNTERFIGHT, 1, 0};
struct sembuf WaitPHunterLeave = {SEM_P_HUNTERLEAVE, -1, 0};
struct sembuf SignalPHunterLeave = {SEM_P_HUNTERLEAVE, 1, 0};
struct sembuf WaitNThiefPath = {SEM_N_THIEFPATH, -1, 0};
struct sembuf SignalNThiefPath = {SEM_N_THIEFPATH, 1, 0};
struct sembuf WaitPThiefPath = {SEM_P_THIEFPATH, -1, 0};
struct sembuf SignalPThiefPath = {SEM_P_THIEFPATH, 1, 0};
struct sembuf WaitSThiefCave = {SEM_S_THIEFCAVE, -1, 0};
struct sembuf SignalSThiefCave = {SEM_S_THIEFCAVE, 1, 0};
struct sembuf WaitSThiefPlay = {SEM_S_THIEFPLAY, -1, 0};
struct sembuf SignalSThiefPlay = {SEM_S_THIEFPLAY, 1, 0};
struct sembuf WaitPThiefLeave = {SEM_P_THIEFLEAVE, -1, 0};
struct sembuf SignalPThiefLeave = {SEM_P_THIEFLEAVE, 1, 0};
struct sembuf WaitPTermination = {SEM_P_TERMINATION, -1, 0};
struct sembuf SignalPTermination = {SEM_P_TERMINATION, 1, 0};


void initialize();
void terminateSimulation();
void releaseResource();
void semctlChecked(int semID, int semNum, int flag, union semun seminfo);
void semopChecked(int semID, struct sembuf *operation, unsigned num);
void shmAllocate(key_t key, size_t size, int shmflg1, const void *shmaddr, int shmflg2, int *flag, int **addr);
void shmDeallocate(int flg, int *ptr);

void setTerminate();
int checkThief();
int checkHunter();
int checkTermination();
void eat(int smaugID);
void nap(int smaugID);


int main(void) {

    initialize();

    srand(time(NULL));

    int THIEF_INTERVAL = random()%LENGTH_INTERVAL;
    int HUNTER_INTERVAL = random()%LENGTH_INTERVAL;

    long long thief_time = 0;

    long long hunter_time = 0;

    int thiefPathTime = 0;

    int hunterPathTime = 0;


    printf("Input the maximum time that a thief looks for the path:");

    scanf("%d", &thiefPathTime);

    printf("Input the maximum time that a hunter looks for the path:");

    scanf("%d", &hunterPathTime);
	
	do{
		printf("Input the winning probability by hunters and thieves:");

		scanf("%f", &winProb);
	}while( winProb > 1);

    thief_time += THIEF_INTERVAL;

    hunter_time += HUNTER_INTERVAL;

    int genflag = 0; 

    long long elapsetime = 0;

    long long lastelapsetime = 0;

    struct timeval lasttime;

    struct timeval curtime;

    gettimeofday(&lasttime, NULL);

    pid_t result = fork();

    if (result < 0) {

        printf("fork error\n");

        exit(1);

    }

    if (result == 0) {

        smaug();

    }

    else {

        pid_t r;

        while (1) {

            gettimeofday(&curtime, NULL);

            elapsetime += (curtime.tv_sec - lasttime.tv_sec) * 1000000 + (curtime.tv_usec - lasttime.tv_usec);


            lasttime = curtime;



            if (checkTermination()) {

                terminateSimulation();

                int status;

                waitpid(-1, &status, 0);

                releaseResource();

                exit(0);

            }

            if (elapsetime > thief_time) {

                genflag = 2;

                thief_time += THIEF_INTERVAL;

                r = fork();

                if (r == 0) break;

            }



            if (elapsetime > hunter_time) {

                genflag = 3;

                hunter_time += HUNTER_INTERVAL;

                r = fork();

                if (r == 0) break;

            }

        }

        if (genflag == 2) thief(rand() % thiefPathTime);

        else if (genflag == 3) hunter(rand() % hunterPathTime);

        exit(0);

    }

}





void initialize() {

    semID = semget(IPC_PRIVATE, 39, 0666 | IPC_CREAT);

    //initialize values of semaphore

    //semaphores of Dragon

    seminfo.val = 0;

    semctlChecked(semID, SEM_S_DRAGONWAKEUP, SETVAL, seminfo);

    semctlChecked(semID, SEM_S_DRAGONEAT, SETVAL, seminfo);

    semctlChecked(semID, SEM_S_DRAGONFIGHT, SETVAL, seminfo);

    semctlChecked(semID, SEM_S_DRAGONPLAY, SETVAL, seminfo);

    seminfo.val = 1;


    semctlChecked(semID, SEM_P_DRAGONJEWEL, SETVAL, seminfo);

    //semaphores of hunters

    seminfo.val = 0;

    semctlChecked(semID, SEM_N_HUNTERPATH, SETVAL, seminfo);

    semctlChecked(semID, SEM_S_HUNTERCAVE, SETVAL, seminfo);

    semctlChecked(semID, SEM_S_HUNTERFIGHT, SETVAL, seminfo);

    seminfo.val = 1;

    semctlChecked(semID, SEM_P_HUNTERPATH, SETVAL, seminfo);

    semctlChecked(semID, SEM_P_HUNTERLEAVE, SETVAL, seminfo);

    //semaphores of thieves

    seminfo.val = 0;

    semctlChecked(semID, SEM_N_THIEFPATH, SETVAL, seminfo);

    semctlChecked(semID, SEM_S_THIEFCAVE, SETVAL, seminfo);

    semctlChecked(semID, SEM_S_THIEFPLAY, SETVAL, seminfo);

    seminfo.val = 1;

    semctlChecked(semID, SEM_P_THIEFPATH, SETVAL, seminfo);

    semctlChecked(semID, SEM_P_THIEFLEAVE, SETVAL, seminfo);

    //semaphores of the system

    seminfo.val = 1;

    semctlChecked(semID, SEM_P_TERMINATION, SETVAL, seminfo);



    //allocate shared memory

    //shared memory for dragon

    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numDragonJewelFlag, &numDragonJewel);

    //shared memory for hunter

    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numHunterPathFlag, &numHunterPath);

    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numHunterLeaveFlag, &numHunterLeave);

    //shared memory for thief

    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numThiefPathFlag, &numThiefPath);

    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &numThiefLeaveFlag, &numThiefLeave);

    //shared memory for the system

    shmAllocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &flagTerminationFlag, &flagTermination);

}

void semctlChecked(int semID, int semNum, int flag, union semun seminfo) {

    if (semctl(semID, semNum, flag, seminfo) == -1) {

        if (errno != EIDRM) {

            printf("errno: %8d\n", errno);

            exit(2);

        }

        else {

            exit(3);

        }

    }

}

void semopChecked(int semID, struct sembuf *operation, unsigned num) {

    if (semop(semID, operation, num) == -1) {

        if (errno != EIDRM) {

            printf("errno: %8d\n", errno);

            exit(2);

        }

        else {

            exit(3);

        }

    }

}

// function for allocating shared memory and attach it to the address provided

void shmAllocate(key_t key, size_t size, int shmflg1, const void *shmaddr, int shmflg2, int *flag, int **addr) {

    //allocate shared memory

    if ((*flag = shmget(key, size, shmflg1)) < 0) {

        printf("CREATE Shared Memory Error\n");

        exit(1);

    } else {

        printf("CREATE Shared Memory Success, %d\n", *flag);

    }

    //attach shared memory

    if ((*addr = shmat(*flag, shmaddr, shmflg2)) == (int *) -1) {

        printf("Attach Shared Memory Error\n");

        exit(1);

    } else {

        printf("Attach Shared Memory Success\n");

    }

}

// deallocate memory

void shmDeallocate(int flg, int *ptr) {

    if (shmdt(ptr) == -1) {

        printf("Detach Shared Memory Failed\n");

    }

    else {

        printf("Detach Shared Memory Success\n");

    }

    if (shmctl(flg, IPC_RMID, NULL) == -1) {

        printf("Release Shared Memory Failed\n");

    }

    else {

        printf("Release Shared Memory Success\n");

    }

}

void terminateSimulation() {

    pid_t localpid = getpid();

    pid_t localgid = getpgid(localpid);

    printf("In process %d Terminate Simulation is executed now\n", localpid);

    if (localgid != dragonGID) {

        if (killpg(dragonGID, SIGKILL) == -1 && errno == EPERM) {

            printf("Dragon not killed\n");

        }

        else printf("Dragon killed\n");

    }


    if (localgid != thiefGID) {

        if (killpg(thiefGID, SIGKILL) == -1 && errno == EPERM) {

            printf("Thieves not killed\n");

        }

        else printf("Thieves killed\n");

    }

    if (localgid != hunterGID) {

        if (killpg(hunterGID, SIGKILL) == -1 && errno == EPERM) {

            printf("Hunters not killed\n");

        }

        else printf("Hunters killed\n");

    }

}

void releaseResource() {

    printf("Releasing Resources\n");



    if (semctl(semID, 0, IPC_RMID) == -1) {

        printf("Release Semaphores Failed\n");

    }

    else {

        printf("Release Semaphores Success\n");

    }

    //shared memory for dragon

    shmDeallocate(numDragonJewelFlag, numDragonJewel);

    //shared memory for hunter

    shmDeallocate(numHunterPathFlag, numHunterPath);

    shmDeallocate(numHunterLeaveFlag, numHunterLeave);

    //shared memory for thief

    shmDeallocate(numThiefPathFlag, numThiefPath);

    shmDeallocate(numThiefLeaveFlag, numThiefLeave);

    //shared memory for the system

    shmDeallocate(flagTerminationFlag, flagTermination);

}

void setTerminate() {

    semopChecked(semID, &WaitPTermination, 1);

    *flagTermination = 1;

    semopChecked(semID, &SignalPTermination, 1);

}

int checkThief() {

    semopChecked(semID, &WaitPThiefLeave, 1);

    if (*numThiefLeave >= MAX_THIEF) {

        printf("Smaug has played with more than maximum number of thieves, the simulation will terminate\n");

        setTerminate();

        semopChecked(semID, &SignalPThiefLeave, 1);

        return 1;

    }

    else {

        semopChecked(semID, &SignalPThiefLeave, 1);

        return 0;

    }

}

int checkHunter() {

    semopChecked(semID, &WaitPHunterLeave, 1);

    if (*numHunterLeave >= MAX_TREASUREHUNTER) {

        printf("Smaug has fought with more than maximum number of hunters, the simulation will terminate\n");

        setTerminate();

        semopChecked(semID, &SignalPHunterLeave, 1);

        return 1;

    }

    else {

        semopChecked(semID, &SignalPHunterLeave, 1);

        return 0;

    }

}

int checkTermination() {

    semopChecked(semID, &WaitPTermination, 1);

    if (*flagTermination == 1) {

        semopChecked(semID, &SignalPTermination, 1);

        return 1;

    }

    else {

        semopChecked(semID, &SignalPTermination, 1);

        return 0;

    }

}

void hunter(int time) {

    pid_t localpid = getpid();

    setpgid(localpid, hunterGID);

    printf("Treasure hunter %d wandering in the valley\n", localpid);

    int pathtime = random();

    if (usleep(time) == -1) {

        /* exit when usleep interrupted by kill signal */

        if (errno == EINTR)exit(4);

    }

    semopChecked(semID, &SignalNHunterPath, 1);

    semopChecked(semID, &WaitPHunterPath, 1);

    *numHunterPath = *numHunterPath + 1;

    semopChecked(semID, &SignalPHunterPath, 1);


    printf("Treasure hunter %d is traveling to the valley\n", localpid);

    semopChecked(semID, &SignalSDragonWakeUp, 1);

    semopChecked(semID, &WaitSHunterCave, 1);

    printf("Treasure hunter %d is fighting Smaug\n", localpid);

    semopChecked(semID, &SignalSDragonFight, 1);

    semopChecked(semID, &WaitSHunterFight, 1);

    semopChecked(semID, &WaitPHunterLeave, 1);

    *numHunterLeave = *numHunterLeave + 1;

	printf("Treasure hunter %d leaves\n", localpid);

    semopChecked(semID, &SignalPHunterLeave, 1);

    if(checkHunter()) {

        terminateSimulation();

    }

    exit(0);

}

void thief(int time) {

    pid_t localpid = getpid();

    setpgid(localpid, thiefGID);

    printf("Thief %d is wandering in the valley\n", localpid);



    if (usleep(time) == -1) {

        if (errno == EINTR)exit(4);

    }

    semopChecked(semID, &SignalNThiefPath, 1);

    semopChecked(semID, &WaitPThiefPath, 1);

    *numThiefPath = *numThiefPath + 1;

    semopChecked(semID, &SignalPThiefPath, 1);


	printf("Thief %d is traveling to the valley\n", localpid);

    semopChecked(semID, &SignalSDragonWakeUp, 1);



    semopChecked(semID, &WaitSThiefCave, 1);

    printf("Thief %d is playing with Smaug\n", localpid);

    semopChecked(semID, &SignalSDragonPlay, 1);



    semopChecked(semID, &WaitSThiefPlay, 1);



    semopChecked(semID, &WaitPThiefLeave, 1);

    *numThiefLeave = *numThiefLeave + 1;
	
    printf("Thief %d leaves\n", localpid);

    semopChecked(semID, &SignalPThiefLeave, 1);

    if(checkThief()) {

        terminateSimulation();

    }

    exit(0);

}

//========================================================

//smaug eats a meal

void eat(int smaugID) {

    printf("Smaug %d take a deep breath\n", smaugID);
	if (usleep(1e6) == -1) {

        /* exit when usleep interrupted by kill signal */

        if (errno == EINTR)exit(4);

    }

}

void nap(int smaugID) {

    printf("Smaug %d is taking a nap\n", smaugID);

    if (usleep(1e6) == -1) {

        /* exit when usleep interrupted by kill signal */

        if (errno == EINTR)exit(4);

    }

}

void fight(int smaugID) {

    semopChecked(semID, &WaitNHunterPath, 1);

    *numHunterPath = *numHunterPath - 1;

    //printf("Smaug is ready to interact with a hunter waiting in the path\n");

    semopChecked(semID, &SignalSHunterCave, 1);

    semopChecked(semID, &WaitSDragonFight, 1);

    printf("Smaug %d is fighting treasure a hunter\n", smaugID);

    int fightrand = (double)rand()/RAND_MAX;

    if (fightrand > winProb) {

        semopChecked(semID, &WaitPDragonJewel, 1);
		
		printf("Smaug %d has defeated a treasure hunter\n", smaugID);

        *numDragonJewel = *numDragonJewel + BRIBE_HUNTER;

        printf("Smaug %d has added %d jewels he now has %d jewels\n",smaugID, BRIBE_HUNTER, *numDragonJewel);

        semopChecked(semID, &SignalPDragonJewel, 1);

    }

    else {

        semopChecked(semID, &WaitPDragonJewel, 1);

        printf("Smaug %d has been defeated by a treasure hunter\n", smaugID);

    if(*numDragonJewel >= REWARD_HUNTER ) {

        *numDragonJewel = *numDragonJewel - REWARD_HUNTER; 

        printf("Smaug %d has lost %d jewels, now he has %d jewels\n", smaugID, REWARD_HUNTER, *numDragonJewel);

    }

        else {
        printf("Smaug %d has no more treasure %d, now simulation terminates\n", smaugID, *numDragonJewel);

        setTerminate();

        terminateSimulation();
        

    }

        semopChecked(semID, &SignalPDragonJewel, 1);

    }

    semopChecked(semID, &SignalSHunterFight, 1);

}

void play(int smaugID) {

    semopChecked(semID, &WaitNThiefPath, 1);

    *numThiefPath = *numThiefPath - 1;

    semopChecked(semID, &SignalSThiefCave, 1);



    semopChecked(semID, &WaitSDragonPlay, 1);

    printf("Smaug %d is playing with a thief\n", smaugID);

    int fightrand = (double)rand()/RAND_MAX;

    if (fightrand > winProb) {

        semopChecked(semID, &WaitPDragonJewel, 1);
		printf("Smaug %d has defeated a thief\n", smaugID);

        *numDragonJewel = *numDragonJewel + BRIBE_THIEF;

        printf("Smaug %d has added %d jewels to his treasure he now has %d jewels\n", smaugID, BRIBE_THIEF, *numDragonJewel);

        semopChecked(semID, &SignalPDragonJewel, 1);

    }

    else {

        semopChecked(semID, &WaitPDragonJewel, 1);

        printf("Smaug %d has been defeated by a thief\n", smaugID);

    if(*numDragonJewel >= REWARD_THIEF) {

        *numDragonJewel = *numDragonJewel - REWARD_THIEF;

        printf("Smaug %d has lost %d jewels he now has %d jewels\n", smaugID, REWARD_THIEF, *numDragonJewel);

    }

    else {

        printf("Smaug %d has no more treasure %d, now simulation terminates\n", smaugID, *numDragonJewel);

        setTerminate();

        terminateSimulation();

    }

    semopChecked(semID, &SignalPDragonJewel, 1);

    }

    semopChecked(semID, &SignalSThiefPlay, 1);

}

void smaug() {

    int newWakeup = 1;

    pid_t localid = getpid();

    smaugID = localid;

    printf("Smaug %d is born\n", smaugID);

    setpgid(smaugID, dragonGID);

    printf("Smaug %d is going to sleep\n", smaugID);



    semopChecked(semID, &WaitPDragonJewel, 1);

    *numDragonJewel = INIT_JEWEL;

    semopChecked(semID, &SignalPDragonJewel, 1);



    semopChecked(semID, &WaitSDragonWakeUp, 1);

    newWakeup = 1;

    int time = 0;

    while (1) { 

        printf("Smaug %d has been woken up\n", smaugID);
		int max_zombie = 0;

        while (max_zombie < MAX_ZOMBIE) {
			max_zombie++;

            int onceMeal = 0;

            int onceThief = 0;

            int onceHunter = 0;

            semopChecked(semID, &WaitPThiefPath, 1);

            while(*numThiefPath > 0 && onceThief < THIEF_ONCE) {

                if(newWakeup==1) newWakeup = 0;

                else semopChecked(semID, &WaitSDragonWakeUp, 1);

                printf("Smaug %d smells a thief\n", smaugID);

                play(smaugID);

                onceThief ++;

            }

            semopChecked(semID, &SignalPThiefPath, 1);


            semopChecked(semID, &WaitPHunterPath, 1);

            while(*numHunterPath > 0 && onceHunter < HUNTER_ONCE) {

                if(newWakeup==1) newWakeup = 0;

                else semopChecked(semID, &WaitSDragonWakeUp, 1);



                printf("Smaug %d smells a treasure hunter\n", smaugID);

                fight(smaugID);

                onceHunter++;

            }

            semopChecked(semID, &SignalPHunterPath, 1);


            if(onceHunter > 0 || onceThief > 0) {

				 eat(smaugID);
				 nap(smaugID);
                continue;

            }

            else {

				break;

            }

        }

        printf("Smaug %d is going to sleep\n", smaugID);

        semopChecked(semID, &WaitSDragonWakeUp, 1);

        newWakeup = 1;

    }

}