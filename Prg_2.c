/* 
 * Subject Realtime Operating Systems 48450
 * Project: Assignment 3 - Prg_2
 * Name: Phong Au 
 * Student Number: 10692820
 * Compile Command: gcc -o Prg_1 Prg_1.c -pthread
 * Run Command : ./Prg_2
 */
 
  /* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

/* Defines */
#define INPUT_FILE "Topic2_Prg_2.txt"
#define OUTPUT_FILE "output_topic2.txt"
#define MESSLENGTH 1024


/* Signal Function*/
void ouch(int sig)
{
	if(sig == SIGUSR1)
		printf("OUCH! - I got signal %d\n", sig);
	else
		prinft("%d basjdbkabsd\n");
}

/* Function appendToFile
 * Appends data to a file
 */ 
void appendToFile(char filename[1024], char text[MESSLENGTH])
{
	/* Declare and initialise local Variables */ 
	FILE *dataFile;
	
	/* Open file for appending */ 
    if ((dataFile = fopen(filename, "a")) == NULL)
    {
        perror("fopen");
		exit(1);
	}
        
    /* Write to file */ 
	if(fputs(text,dataFile) == EOF)
    {
        perror("fputs");
		exit(2);
    }
    
    /* Close file */ 
    if(fclose(dataFile) != 0)
    {
        perror("fclose");
		exit(3);
	}
}

/* Function main */
int main(void)
{
	/* Declare and initialise variables */
	
	FILE *readFile;
	char line[MESSLENGTH];
	int i;
	int processId[MESSLENGTH];
	int processNb = 0;
	
	int dataType;
	char data[MESSLENGTH];
	
	/* Open Input File */
	//if((readFile = fopen(INPUT_FILE, "r")) != NULL)
	//{
	//	perror("fopen");
	//	exit(1);
	//}
	
	
	
	/* Loop while not end of file */
	// while(fgets(line,sizeof line, dataFile) != NULL)
	// {
		// /* Check for valid line of data */
		// if ((line[0] == 'P') && (isdigit(line[1]) == 0))
		// {
			
			// strcat(line[0], process);
			// strcat(line[1], process);
			
			// dataType = 0;	// Process Id
			
			// /* Loop through each char in the line */
			// for(i = 2 ;line[i] != '\0'; i++;)
			// {
				// /* Check  */
				// if(line[])
				
				
			// }
		// }
		
		// sem_wait(&(threadData->write));					// Wait for semaphore
		// write(threadData->fd[1], line, strlen(line));	// Write line to pipe
		// sem_post(&(threadData->read));					// Signal threadB
	// }
	
	//fclose(readFile);
	
	
	/* Signal stuff */
	
	
	 struct sigaction act;

    act.sa_handler = ouch; /* Pointer to function. */

    if(sigemptyset(&act.sa_mask) < 0)
    {
		printf("Error: sigemptyset() failed, terminating!\n");
		exit(1);
    }
    act.sa_flags = 0;



    if(sigaction(SIGINT, &act, 0) < 0)
    {
		printf("Error: sigaction() failed, terminating!\n");
        exit(2);
    }
	
	
	while(1) 
    {
       printf("Waiting to catch signal. PID: %d!\n",getpid());
	   
       sleep(1);
    }
	
	/* Clear output file if it exists */
	//FILE *dataFile;
    //if ((dataFile = fopen(OUTPUT_FILE, "w")) != NULL)
	//	fclose(dataFile);
	
	
	
	return 0;
}

