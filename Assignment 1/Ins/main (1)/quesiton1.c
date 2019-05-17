
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void function_grandChildren(int seed, int parentID)
{
	srand(seed);    //2_b
	int random = (rand() % 3) + 1;  //2_d

	pid_t pid;
	pid_t childID = getpid();
	pid_t grandchildID;

	printf("I am a new child, my process ID is %d, my seed id %d\n", childID, seed);    //2_c
	printf("I am child %d, I will have %d children\n", childID, random);    //2_e



	for (int i = 0; i < random; i++)
	{

		printf("I am child %d, I am about to create a child\n", childID);       //2_f
		pid = fork();                                                   //2_g

		if (pid == 0)
		{
			printf("I am grandchild %d, My grandparent is %d, My Parent is %d\n", getpid(), parentID, childID);
			int sleeper = (rand() % 10) + 5;
			sleep(sleeper);
			printf("I am grandchild %d with parent %d, I am about to terminate\n", getpid(), childID);
			exit(0);
		}
		else if (pid >0)
		{
			printf("I am child %d, I just created child\n", childID);       //2_h
			grandchildID = wait(NULL);
			printf("I am child %d. My child %d has been waited\n", childID, grandchildID);      //2_j
		}
		else
		{
			printf("\n\n");
			printf("Error in child");
			printf("\n\n");
		}
	}

	if (pid >0)
	{
		printf("I am the child %d I have %d children\n", childID, random);  //2_i
		printf("I am waiting for my children to terminate\n");          //2_i
	}

	printf("I am child %d, I am about to terminate\n", childID);        //2_k
	sleep(5);   //2_i
				//2_m
}

int main()
{
	int seed;
	printf("enter the seed for the parent process:  ");  //1_a
	scanf("%d", &seed); //1_a
	srand(seed);    //1_b
	int random = (rand() % 5) + 5;  //1_c 5<= number of process<=9

	pid_t pid;
	int parentID = (int)getpid();  //1_c
	pid_t childID;
	for (int i = 0; i < random; i++)
	{
		printf("\n\n");
		printf("My process ID is %d\n", parentID);  //1_d
		printf("%d is about to create a child\n", parentID); //1_e

		pid = fork();   //1_f
		if (pid == 0)   //child_process
		{
			function_grandChildren(seed + i, parentID);  //2_a fixed it from (seed, parentId) to (seed+i, parentID)
			exit(0);
		}
		else if (pid > 0)    //parent_process
		{

			printf("Parent %d has created a child with process ID %d\n", parentID, (int)pid);    //1_g
			printf("I am the parent, I am waiting for child %d to terminate\n", (int)pid);  //1_h_1

			childID = wait(NULL);
			printf("I am process %d. My child %d is dead\n", parentID, childID); //1_h_2
		}
		else
		{
			printf("\n\n");
			printf("Error in parents");
			printf("\n\n");
		}

	}

	printf("\n\n");
	printf("I am the parent, child %d has terminated\n", childID);  //1_h_3
	sleep(5);       //1_h_4

	return 0;   //1_h_5
}
