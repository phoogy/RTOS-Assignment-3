/* 
 * Subject Realtime Operating Systems 48450
 * Project: Assignment 3 - Prg_1
 * Name: Phong Au 
 * Student Number: 10692820
 * Compile Command: gcc -o Prg_1 Prg_1.c -pthread
 * Run Command : ./Prg_1
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

/* Defines */
#define NUMBER_OF_PROCESSES 7
#define PROCESS_ID {1,2,3,4,5,6,7}
#define PROCESS_ARRIVE {8,10,14,9,16,21,26}
#define PROCESS_BURST {10,3,7,5,4,6,2}
#define FIFONAME "/tmp/myfifo"
#define OUTPUT_FILE "output.txt"
#define MESSLENGTH 1024

/* ThreadData Struct */
struct ThreadDataA
{
    int nbProcesses;
	int processes[NUMBER_OF_PROCESSES];
    int arriveTime[NUMBER_OF_PROCESSES];
	int burstTime[NUMBER_OF_PROCESSES];
	int remainingTime[NUMBER_OF_PROCESSES];
	char fifoname[1024];
	sem_t *sem;
};

struct ThreadDataB
{
    char fifoname[1024];
	int messageLength;
	char outFileName[1024];
	sem_t *sem;
};

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

/* Function getSmallest
 * Returns -1 if no processes needs to do work otherwise
 * Returns the position of the process in the array with the smallest remaining time
 */
int getSmallest(int time, int nbProcesses, int * arrivalTime, int * remainingTime)
{
	/* Declare and initialise local Variables */ 
	int smallest = -1;
	int i;
	
	/* Loop through all processes */
	for (i = 0; i < nbProcesses; i++)
	{
		/* If the arrival time of process is less than the current time */
		if(arrivalTime[i] <= time)
		{
			/* If the remaining Time is greater than 0 */
			if (remainingTime[i] > 0)
			{
				/* if smallest == -1 set new smallest value. Else compare remainging times */
				if (smallest == -1)
					smallest = i;
				else if (remainingTime[i] < remainingTime[smallest])
					smallest = i;
			}
		}
	}

	return smallest;
}

/* Thread B
 * Reads from the fifo and writes to a file
 */
void *threadB(struct ThreadDataB *data)
{
	/* Declare and initialise local variables */
	int fifofd;
	char datafromFIFO[data->messageLength];
	
	/* Open Fifo for reading */
	if((fifofd = open(data->fifoname, O_RDONLY)) < 0)
    {
        perror("open");
		exit(2);
    }
    
    /* Read from fifo */
	int n = read(fifofd, datafromFIFO, data->messageLength);
	
	/* Loop until fifo has been closed */
	while(n>0)
	{
		/* Put a null char at the end */
		datafromFIFO[n] = '\0'; 
		
		/* Let thread other thread know that they can put more data into fifo */
		sem_post(data->sem);
	
		/* write data to file */
		appendToFile(data->outFileName,datafromFIFO);
		
		/* Read from fifo */
		n = read(fifofd, datafromFIFO, data->messageLength);
	}
	
	/* Close the fifo */
	close(fifofd);
	return 0;
}

/* Thread threadA 
 * Processes processes and calculates averages then writes to fifo
 */
void *threadA(struct ThreadDataA *data)
{
    /* Declare local variables */
	int time = 0;
	int completedProcesses = 0;
	int sumTurnaroundTime = 0;
	int sumWaitTime = 0;
	int fifofd;
	int smallest;
	int waitTime;
	int turnaroundTime;
	float averageWaitTime;
	float averageTurnaroundTime;
	char averageWaitTimeAsString[MESSLENGTH];
	char averageTurnaroundTimeAsString[MESSLENGTH];
	
    /* Loop until all the processes are complete */
	while (completedProcesses < data->nbProcesses)
	{
		/* Find the process with the smallest remaining time */
		smallest = getSmallest(time,data->nbProcesses, data->arriveTime, data->remainingTime);
		
		/* getSmallest returns -1 if no processes are waiting */
		if(smallest >= 0)
		{
			/* Decrease remaining time of process with smallest remaining */
			data->remainingTime[smallest]--;
		
			/* If this process has completed(remaining time is 0) */
			if (data->remainingTime[smallest] == 0)
			{
				/* Increase the number of completed processes */
				completedProcesses++;
			
				/* Calculate Turnaround Time */
				turnaroundTime = time - data->arriveTime[smallest];
			
				/* Calculate Wait Time */
				waitTime = turnaroundTime - data->burstTime[smallest];
			
				/* Add to sum */
				sumTurnaroundTime += turnaroundTime + 1;
				sumWaitTime += waitTime + 1;
			}	
		}
		
		/* Increase Time */
		time++;
	}
	
	
	
	/* Calculate Averages Time */
	averageWaitTime = ((sumWaitTime)*1.0/NUMBER_OF_PROCESSES);
	averageTurnaroundTime = ((sumTurnaroundTime)*1.0/NUMBER_OF_PROCESSES);

	// Convert float to string
	sprintf(averageWaitTimeAsString, "Average Wait Time: %f\n", averageWaitTime);
	sprintf(averageTurnaroundTimeAsString, "Average Turnaround Time: %f\n", averageTurnaroundTime);
	
	/* Open Fifo for writing */
	if((fifofd = open(data->fifoname, O_WRONLY)) < 0)
    {
        perror("open");
		exit(2);
    }
	
	/* Write to fifo */
	write(fifofd, &averageWaitTimeAsString, strlen(averageWaitTimeAsString));
	
	/* Wait for thread b to read before writing more to the fifo */
	sem_wait(data->sem);
	
	/* Write to fifo */
	write(fifofd, &averageTurnaroundTimeAsString, strlen(averageTurnaroundTimeAsString));

	/* Close the fifo */
	close(fifofd);
	return 0;
}

/* Function main */
int main(void)
{
	/* Declare and initialise variables */
	pthread_t tidA,tidB;
	char fifoname[] =  FIFONAME;
	sem_t sem;
	
	/* Clear output file if it exists */
	FILE *dataFile;
    if ((dataFile = fopen(OUTPUT_FILE, "w")) != NULL)
		fclose(dataFile);
	
	/* Initialise Semaphores */
	if (sem_init(&sem, 0, 0) < 0)
	{
		perror("sem_init");
		exit(1);
	}
	
	/* Make the fifo */
	if(mkfifo(fifoname, S_IRWXU) < 0)
    {
    	/* If failed to make fifo remove existing fifo */
		unlink(fifoname);
		
		/* Make the fifo again */
		if(mkfifo(fifoname, S_IRWXU) < 0)
		{		
			perror("mkfifo");
			exit(1);
		}
    }
	
	/* Set thread data */
	struct ThreadDataA dataA = {NUMBER_OF_PROCESSES, PROCESS_ID, PROCESS_ARRIVE, PROCESS_BURST, PROCESS_BURST, FIFONAME,&sem};
	struct ThreadDataB dataB = {FIFONAME, MESSLENGTH, OUTPUT_FILE, &sem};
	
	/* Create threads */
	if(pthread_create(&tidA, NULL, (void *)threadA, &dataA) != 0)
	{
		perror("pthread_create");
		exit(3);
	}
	if(pthread_create(&tidB, NULL, (void *)threadB, &dataB) != 0)
	{
		perror("pthread_create");
		exit(4);
	}
	
	/* Wait for threads to finish */
	pthread_join(tidA,NULL);
	pthread_join(tidB,NULL);
	
	/* Destroy fifo */
	if(unlink(FIFONAME) < 0)
    {
		perror("unlink");
		exit(5);
    }
	
	/* Destroy semaphore */
	if(sem_destroy(&sem) < 0)
    {
		perror("sem_destroy");
		exit(5);
    }
	
	return 0;
}

