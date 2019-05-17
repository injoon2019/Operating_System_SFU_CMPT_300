#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>

#define TREASUREHUNTER_MAX 4
#define THIEF_MAX 3
#define JEWEL_MAX 80
#define JEWEL_MIN 0
#define INIT_JEWEL 30
#define HUNTER_REWARDED 10
#define HUNTER_BRIBE 5
#define THIEF_REWARDED 8
#define THIEF_BRIBE 20
#define LENGTH_INTERVAL 5
#define ZOMBIE_MAX 10

const pid_t dragonGID;
const pid_t thiefGID;
const pid_t hunterGID;

#define SEM_S_DRAGONWAKEUP  0 
#define SEM_P_DRAGONJEWEL   3
#define SEM_S_DRAGONFIGHT   4
#define SEM_S_DRAGONPLAY    5
#define SEM_N_HUNTERPATH    6
#define SEM_P_HUNTERPATH    7
#define SEM_S_HUNTERCAVE    8
#define SEM_S_HUNTERFIGHT   9
#define SEM_P_HUNTERLEAVE   10
#define SEM_N_THIEFPATH     11
#define SEM_P_THIEFPATH     12
#define SEM_S_THIEFCAVE      13
#define SEM_S_THIEFPLAY      14
#define SEM_P_THIEFLEAVE     15
#define SEM_P_TERMINATION    16


float winProb = 0.0f;
pid_t smaugID;

int semID;
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
} seminfo;

struct timeval startTime;

pid_t smaugID;
const pid_t dragonGID = 300;
const pid_t thiefGID = 301;
const pid_t hunterGID = 302;

int num_dragonjewelflag = 0;
int *num_dragonjewel = NULL;
int num_hunterpathflag = 0;
int *num_hunterpath = NULL;
int num_hunterleaveflag = 0;
int *num_hunterleave = NULL;
int num_thiefpathflag = 0;
int *num_thiefpath = NULL;
int num_thiefleaveflag = 0;
int *num_thiefleave = NULL;
int flag_terminationflag = 0;
int *flag_termination = NULL;

struct sembuf waitS_dragonWakeup = { SEM_S_DRAGONWAKEUP, -1, 0 };
struct sembuf signalS_dragonWakeup = { SEM_S_DRAGONWAKEUP, 1, 0 };
struct sembuf waitP_dragonjewel = { SEM_P_DRAGONJEWEL, -1, 0 };
struct sembuf signalP_dragonjewel = { SEM_P_DRAGONJEWEL, 1, 0 };
struct sembuf waitS_dragonfight = { SEM_S_DRAGONFIGHT, -1, 0 };
struct sembuf signalS_dragonfight = { SEM_S_DRAGONFIGHT, 1, 0 };
struct sembuf waitS_dragonplay = { SEM_S_DRAGONPLAY, -1, 0 };
struct sembuf signalS_dragonplay = { SEM_S_DRAGONPLAY, 1, 0 };
struct sembuf waitN_hunterpath = { SEM_N_HUNTERPATH, -1, 0 };
struct sembuf signalN_hunterpath = { SEM_N_HUNTERPATH, 1, 0 };
struct sembuf waitP_hunterpath = { SEM_P_HUNTERPATH, -1, 0 };
struct sembuf signalP_hunterpath = { SEM_P_HUNTERPATH, 1, 0 };
struct sembuf waitS_huntercave = { SEM_S_HUNTERCAVE, -1, 0 };
struct sembuf signalS_huntercave = { SEM_S_HUNTERCAVE, 1, 0 };
struct sembuf waitS_hunterfight = { SEM_S_HUNTERFIGHT, -1, 0 };
struct sembuf signalS_hunterfight = { SEM_S_HUNTERFIGHT, 1, 0 };
struct sembuf waitP_hunterleave = { SEM_P_HUNTERLEAVE, -1, 0 };
struct sembuf signalP_hunterleave = { SEM_P_HUNTERLEAVE, 1, 0 };
struct sembuf waitN_thiefpath = { SEM_N_THIEFPATH, -1, 0 };
struct sembuf signalN_thiefpath = { SEM_N_THIEFPATH, 1, 0 };
struct sembuf waitP_thiefpath = { SEM_P_THIEFPATH, -1, 0 };
struct sembuf signalP_thiefpath = { SEM_P_THIEFPATH, 1, 0 };
struct sembuf waitS_thiefcave = { SEM_S_THIEFCAVE, -1, 0 };
struct sembuf signalS_thiefcave = { SEM_S_THIEFCAVE, 1, 0 };
struct sembuf waitS_thiefplay = { SEM_S_THIEFPLAY, -1, 0 };
struct sembuf signalS_thiefplay = { SEM_S_THIEFPLAY, 1, 0 };
struct sembuf waitP_thiefleave = { SEM_P_THIEFLEAVE, -1, 0 };
struct sembuf signalP_thiefleave = { SEM_P_THIEFLEAVE, 1, 0 };
struct sembuf waitP_termination = { SEM_P_TERMINATION, -1, 0 };
struct sembuf signalP_termination = { SEM_P_TERMINATION, 1, 0 };

void initialize() {
	semID = semget(IPC_PRIVATE, 39, 0666 | IPC_CREAT);

	seminfo.val = 0;

	semctl_checked(semID, SEM_S_DRAGONWAKEUP, SETVAL, seminfo);
	semctl_checked(semID, SEM_S_DRAGONFIGHT, SETVAL, seminfo);
	semctl_checked(semID, SEM_S_DRAGONPLAY, SETVAL, seminfo);
	semctl_checked(semID, SEM_N_HUNTERPATH, SETVAL, seminfo);
	semctl_checked(semID, SEM_S_HUNTERCAVE, SETVAL, seminfo);
	semctl_checked(semID, SEM_S_HUNTERFIGHT, SETVAL, seminfo);
	semctl_checked(semID, SEM_N_THIEFPATH, SETVAL, seminfo);
	semctl_checked(semID, SEM_S_THIEFCAVE, SETVAL, seminfo);
	semctl_checked(semID, SEM_S_THIEFPLAY, SETVAL, seminfo);

	seminfo.val = 1;
	semctl_checked(semID, SEM_P_DRAGONJEWEL, SETVAL, seminfo);
	semctl_checked(semID, SEM_P_HUNTERPATH, SETVAL, seminfo);
	semctl_checked(semID, SEM_P_HUNTERLEAVE, SETVAL, seminfo);

	semctl_checked(semID, SEM_P_THIEFPATH, SETVAL, seminfo);
	semctl_checked(semID, SEM_P_THIEFLEAVE, SETVAL, seminfo);

	//semaphores of the system
	semctl_checked(semID, SEM_P_TERMINATION, SETVAL, seminfo);
	//allocate shared memory

	//shared memory for dragon

	shm_allocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &num_dragonjewelflag, &num_dragonjewel);
	shm_allocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &num_hunterpathflag, &num_hunterpath);
	shm_allocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &num_hunterleaveflag, &num_hunterleave);
	shm_allocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &num_thiefpathflag, &num_thiefpath);
	shm_allocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &num_thiefleaveflag, &num_thiefleave);
	shm_allocate(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666, NULL, 0, &flag_terminationflag, &flag_termination);

}
void semctl_checked(int semID, int semNum, int flag, union semun seminfo) {

	if (semctl(semID, semNum, flag, seminfo) == -1) {
		exit(3);
	}

}

void semop_checked(int semID, struct sembuf *operation, unsigned num) {
	if (semop(semID, operation, num) == -1) {

		exit(3);
	}
}

// function for allocating shared memory and attach it to the address provided

void shm_allocate(key_t key, size_t size, int shmflg1, const void *shmaddr, int shmflg2, int *flag, int **addr) {

	//allocate shared memory
	if ((*flag = shmget(key, size, shmflg1)) < 0) {
		exit(1);
	}

	//attach shared memory
	if ((*addr = shmat(*flag, shmaddr, shmflg2)) == (int *)-1) {
		exit(1);
	}

}


// deallocate memory

void shm_deallocate(int flg, int *ptr) {
	shmdt(ptr);
	shmctl(flg, IPC_RMID, NULL);
}

void terminate_simulation() {
	pid_t localpid = getpid();
	pid_t localgid = getpgid(localpid);
	printf("In process %d Terminate Simulation is executed now\n", localpid);

}

void release_resource() {

	if (semctl(semID, 0, IPC_RMID) == -1) {
		printf("Release Semaphores Failed\n");
	}

	shm_deallocate(num_dragonjewelflag, num_dragonjewel);
	shm_deallocate(num_hunterpathflag, num_hunterpath);
	shm_deallocate(num_hunterleaveflag, num_hunterleave);
	shm_deallocate(num_thiefpathflag, num_thiefpath);
	shm_deallocate(num_thiefleaveflag, num_thiefleave);
	shm_deallocate(flag_terminationflag, flag_termination);
}

void set_terminate() {

	semop_checked(semID, &waitP_termination, 1);
	*flag_termination = 1;
	semop_checked(semID, &signalP_termination, 1);
}

int check_thief() {

	semop_checked(semID, &waitP_thiefleave, 1);

	if (*num_thiefleave >= THIEF_MAX) {
		printf("Smaug has finished a game\n");
		set_terminate();
		semop_checked(semID, &signalP_thiefleave, 1);
		return 1;
	}

	else {
		semop_checked(semID, &signalP_thiefleave, 1);
		return 0;
	}

}

int check_hunter() {

	semop_checked(semID, &waitP_hunterleave, 1);

	if (*num_hunterleave >= TREASUREHUNTER_MAX) {
		printf("Smaug has finished a battle\n");
		set_terminate();
		semop_checked(semID, &signalP_hunterleave, 1);
		return 1;
	}

	else {
		semop_checked(semID, &signalP_hunterleave, 1);
		return 0;
	}

}

int check_termination() {

	semop_checked(semID, &waitP_termination, 1);

	if (*flag_termination == 1) {
		semop_checked(semID, &signalP_termination, 1);
		return 1;
	}

	else {
		semop_checked(semID, &signalP_termination, 1);
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

	semop_checked(semID, &signalN_hunterpath, 1);
	semop_checked(semID, &waitP_hunterpath, 1);

	*num_hunterpath = *num_hunterpath + 1;

	semop_checked(semID, &signalP_hunterpath, 1);
	printf("Treasure hunter %d is traveling to the valley\n", localpid);

	semop_checked(semID, &signalS_dragonWakeup, 1);
	semop_checked(semID, &waitS_huntercave, 1);
	printf("Treasure hunter %d is fighting Smaug\n", localpid);

	semop_checked(semID, &signalS_dragonfight, 1);
	semop_checked(semID, &waitS_hunterfight, 1);
	semop_checked(semID, &waitP_hunterleave, 1);

	*num_hunterleave = *num_hunterleave + 1;

	printf("Treasure hunter %d leaves\n", localpid);

	semop_checked(semID, &signalP_hunterleave, 1);

	if (check_hunter()) {
		terminate_simulation();
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

	semop_checked(semID, &signalN_thiefpath, 1);
	semop_checked(semID, &waitP_thiefpath, 1);

	*num_thiefpath = *num_thiefpath + 1;
	semop_checked(semID, &signalP_thiefpath, 1);

	printf("Thief %d is traveling to the valley\n", localpid);
	semop_checked(semID, &signalS_dragonWakeup, 1);

	semop_checked(semID, &waitS_thiefcave, 1);
	printf("Thief %d is playing with Smaug\n", localpid);

	semop_checked(semID, &signalS_dragonplay, 1);
	semop_checked(semID, &waitS_thiefplay, 1);

	semop_checked(semID, &waitP_thiefleave, 1);
	*num_thiefleave = *num_thiefleave + 1;

	printf("Thief %d leaves\n", localpid);
	semop_checked(semID, &signalP_thiefleave, 1);

	if (check_thief()) {
		terminate_simulation();
	}

	exit(0);

}

void fight(int smaugID) {

	semop_checked(semID, &waitN_hunterpath, 1);
	*num_hunterpath = *num_hunterpath - 1;

	semop_checked(semID, &signalS_huntercave, 1);
	semop_checked(semID, &waitS_dragonfight, 1);

	printf("Smaug %d is fighting treasure a hunter\n", smaugID);

	int fightrand = (double)rand() / RAND_MAX;

	if (fightrand > winProb) {

		semop_checked(semID, &waitP_dragonjewel, 1);
		printf("Smaug %d has defeated a treasure hunter\n", smaugID);

		*num_dragonjewel = *num_dragonjewel + HUNTER_BRIBE;

		printf("Smaug %d has added %d jewels he now has %d jewels\n", smaugID, HUNTER_BRIBE, *num_dragonjewel);
		semop_checked(semID, &signalP_dragonjewel, 1);
	}

	else {
		semop_checked(semID, &waitP_dragonjewel, 1);

		printf("Smaug %d has been defeated by a treasure hunter\n", smaugID);

		if (*num_dragonjewel >= HUNTER_REWARDED) {
			*num_dragonjewel = *num_dragonjewel - HUNTER_REWARDED;
			printf("Smaug %d has lost %d jewels, now he has %d jewels\n", smaugID, HUNTER_REWARDED, *num_dragonjewel);
		}

		else {
			printf("Smaug %d has no more treasure %d, now simulation terminates\n", smaugID, *num_dragonjewel);
			set_terminate();
			terminate_simulation();
		}

		semop_checked(semID, &signalP_dragonjewel, 1);
	}
	semop_checked(semID, &signalS_hunterfight, 1);
}

void play(int smaugID) {
	pid_t localpid = getpid();

	semop_checked(semID, &waitN_thiefpath, 1);
	*num_thiefpath = *num_thiefpath - 1;
	semop_checked(semID, &signalS_thiefcave, 1);
	semop_checked(semID, &waitS_dragonplay, 1);

	printf("Smaug %d is playing with a thief\n", smaugID);
	int fightrand = (double)rand() / RAND_MAX;

	if (fightrand > winProb) {

		semop_checked(semID, &waitP_dragonjewel, 1);
		printf("Smaug %d has defeated a thief\n", smaugID);

		*num_dragonjewel = *num_dragonjewel + THIEF_BRIBE;

		printf("Smaug %d has added %d jewels to his treasure he now has %d jewels\n", smaugID, THIEF_BRIBE, *num_dragonjewel);
		semop_checked(semID, &signalP_dragonjewel, 1);
	}

	else {

		semop_checked(semID, &waitP_dragonjewel, 1);

		printf("Smaug %d has been defeated by a thief\n", smaugID);

		if (*num_dragonjewel >= THIEF_REWARDED) {

			*num_dragonjewel = *num_dragonjewel - THIEF_REWARDED;

			printf("Smaug %d has lost %d jewels he now has %d jewels\n", smaugID, THIEF_REWARDED, *num_dragonjewel);

		}

		else {
			printf("Smaug %d has only %d treausre, now simulation terminates\n", smaugID, *num_dragonjewel);
			set_terminate();
			terminate_simulation();
		}

		semop_checked(semID, &signalP_dragonjewel, 1);

	}
	semop_checked(semID, &signalS_thiefplay, 1);
}

void smaug() {

	int newWakeup = 1;
	pid_t localid = getpid();
	smaugID = localid;
	setpgid(smaugID, dragonGID);
	semop_checked(semID, &waitP_dragonjewel, 1);

	*num_dragonjewel = INIT_JEWEL;
	semop_checked(semID, &signalP_dragonjewel, 1);
	semop_checked(semID, &waitS_dragonWakeup, 1);

	int time = 0;

	for (; ;) {

		printf("Smaug %d has been woken up\n", smaugID);
		for (int zombie_max = 0; zombie_max < ZOMBIE_MAX; zombie_max++) {

			semop_checked(semID, &waitP_thiefpath, 1);

			for (;;) {
				if (*num_thiefpath <= 0)
					break;

				if (newWakeup == 1) newWakeup = 0;

				else semop_checked(semID, &waitS_dragonWakeup, 1);

				printf("Smaug %d smells a thief\n", smaugID);

				play(smaugID);

			}
			semop_checked(semID, &signalP_thiefpath, 1);
			semop_checked(semID, &waitP_hunterpath, 1);

			for (; ; ) {
				if (*num_hunterpath <= 0)
					break;

				if (newWakeup == 1) newWakeup = 0;

				else semop_checked(semID, &waitS_dragonWakeup, 1);

				printf("Smaug %d smells a treasure hunter\n", smaugID);

				fight(smaugID);

			}

			semop_checked(semID, &signalP_hunterpath, 1);
			break;
		}

		printf("Smaug %d is going to sleep\n", smaugID);
		semop_checked(semID, &waitS_dragonWakeup, 1);

		newWakeup = 1;

	}

}

int main(void) {

	initialize();

	srand(time(NULL));
	int thief_interval = random() % LENGTH_INTERVAL;
	int hunter_interval = random() % LENGTH_INTERVAL;

	long long thief_time = 0;
	long long hunter_time = 0;

	int maximumThiefInterval = 0;
	int maximumHunterInterval = 0;

	printf("Type in the maximum thief interval:");
	scanf("%d", &maximumThiefInterval);

	printf("Type in the maximum hunter interval:");
	scanf("%d", &maximumHunterInterval);

	printf("Type in the the winning probability by hunters and thieves:");
	scanf("%f", &winProb);

	thief_time += thief_interval;
	hunter_time += hunter_interval;

	int genflag = 0;
	long long elapsedtime = 0;
	long long final_elapsedtime = 0;

	struct timeval lasttime;
	struct timeval curtime;
	gettimeofday(&lasttime, NULL);

	pid_t result = fork();

	if (result == 0) {
		smaug();
	}

	else {
		pid_t temp;

		for (;;) {
			gettimeofday(&curtime, NULL);
			elapsedtime += (curtime.tv_sec - lasttime.tv_sec) * 1000000 + (curtime.tv_usec - lasttime.tv_usec);
			lasttime = curtime;

			if (check_termination()) {
				terminate_simulation();
				int status;
				waitpid(-1, &status, 0);
				release_resource();
				exit(0);

			}

			if (elapsedtime > thief_time) {
				genflag = 2;
				thief_time += thief_interval;
				temp = fork();

				if (temp == 0) break;
			}

			if (elapsedtime > hunter_time) {
				genflag = 3;
				hunter_time += hunter_interval;

				temp = fork();
				if (temp == 0) break;
			}
		}

		if (genflag == 2) thief(rand() % maximumThiefInterval);

		else if (genflag == 3) hunter(rand() % maximumHunterInterval);
		return 0;
		exit(0);

	}
}