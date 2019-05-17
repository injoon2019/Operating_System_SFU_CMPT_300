/* File name: candykids.c
 * Description: Main application holding factory thread, kid thread, and main() function.
 *
 * Student Name: Jin Hyun (Sam) Kim
 * Student ID: jhk29
 * Student # 301299137
 *
 * Date Last Modified: Feb 26th 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "bbuff.h"
#include "stats.h"

_Bool stop_thread = false;

typedef struct{
	int factory_number;		// Tracks which factory thread produced the candy item.
	double creation_ts_ms;	// tracks when the item was created.
} candy_t;

double current_time_in_ms(void){	// Get the current number of ms
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}


/* ---------------------------------------------------------------------------------------- */
/* Description: Each candy factory should:
 * 				(i) Loop until main() signals to exit
 *					- Pick a # of seconds which it will (later) wait. The number is randomly selected
 *					  between 0 to 3 (inclusive)
 *					- Print a message such as "Factory 0 ships candy & waits 2s"
 *					- Dynamically allocate a new candy item and populate its fields.
 *					- Add the candy item to the bounded buffer.
 *					- Sleep for number of seconds identified in the first step.
 *				(ii) When the thread finishes, print a message such as the following: 
 *								"Candy-Factory 0 done" (for thread 0)
 */
void* CandyFactoryThread(void* arg){
	int thread_number = *(int *) arg;
	int rand_time;

	while (!stop_thread){
		
		rand_time = (rand() % 4);
		
		printf("Factory %d ships candy & waits %ds\n", thread_number, rand_time);
		
		candy_t *candy= malloc(sizeof(candy_t));

		candy->factory_number= thread_number;
		candy->creation_ts_ms= current_time_in_ms();

		bbuff_blocking_insert(candy);
		stats_record_produced(thread_number);
		sleep(rand_time);
	}
	printf("Candy-Factory %d done\n", thread_number);
	pthread_exit(NULL);
}

/* Description: Each kid thread should loop forever and during each iteration, do the following:
 * 				(i) Extract a candy item from the bounded buffer; this will block until there is a candy
 *					item to extract.
 *				(ii) Process the item. Initially you may want to printf() it to the screen; in the next
 *					 section, you must add a statistics module that will track what candies have been eaten.
 *				(iii) Sleep for either 0 or 1 seconds (randomly selected.)
 */
void* kidThread(void* arg){
	
	do{
		candy_t *candy= (candy_t*)bbuff_blocking_extract();
		int rand_time = (rand() % 2);
		// int kid_number= *(int *) arg;	

		// printf("Kid %d eats candy and waits for %d seconds\n", kid_number, rand_time);

		int factory_number= candy->factory_number;
		double delay = current_time_in_ms() - (candy->creation_ts_ms);
		stats_record_consumed(factory_number,delay);

		sleep(rand_time);

	}while (true);

	pthread_exit(NULL);
}

/* ---------------------------------------------------------------------------------------- */

int main(int argc, char *argv[]){

	// 1. Extract arguments
	if (argc < 4){
		printf("ERROR: 3 arguments must be entered.\n");
		return 1;
	}

	int factories = atoi(argv[1]);
	int kids = atoi(argv[2]);
	int time = atoi(argv[3]);

	if (factories <= 0 || kids <= 0 || time <= 0){
		printf("ERROR: All arguments must be greater than 0.\n");
		return 1;
	}

	// 2. Initialize modules
	bbuff_init();
	stats_init(factories);

	
	// 3. Launch candy-factory threads
	pthread_t producers[factories];
	for (int i=0; i < factories ; i++){
		pthread_create(&(producers[i]),NULL,CandyFactoryThread,&i);
	}
	
	// 4. Launch kid threads
	pthread_t consumers[kids];
	for (int i=0; i< kids; i++){
		pthread_create(&(consumers[i]),NULL,kidThread,&i);
	}

	// 5. Wait for requested time
	for (int i=0 ; i < time ; i++){
		printf("Time %ds\n", i);
		sleep(1);
	}

	// 6. Stop candy-factory threads
	stop_thread=true;
	for (int i=0; i< factories; i++){
		pthread_join(producers[i],NULL);
	}
	
	// 7. Wait until no more candy
	while (!bbuff_is_empty()){
		printf("Waiting for all candies to be consumed.\n");
		sleep(1);
	}
	
	// 8. Stop kid threads
	for (int i=0; i < kids; i++){
		pthread_cancel(consumers[i]);
		pthread_join(consumers[i],NULL);
	}

	// 9. Print statistics
	stats_display();

	// 10. Cleanup any allocated memory
	stats_cleanup();
	return 0;
}






