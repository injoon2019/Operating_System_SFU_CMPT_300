/* File name: stats.c
 * Description: keeps track of the following:
 *					(i) The number of candies each factory creates.
 *					(ii) The number of candies that were consumed from each factory.
 *					(iii) For each factory, the min, max, and the average delays for how long it
 *						  took  the moment the candy was produced until consumed.
 *
 * Student Name: Jin Hyun (Sam) Kim
 * Student ID: jhk29
 * Student # 301299137
 *
 * Date Last Modified: Feb 18th 2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <semaphore.h>
#include <float.h>
#include "stats.h"

typedef struct{
	int factory_number;
	int candies_made;
	int candies_eaten;
	double min_delay;
	double avg_delay;
	double max_delay;
	double total_delay;
} stats_t;

stats_t *stats;
int factoryCount;

void stats_init(int num_producers){
	factoryCount= num_producers;
	stats= malloc(sizeof(stats_t)*num_producers);
	for (int i= 0; i < num_producers; i++){
		stats[i].factory_number= i;
		stats[i].candies_made= 0;
		stats[i].candies_eaten= 0;
		stats[i].max_delay= 0;
		stats[i].avg_delay= 0;
		stats[i].min_delay= 9999999999;
		stats[i].total_delay= 0;
	}
}

void stats_cleanup(void){
	free(stats);
	stats= NULL;
}

void stats_record_produced(int factory_number){
	stats[factory_number].candies_made++;
}

void stats_record_consumed(int factory_number, double delay_in_ms){

	stats[factory_number].candies_eaten++;
	stats[factory_number].total_delay += delay_in_ms;

	if (stats[factory_number].min_delay >= delay_in_ms){
		stats[factory_number].min_delay = delay_in_ms;
	}
	else if (stats[factory_number].max_delay <= delay_in_ms){
		stats[factory_number].max_delay = delay_in_ms;
	}
	
	stats[factory_number].avg_delay= (stats[factory_number].total_delay) / stats[factory_number].candies_eaten;
}

void stats_display(void){
	printf("Statistics:\n");
	printf("%8s%10s%10s%20s%20s%20s\n", "Factory #", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
	for (int i = 0 ; i < factoryCount; i++){
		if (stats[i].candies_made != stats[i].candies_eaten){
			printf("ERROR: Mismatch between number made and eaten.\n");
			continue;
		}
		printf("%8d%10d%10d%20.5f%20.5f%20.5f\n", stats[i].factory_number, stats[i].candies_made, stats[i].candies_eaten, stats[i].min_delay, stats[i].avg_delay, stats[i].max_delay);
	}
}

