#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h> 
#include <pthread.h>
#include <semaphore.h>

#define LOAD_MAX 5
#define QUEUE_MAX 8
#define SPOTS_ON_FERRY 6

int func_min(a,b){
	return a<b?a:b;
}
struct synch_counter{
	int counter;
	pthread_mutex_t mutex;
};
struct arg_struct {
    int probability_vehicle_truck;
    int max_length_interval;
    int seed;
};

sem_t car_queue, truck_queue, car_loaded, truck_loaded, car_sailing, truck_sailing, car_unloaded, truck_unloaded, Exit, car_exit, truck_exit, car_end, truck_end; //8 counters
struct synch_counter car_queue_counter, truck_queue_counter, car_loaded_counter, truck_loaded_counter, 
car_sailing_counter, truck_sailing_counter, car_unloaded_counter, truck_unloaded_counter,
car_exit_counter, truck_exit_counter; //8 counters
sem_t simulation_time;

void* runCar(void* arg);
void* runTruck(void* arg);
void* runCaptain(void* arg);
void* createVehicle(void* arg);
int timeChange( const struct timeval startTime );
int sem_wait_Checked(sem_t *semaphoreID);
int sem_postChecked(sem_t *semaphoreID);
int sem_initChecked(sem_t *semaphoreID, int pshraed, unsigned int value);
int sem_destroyChecked(sem_t *semaphoreID);
int pthread_mutex_initChecked(pthread_mutex_t *mutexID, const pthread_mutexattr_t *attrib);
int pthread_mutex_destroyChecked(pthread_mutex_t *mutexID);

int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_init(pthread_mutex_t *restrict mutex,
const pthread_mutexattr_t *restrict attr);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

int main(int argc, char* argv[])
{
	 // initialize semaphore, only to be used with threads in this process, set value to 1
	sem_initChecked(&car_queue, 0, 0);
	sem_initChecked(&truck_queue, 0, 0);
	sem_initChecked(&car_loaded, 0, 0);
	sem_initChecked(&truck_loaded, 0, 0);
	sem_initChecked(&car_sailing, 0, 0);
	sem_initChecked(&truck_sailing, 0, 0);
	sem_initChecked(&car_unloaded, 0, 0);
	sem_initChecked(&truck_unloaded, 0, 0);
	sem_initChecked(&car_exit, 0, 0);
	sem_initChecked(&truck_exit, 0, 0);
	sem_initChecked(&car_end, 0, 0);
	sem_initChecked(&truck_end, 0, 0);
    sem_initChecked(&Exit, 0, 0);
	
	pthread_mutex_initChecked(&car_queue_counter.mutex, NULL);
	pthread_mutex_initChecked(&truck_queue_counter.mutex, NULL);
	pthread_mutex_initChecked(&car_loaded_counter.mutex, NULL);
	pthread_mutex_initChecked(&truck_loaded_counter.mutex, NULL);
	pthread_mutex_initChecked(&car_sailing_counter.mutex, NULL);
	pthread_mutex_initChecked(&truck_sailing_counter.mutex, NULL);
	pthread_mutex_initChecked(&car_unloaded_counter.mutex, NULL);
	pthread_mutex_initChecked(&truck_unloaded_counter.mutex, NULL);
	pthread_mutex_initChecked(&car_exit_counter.mutex, NULL);
	pthread_mutex_initChecked(&truck_exit_counter.mutex, NULL);
	
	int probability_vehicle_truck=100, max_length_interval=1, seed=1;
	printf("Please enter integer values for the following variables\n");
	while (probability_vehicle_truck > 99 || probability_vehicle_truck < 0) {
		printf("Enter the percent probability that the next vehicle is a truck:  ");
		scanf("%d", &probability_vehicle_truck);
	}
	while (max_length_interval < 1000 || max_length_interval > 5000) {
		printf("Enter the maximum length of the interval between vehicles time interval should be >1000 but <5000:  ");
		scanf("%d", &max_length_interval);
	}
	while (seed < 2 || seed > RAND_MAX){
		printf("Enter the seed for random number generation (2<seed<%d):  ", RAND_MAX);
		scanf("%d",&seed);
	}
	
	struct arg_struct args ={probability_vehicle_truck, max_length_interval, seed};
	pthread_t create_vehicle;
	pthread_create(&create_vehicle, NULL, createVehicle, (void*)&args);

	pthread_t create_captain;
	pthread_create(&create_captain, NULL, runCaptain, NULL);

	pthread_join(create_vehicle, NULL);
	pthread_join(create_captain,NULL);
	
	pthread_mutex_destroyChecked(&car_queue_counter.mutex);
	pthread_mutex_destroyChecked(&truck_queue_counter.mutex);
	pthread_mutex_destroyChecked(&car_loaded_counter.mutex);
	pthread_mutex_destroyChecked(&truck_loaded_counter.mutex);
	pthread_mutex_destroyChecked(&car_sailing_counter.mutex);
	pthread_mutex_destroyChecked(&truck_sailing_counter.mutex);
	pthread_mutex_destroyChecked(&car_unloaded_counter.mutex);
	pthread_mutex_destroyChecked(&truck_unloaded_counter.mutex);
	pthread_mutex_destroyChecked(&car_exit_counter.mutex);
	pthread_mutex_destroyChecked(&truck_exit_counter.mutex);
	
	sem_destroyChecked(&car_queue);
	sem_destroyChecked(&truck_queue);
	sem_destroyChecked(&car_loaded);
	sem_destroyChecked(&truck_loaded);
	sem_destroyChecked(&car_sailing);
	sem_destroyChecked(&truck_sailing);
	sem_destroyChecked(&car_unloaded);
	sem_destroyChecked(&truck_unloaded);
	sem_destroyChecked(&car_exit);
	sem_destroyChecked(&truck_exit);
	sem_destroyChecked(&car_end);
	sem_destroyChecked(&truck_end);
	sem_destroyChecked(&Exit);
	
	return 0;
}

void* runCar(void* arg)	
{
	//To print thread ID
	pid_t threadID = pthread_self();
	printf("CAR:      	    Car with threadID %u queued\n", threadID);
	
	pthread_mutex_lock(&car_queue_counter.mutex);
	car_queue_counter.counter++;
	pthread_mutex_unlock(&car_queue_counter.mutex);
	
	sem_wait(&car_queue);
	
	printf("CAR:      	    Car with threadID %u leaving the queue to load\n", threadID);
	pthread_mutex_lock(&car_queue_counter.mutex);
	car_queue_counter.counter--;
	pthread_mutex_unlock(&car_queue_counter.mutex);
	
	sem_post(&car_loaded);
	
	printf("CAR:      	    Car with threadID %u is onboard the ferry\n", threadID);
	
	pthread_mutex_lock(&car_sailing_counter.mutex);
	car_sailing_counter.counter++;
	pthread_mutex_unlock(&car_sailing_counter.mutex);

	sem_wait(&car_sailing);
	printf("CAR:      	    Car with threadID %u is now unloading\n", threadID);

	pthread_mutex_lock(&car_sailing_counter.mutex);
	car_sailing_counter.counter--;
	pthread_mutex_unlock(&car_sailing_counter.mutex);
	
	sem_post(&car_unloaded);
	printf("CAR:      	    Car with threadID %u has unloaded\n", threadID);

	pthread_mutex_lock(&car_exit_counter.mutex);
	car_exit_counter.counter++;
	pthread_mutex_unlock(&car_exit_counter.mutex);
	sem_wait(&car_exit);
	printf("CAR:      	    Car with threadID %u is about to exit\n", threadID);
	pthread_mutex_lock(&car_exit_counter.mutex);
	car_exit_counter.counter--;
	pthread_mutex_unlock(&car_exit_counter.mutex);
	
	sem_post(&car_end);
}

void* runTruck(void* arg)	
{
	//To print thread ID
	//pthread_id_np_t   threadID;
	pid_t threadID = pthread_self();
	printf("TRUCK:      	    Truck with threadID %u queued\n", threadID);

	pthread_mutex_lock(&truck_queue_counter.mutex);
	truck_queue_counter.counter++;
	pthread_mutex_unlock(&truck_queue_counter.mutex);
	
	sem_wait(&truck_queue);
	
	printf("TRUCK:      	    Truck with threadID %u leaving the queue to load\n", threadID);
	pthread_mutex_lock(&truck_queue_counter.mutex);
	truck_queue_counter.counter--;
	pthread_mutex_unlock(&truck_queue_counter.mutex);
	sem_post(&truck_loaded);
	printf("TRUCK:      	    Truck with threadID %u is onboard the ferry\n", threadID);

	pthread_mutex_lock(&truck_sailing_counter.mutex);
	truck_sailing_counter.counter++;
	pthread_mutex_unlock(&truck_sailing_counter.mutex);
	sem_wait(&truck_sailing);
	printf("TRUCK:      	    Truck with threadID %u is now unloading\n", threadID);

	pthread_mutex_lock(&truck_sailing_counter.mutex);
	truck_sailing_counter.counter--;
	pthread_mutex_unlock(&truck_sailing_counter.mutex);
	sem_post(&truck_unloaded);
	printf("TRUCK:      	    Truck with threadID %u has unloaded\n", threadID);
	
	
	pthread_mutex_lock(&truck_exit_counter.mutex);
	truck_exit_counter.counter++;
	pthread_mutex_unlock(&truck_exit_counter.mutex);
	sem_wait(&truck_exit);
	printf("TRUCK:      	    Truck with threadID %u is about to exit\n", threadID);
	pthread_mutex_lock(&truck_exit_counter.mutex);
	truck_exit_counter.counter--;
	pthread_mutex_unlock(&truck_exit_counter.mutex);
	
	sem_post(&truck_end);
}

void* runCaptain(void* arg)
{
    printf("CAPTAIN:  	    Captain thread started\n");
	int counter = 0;
		while(1)
		{
			pthread_mutex_lock(&car_queue_counter.mutex);
			pthread_mutex_lock(&truck_queue_counter.mutex);
			
			counter = car_queue_counter.counter + truck_queue_counter.counter;
			
			pthread_mutex_unlock(&car_queue_counter.mutex);
			pthread_mutex_unlock(&truck_queue_counter.mutex);
		
			if(counter >= QUEUE_MAX)
			{
				break;
			}
		}
		
		for (int i = 0; i<LOAD_MAX; i++)
		{
			int remaining_vehicle_slots = SPOTS_ON_FERRY;
			int remaining_truck_slots = 2;
			int new_trucks;
			int car_counter;
			int truck_counter;
			int selected_trucks_loading = 0;
			int selected_cars_loading = 0;
			int remaining_trucks_unloading = 0;
			int remaining_cars_undloading = 0;
			
			printf("\n");
			printf("CAPTAINCAPTAIN:		Arrived at loading dock\n");
			printf("CAPTAINCAPTAIN:		loop started %d\n",i+1);
			
			do
			{
				pthread_mutex_lock(&car_queue_counter.mutex);
				pthread_mutex_lock(&truck_queue_counter.mutex);
				
				car_counter = car_queue_counter.counter;
				truck_counter = truck_queue_counter.counter;
				
				pthread_mutex_unlock(&car_queue_counter.mutex);
				pthread_mutex_unlock(&truck_queue_counter.mutex);
				
				if ((remaining_truck_slots != 0) && (truck_counter != 0)){
					selected_trucks_loading = func_min(truck_counter, remaining_truck_slots);
					remaining_truck_slots -= selected_trucks_loading;
					
					for(int t=0;t<selected_trucks_loading;t++)
					{
						printf("CAPTAIN:	Truck selected for loading\n");
						sem_postChecked(&truck_queue);
						sem_waitChecked(&truck_loaded);
						printf("CAPTAIN:	Captain knows truck is loaded\n");
					}		
				
					remaining_vehicle_slots -= 2 * selected_trucks_loading;	
					remaining_trucks_unloading += selected_trucks_loading;	
				}

				selected_cars_loading = func_min(car_counter, remaining_vehicle_slots);		
				remaining_vehicle_slots -= selected_cars_loading;
				remaining_cars_undloading += selected_cars_loading;
				
				for(int c=0;c<selected_cars_loading;c++)
				{
					printf("CAPTAIN:	Car selected for loading\n");
					sem_postChecked(&car_queue);
					sem_waitChecked(&car_loaded);
					printf("CAPTAIN:	Captain knows car is loaded\n");
				}
				
			
			}while(remaining_vehicle_slots != 0);
			printf("CAPTAIN:	Ferry is full, starting to sail\n");
			printf("\n");

			printf("CAPTAIN:	Ferry has reached destination port %d\n",i+1);
			
			int rc = remaining_cars_undloading;
			int rt = remaining_trucks_unloading;
			
			do{
				pthread_mutex_lock(&car_sailing_counter.mutex);
				pthread_mutex_lock(&truck_sailing_counter.mutex);
				
				car_counter = car_sailing_counter.counter;
				truck_counter = truck_sailing_counter.counter;
				
				pthread_mutex_unlock(&car_sailing_counter.mutex);
				pthread_mutex_unlock(&truck_sailing_counter.mutex);
				
				rt -= truck_counter;
				
				for(int t=0;t<truck_counter;t++)
					{
						printf("CAPTAIN:	Captain knows a truck has unloaded from the ferry\n");
						sem_postChecked(&truck_sailing);
						sem_waitChecked(&truck_unloaded);
						printf("CAPTAIN:	Captain sees a truck leaving ferry terminal\n");
					}
				
				rc -= car_counter;
				for(int c=0;c<car_counter;c++)
				{
					printf("CAPTAIN:	Captain knows a car has unloaded from the ferry\n");
					sem_postChecked(&car_sailing);
					sem_waitChecked(&car_unloaded);
					printf("CAPTAIN:	Captain sees a car leaving ferry terminal\n");
				}
				
			}while((rc != 0) && (rt != 0) );	
			
			rc = remaining_cars_undloading;
			rt = remaining_trucks_unloading;
			do{
				pthread_mutex_lock(&car_exit_counter.mutex);
				pthread_mutex_lock(&truck_exit_counter.mutex);
				
				car_counter = car_exit_counter.counter;
				truck_counter = truck_exit_counter.counter;
				
				pthread_mutex_unlock(&car_exit_counter.mutex);
				pthread_mutex_unlock(&truck_exit_counter.mutex);
				
				rt -= truck_counter;
				
				for(int t=0;t<truck_counter;t++)
				{
					sem_post(&truck_exit);
					sem_wait(&truck_end);
				}
				
				rc -= car_counter;
				
				for(int c=0;c<car_counter;c++)
				{
					sem_post(&car_exit);
					sem_wait(&car_end);
				}
				
			}while((rc != 0) && (rt != 0));	
				
		printf("CAPTAIN: 	Unloading complete\n");
		printf("\n");
		}
		
	exit(0);
}

void* createVehicle(void* arg)	/*createVehicle thread*/
{
	struct arg_struct *args = (struct arg_struct *)arg;
	int probability_vehicle_truck= args->probability_vehicle_truck;
	int max_length_interval = args->max_length_interval;
	int seed = args->seed;
	struct timeval startTime;

	printf("CREATEVEHICLE:      Vehicle creation thread has been started\n");
	srand(seed);
	int next_arrival_time=0;
	int elapsed=0;
	gettimeofday(&startTime, NULL);

	elapsed = timeChange(startTime);

	//When the elapsed time exceeds the next arrival time completes steps 3 to 8
	while( elapsed < next_arrival_time){
		//calculate elapsed time
		elapsed = timeChange(startTime);
	};
	
	printf("CREATEVEHICLE:      Elapsed time %d msec\n", elapsed);
	//Determine when the next vehicle should arrive. 1000<= wait till next vehicle <= k
	next_arrival_time = ( rand()% (max_length_interval-1000)+ 1001) + elapsed;
	printf("CREATEVEHICLE:      Next arrival time %d msec\n", next_arrival_time);
	
	while(1){
		//if random integer <= probability then the vehicle is a truck
		if( rand()%101 <= probability_vehicle_truck)	//case : truck
		{
			elapsed = timeChange(startTime);

			//When the elapsed time exceeds the next arrival time completes steps 3 to 8
			while( elapsed < next_arrival_time){
				elapsed = timeChange(startTime);
			};

			printf("CREATEVEHICLE:      Elapsed time %d msec\n", elapsed);		

			pthread_t create_truck;	//Create a thread to create_truck
			pthread_create(&create_truck, NULL, runTruck, NULL);
			printf("CREATEVEHICLE:      Created a truck thread\n");

			//Determine when the next vehicle should arrive. 1000<= wait till next vehicle <= k
			next_arrival_time = ( rand()% (max_length_interval-1000)+ 1001) + elapsed;
			printf("CREATEVEHICLE:      Next arrival time %d msec\n", next_arrival_time);

				//pthread_join(create_truck, NULL);
		}

		else 		//case: car
		{
			elapsed = timeChange(startTime);

		    //When the elapsed time exceeds the next arrival time completes steps 3 to 8
		    while( elapsed < next_arrival_time){
				elapsed = timeChange(startTime);
		    };

			printf("CREATEVEHICLE:      Elapsed time %d msec\n", elapsed);
			pthread_t create_car;	//Create a thread to create_truck
			pthread_create(&create_car, NULL, runCar, NULL);
			printf("CREATEVEHICLE:      Created a car thread\n");

			//Determine when the next vehicle should arrive. 1000<= wait till next vehicle <= k
			next_arrival_time = ( rand()% (max_length_interval-1000)+ 1001) + elapsed;
			printf("CREATEVEHICLE:      Next arrival time %d msec\n", next_arrival_time);

		}
		
	}
	
	printf("CREATCREATCREATVEHICLE: Exiting\n");
}

int timeChange( const struct timeval startTime )
{
	struct timeval nowTime;
	long int elapsed;
	int elapsedTime;

	gettimeofday(&nowTime, NULL);
	elapsed = (nowTime.tv_sec - startTime.tv_sec) * 1000000 + (nowTime.tv_usec - startTime.tv_usec);
	elapsedTime = elapsed / 1000;
	return elapsedTime;
}

int sem_waitChecked(sem_t *semaphoreID){
	int returnValue;
	returnValue = sem_wait(semaphoreID);
	if(returnValue == -1){
		printf("sem_wait failed: exit(0)");
		exit(0);
	}
	return returnValue;
}

int sem_postChecked(sem_t *semaphoreID){
	int returnValue;
	returnValue = sem_post(semaphoreID);
	if(returnValue == -1)
	{
		printf("sem_post failed: exit(0)");
		exit(0);
	}
	return returnValue;
}

int sem_initChecked(sem_t *semaphoreID, int pshared, unsigned int value){
	int returnValue;
	returnValue = sem_init(semaphoreID, pshared, value);
	if(returnValue < 0){
		printf("sem_init failed: exit(0)");
		exit(0);
	}
	return returnValue;
}

int sem_destroyChecked(sem_t *semaphoreID){
	int returnValue;
	returnValue = sem_destroy(semaphoreID);
	if(returnValue < 0){
		printf("sem_destroy failed :exit(0)");
		exit(0);
	}
	return returnValue;
}

int pthread_mutex_initChecked(pthread_mutex_t *mutexID, const pthread_mutexattr_t *attrib){
	int returnValue;
	returnValue = pthread_mutex_init(mutexID, attrib);
	if(returnValue < 0){
		printf("pthread_mutex_init failed : exit(0)");
		exit(0);
	}
	return returnValue;
}

int pthread_mutex_destroyChecked(pthread_mutex_t *mutexID){
	int returnValue;
	returnValue = pthread_mutex_destroy(mutexID);
	if (returnValue < 0){
		printf("pthread_mutex_destroy failed: exit(0)");
		exit(0);
	}
	return returnValue;
}