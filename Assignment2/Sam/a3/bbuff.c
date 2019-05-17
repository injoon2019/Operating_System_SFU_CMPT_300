/* File name: bbuff.c
 * Description: encapsulates access to the bounded buffer.
 *
 * Student Name: Jin Hyun (Sam) Kim
 * Student ID: jhk29
 * Student # 301299137
 *
 * Date Last Modified: Feb 18th 2019
 *
 * References: https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_mutex_lock.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "bbuff.h"

// semaphores
sem_t empty_buffs;
sem_t full_buffs;

// mutex
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;

void* bbuff[BUFFER_SIZE];
int candy_count;

void bbuff_init(void){
	sem_init(&empty_buffs,0,BUFFER_SIZE);
	sem_init(&full_buffs,0,0);
	candy_count=0;
}

void bbuff_blocking_insert(void* item){
	sem_wait(&empty_buffs);
	pthread_mutex_lock(&mutex);
	
	bbuff[candy_count++] = item;
	pthread_mutex_unlock(&mutex);
	
	sem_post(&full_buffs);
}

void* bbuff_blocking_extract(void){
	sem_wait(&full_buffs);
	pthread_mutex_lock(&mutex);
	
	void* candy = bbuff[candy_count-1];
	bbuff[candy_count--]= NULL;

	pthread_mutex_unlock(&mutex);
	sem_post(&empty_buffs);
	return candy;
}

_Bool bbuff_is_empty(void){
	if (candy_count==0) return true;
	else return false;
}

