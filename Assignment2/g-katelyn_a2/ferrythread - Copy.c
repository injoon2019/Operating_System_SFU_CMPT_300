#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h> 
#include <pthread.h>
#include <semaphore.h>

#define FERRY_CAPACITY 6
#define CYCLES 5

// VTHREADS

pthread_t createVehicleThread;
pthread_t captainThread;
pthread_t vehicles[300];
int countThread = 0;

//Mutex

pthread_mutex_t mutexCarsQueued;
int CountCarsQueued = 0;
pthread_mutex_t mutexTrucksQueued;
int CountTrucksQueued = 0;
pthread_mutex_t mutexCarsUnloaded;
int CountCarsUnloaded = 0;
pthread_mutex_t mutexTrucksUnloaded;
int CountTrucksUnloaded = 0;

// Counting semaphores for cars and trucks

sem_t carsQueued;
sem_t trucksQueued;
sem_t carsLoaded;
sem_t trucksLoaded;

sem_t sailing;
sem_t arrived;
sem_t waitUnload;
sem_t readyUnload;

sem_t carsUnloaded;
sem_t trucksUnloaded;
sem_t Exit;


struct timeval tm;
int ferry_filled_spots = 0;
int truck_probability = 0;
int maxArrivalTime = 0;
int seed = 0;


void* truck();
void* car();
void* truck();
void* createVehicle();
void* captain();

int timeChange(const struct timeval startTime);	//janice gave

int main() {
	sem_init(&carsQueued, 0, 0);
	sem_init(&trucksQueued, 0, 0);
	sem_init(&carsLoaded, 0, 0);
	sem_init(&trucksLoaded, 0, 0);

	sem_init(&sailing, 0, 0);
	sem_init(&arrived, 0, 0);
	sem_init(&waitUnload, 0, 0);
	sem_init(&readyUnload, 0, 0);

	sem_init(&carsUnloaded, 0, 0);
	sem_init(&trucksLoaded, 0, 0);
	sem_init(&Exit, 0, 0);

	//pthread_create(&createVehicleThread)

	printf("Please enter integer values for the following variables\n");
	printf("Enter the percent probability that the next vehicle is a truck\n");
	scanf("%d", &truck_probability);
	printf("Enter the maximum length of the interval between vehicles time interval should be >1000 but <5000\n");
	scanf("%d", &maxArrivalTime);
	printf("Enter the seed for random number generation (2<seed<%d\n)", RAND_MAX);
	scanf("%d", &seed);

	pthread_create(&createVehicleThread, NULL, createVehicle, NULL);
	pthread_create(&captainThread, NULL, captain, NULL);
	pthread_mutex_init(&mutexCarsQueued, NULL);
	pthread_mutex_init(&mutexTrucksQueued, NULL);
	pthread_mutex_init(&mutexCarsUnloaded, NULL);
	pthread_mutex_init(&mutexTrucksUnloaded, NULL);


	pthread_join(createVehicleThread, NULL);
	pthread_join(captainThread, NULL);
	pthread_mutex_destroy(&mutexTrucksQueued);
	pthread_mutex_destroy(&mutexCarsQueued);
	pthread_mutex_destroy(&mutexTrucksUnloaded);
	pthread_mutex_destroy(&mutexCarsUnloaded);



	sem_destroy(&carsQueued);
	sem_destroy(&trucksQueued);
	sem_destroy(&carsLoaded);
	sem_destroy(&trucksLoaded);

	sem_destroy(&sailing);
	sem_destroy(&arrived);
	sem_destroy(&waitUnload);
	sem_destroy(&readyUnload);

	sem_destroy(&carsUnloaded);
	sem_destroy(&trucksUnloaded);
	sem_destroy(&Exit);

	return 0;
}




void* createVehicle()
{
	struct timeval t1;   //start time
	int elapsed = 0;
	int t2 = 0;    //second time to measure
	printf("CREATEVEHICLE:      Vehicle creation thread has been started\n");
	gettimeofday(&t1, NULL);
	elapsed = timeChange(t1);
	srand(seed);

	while (1)
	{
		elapsed = timeChange(t1);
		while (elapsed >= t2)
		{
			printf("CREATEVEHICLE:      Elapsed time %d msec\n", elapsed);
			if (t2 > 0)
			{
				//create car when random number is greater than truck probability
				if (rand() % 101 >  truck_probability) {
					pthread_create(&(vehicles[countThread++]), NULL, car, NULL);
					printf("CREATEVEHICLE:      Created a truck thread\n");
				}
				else {	//create truck thread

					pthread_create(&(vehicles[countThread++]), NULL, truck, NULL);
					printf("CREATEVEHICLE:      Created a truck thread\n");
				}
			}
			//Determine when the next vehicle should arrive. 1000<= wait till next vehicle <= k
			t2 = (rand() % (maxArrivalTime - 1000) + 1001) + elapsed;
			printf("CREATEVEHICLE:      Next arrival time %d msec\n", t2);
		}
	}


}

void* car(void* arg)
{
	//To print thread ID
	pid_t threadID = pthread_self();

	pthread_mutex_lock(&mutexCarsQueued);
	CountCarsQueued++;
	pthread_mutex_unlock(&mutexCarsQueued);
	printf("CAR:      	    Car with threadID %u queued\n", threadID);
	sem_wait(&carsQueued);
	printf("CAR:      	    Car with threadID %u leaving the queue to load\n", threadID);

	printf("CAR:      	    Car with threadID %u is onboard the ferry\n", threadID);
	sem_post(&carsLoaded);

	//need to check later
	sem_wait(&sailing);
	sem_wait(&arrived);
	sem_post(&readyUnload);


	// unload

	sem_wait(&waitUnload);
	printf("CAR:      	    Car with threadID %u is now unloading\n", threadID);

	pthread_mutex_lock(&mutexCarsUnloaded);
	CountCarsUnloaded++;
	pthread_mutex_unlock(&mutexCarsUnloaded);
	printf("CAR:      	    Car with threadID %u has unloaded\n", threadID);

	sem_post(&carsUnloaded);
	sem_wait(&Exit);

	printf("CAR:      	    Car with threadID %u is about to exit\n", threadID);

	pthread_exit(0);

}


void* truck(void* arg)
{
	//To print thread ID
	pid_t threadID = pthread_self();

	pthread_mutex_lock(&mutexTrucksQueued);
	CountTrucksQueued;
	pthread_mutex_unlock(&mutexTrucksQueued);
	printf("TRUCK:      	    Truck with threadID %u queued\n", threadID);


	sem_wait(&trucksQueued);
	printf("TRUCK:      	    Truck with threadID %u leaving the queue to load\n", threadID);

	printf("TRUCK:      	    Truck with threadID %u is onboard the ferry\n", threadID);
	sem_post(&trucksLoaded);

	sem_wait(&sailing);

	sem_post(&arrived);
	sem_post(&readyUnload);

	// unload

	sem_wait(&waitUnload);
	printf("TRUCK:      	    Truck with threadID %u is now unloading\n", threadID);

	pthread_mutex_lock(&mutexTrucksUnloaded);
	CountTrucksUnloaded++;
	pthread_mutex_unlock(&mutexTrucksUnloaded);
	printf("TRUCK:      	    Truck with threadID %u has unloaded\n", threadID);

	sem_post(&trucksUnloaded);
	sem_wait(&Exit);

	printf("TRUCK:      	    Truck with threadID %u is about to exit\n", threadID);

	pthread_exit(0);

}


//Given by Janice
int timeChange(const struct timeval startTime)
{

	struct timeval nowTime;
	long int elapsed;
	int elapsedTime;
	gettimeofday(&nowTime, NULL);

	elapsed = (nowTime.tv_sec - startTime.tv_sec) * 1000000

		+ (nowTime.tv_usec - startTime.tv_usec);

	elapsedTime = elapsed / 1000;

	return elapsedTime;
}




void* captain()
{
	int loads = 0;
	int number_CarsQueued = 0;
	int number_TrucksQueued = 0;
	int number_TrucksLoaded = 0;
	int number_SpacesFilled = 0;
	int number_SpacesEmpty = 0;
	int number_Vehicles = 0;
	int counter = 0;

	printf("CAPTAIN:  	    Captain thread started\n");
	while (loads < CYCLES)
	{
		number_TrucksLoaded = 0;
		number_SpacesFilled = 0;
		number_Vehicles = 0;

		while (number_SpacesFilled < FERRY_CAPACITY) {
			pthread_mutex_lock(&mutexTrucksQueued);
			pthread_mutex_lock(&mutexCarsQueued);
			number_TrucksQueued = CountTrucksQueued;
			number_CarsQueued = CountCarsQueued;
			pthread_mutex_unlock(&mutexCarsQueued);
			pthread_mutex_unlock(&mutexTrucksQueued);
			while (number_TrucksQueued > 0 && number_SpacesFilled < FERRY_CAPACITY - 1 && number_TrucksLoaded < 2)
			{
				pthread_mutex_lock(&mutexTrucksQueued);
				CountTrucksQueued--;
				printf("CAPTAIN:  	    Truck selected for loading\n");
				sem_post(&trucksQueued);
				pthread_mutex_unlock(&mutexTrucksQueued);
				number_TrucksQueued--;
				number_TrucksLoaded++;
				number_SpacesFilled += 2;
				number_Vehicles++;
			}
			while (number_CarsQueued > 0 && number_SpacesFilled < FERRY_CAPACITY) {
				pthread_mutex_lock(&mutexCarsQueued);
				CountCarsQueued--;
				printf("CAPTAIN:  	    Car selected for loading \n");
				sem_post(&carsQueued);
				pthread_mutex_unlock(&mutexCarsQueued);
				number_CarsQueued--;
				number_SpacesFilled++;
				number_Vehicles++;
			}
		}



		for (counter = 0; counter < number_TrucksLoaded; counter++) {
			sem_wait(&trucksLoaded);
			printf("CAPTAIN:  	    Captain knows truck is loaded\n");
		}
		for (counter = 0; counter < number_Vehicles - number_TrucksLoaded; counter++)
		{
			sem_wait(&carsLoaded);
			printf("CAPTAIN:  	    Captain knows car is loaded\n");
		}
		printf("CAPTAIN:  	    Ferry is full, starting to sail\n");


		//signal all vehicles waiting to sail 

		for (counter = 0; counter < number_Vehicles; counter++) {
			sem_post(&sailing);
		}

		for (counter = 0; counter < number_Vehicles; counter++) {
			sem_post(&arrived);
		}
		for (counter = 0; counter < number_Vehicles; counter++) {
			sem_wait(&readyUnload);
		}

		for (counter = 0; counter < number_Vehicles; counter++) {
			sem_post(&waitUnload);
		}



		//captain signals unload
		number_SpacesEmpty = 0;
		while (number_SpacesEmpty < FERRY_CAPACITY) {
			pthread_mutex_lock(&mutexCarsUnloaded);
			if (CountCarsUnloaded> 0) {
				sem_wait(&carsUnloaded);
				CountCarsUnloaded--;
				number_SpacesEmpty++;
				printf("CAPTAIN:  	    Captain knows a car has unloaded from the ferry\n");
				printf("CAPTAIN:  	    Captain sees a car leaving the ferry terminal\n");
			}
			pthread_mutex_unlock(&mutexCarsUnloaded);

			pthread_mutex_lock(&mutexTrucksUnloaded);
			if (CountTrucksUnloaded > 0) {
				sem_wait(&trucksUnloaded);
				CountTrucksUnloaded--;
				number_SpacesEmpty += 2;
				printf("CAPTAIN:  	    Captain knows a truck has unloaded from the ferry");
				printf("CAPTAIN:  	    Captain sees a truck leaving the ferry terminal\n");
			}
			pthread_mutex_unlock(&mutexTrucksUnloaded);
		}

		for (counter = 0; counter < number_Vehicles; counter++) {
			//printf("CAPTAINCAPTAIN                unloaded vehicle %d is about to exit\n", counter);
			sem_post(&Exit);
		}

		printf("\n\n\n\n");
		loads++;
	}
	printf("Unloading complete\n");
	exit(0);
}



































































