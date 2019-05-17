#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int seed;
    printf("enter the seed for the parent process:  ");
    scanf("%d", &seed);
    srand(seed);
    int random = rand() % 5 + 5;

    pid_t pid;
    int parentID = (int) getpid();
    pid_t childID;
    for(int i=0; i < random; i++)
    {
        printf("\n\n");
        printf("My process ID is %d\n", parentID);
        printf("%d is about to create a child\n", parentID);

        pid = fork();
        if( pid== 0)
        {
            exit(0);
        }
        else if(pid > 0)
        {
            printf("Parent %d has created a child with process ID %d\n",parentID, (int)pid);
            printf("I am the parent, I am waiting for child %d to terminate\n", (int)pid);

            childID = wait(NULL);
            printf("I am process %d. My child %d is dead\n",parentID ,childID);
        }
        else
        {
            printf("Error");
        }

    }

    printf("\n\n");
    printf("I am the parent, child %d has terminated\n", childID);
    sleep(5);

    return 0;
}

