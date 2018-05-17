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
#define NB_PROCESSES 9

struct Data{
    int avail[3];
    char processId[NB_PROCESSES];
    int alloc[3][NB_PROCESSES];
    int req[3][NB_PROCESSES];
};


/* Process Input File */
void readDataFromFile(struct Data *data)
{
    FILE * inputFile;
    inputFile = fopen(INPUT_FILE, "r");

    char rubbishData[MESSLENGTH];
    if(inputFile == NULL)
    {
        perror("fopen");
        exit(0);
    }
    char procId[MESSLENGTH];
    int allocA, allocB, allocC, reqA,reqB,reqC, availA, availB,availC;
    char processId[MESSLENGTH];
    int matched;
    int i = 0;

    while (matched != EOF)
    {
        matched = fscanf(inputFile, "%s %d %d %d %d %d %d %d %d %d", procId, &allocA, &allocB, &allocC, &reqA, &reqB, &reqC, &availA, &availB, &availC);
        //printf("I: %i Matched: %d\n",i,matched);
        if(matched == 7 || matched == 10)
        {

            strcpy(&data->processId[i],procId);
            data->alloc[0][i] = allocA;
            data->alloc[1][i] = allocB;
            data->alloc[2][i] = allocC;
            data->req[0][i] = reqA;
            data->req[1][i] = reqB;
            data->req[2][i] = reqC;
            if(matched == 10)
            {

                data->avail[0] = availA;
                data->avail[1] = availB;
                data->avail[2] = availC;
                printf("Available: %d %d %d\n", data->avail[0],data->avail[1],data->avail[2]);
            }
            printf("%s %d %d %d   %d %d %d\n", &data->processId[i], data->alloc[0][i], data->alloc[1][i], data->alloc[2][i], data->req[0][i], data->req[1][i], data->req[2][i]);
            i++;
        }
    }
}

/* Signal Function*/
void ouch(int sig)
{
	if(sig == SIGUSR1)
		printf("OUCH! - I got signal %d\n", sig);
	else
		printf("basjdbkabsd\n");

    exit(0);
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

    /* Clear output file if it exists */
    FILE *dataFile;
    if ((dataFile = fopen(OUTPUT_FILE, "w")) != NULL)
        fclose(dataFile);

	/* Declare and initialise variables */
    struct Data data;

    readDataFromFile(&data);

	FILE *readFile;
	FILE *writeFile;
	char line[MESSLENGTH];
	int i;
	int processId[MESSLENGTH];
	int processNb = 9;

    int a = 0, b = 1, c = 2;
	int finished[NB_PROCESSES];
    int work[3]; //= {0,1,2};
    int prevWork[3];// = {0,1,2};
    for(i = 0; i < 3; i++)
    {
        work[i] = data.avail[i];
        prevWork[i] = work[i];
    }


	/* ?Preparing Signal */
//	if(signal(SIGUSR1, ouch) == SIG_ERR) {
//		perror("signal");
//		exit(0);
//	}

	/* Loop through processes forever */
	int loopNb = 1;

	char temp[MESSLENGTH];
	sprintf(temp, "Loop %d: \n", loopNb);

	//strcat(deadlocks,temp);

    int allFinished;
	int j;
    allFinished = 0;

	for (i = 0;allFinished == 0; i++)
	{
		/* Reset loop through processes */
		if (i >= NB_PROCESSES)
		{
			if (work[a] == prevWork[a] && work[b] == prevWork[b] && work[c] == prevWork[c])
			{
				allFinished = 0;
				for(j = 0; j < NB_PROCESSES; j++)
				{
					if(j==0)
						allFinished = finished[j];
					else
						allFinished &= finished[0];
				}
				if (allFinished == 1)
						appendToFile(OUTPUT_FILE,"No Potential Deadlocks.");
				//close(file)

				//kill(getpid(),SIGUSR1);
			}
			printf("No Potential Deadlocks\n");
			i = 0;
			loopNb++;
			sprintf(temp,"Loop %d: \n", loopNb);
			//strcat(deadlocks,temp);

			prevWork[a] = work[a];
			prevWork[b] = work[b];
			prevWork[c] = work[c];

			printf("Finished Loop %d A%d%d B%d%d C%d%d\n",loopNb,prevWork[a],work[a],prevWork[b],work[b],prevWork[c],work[c]);
			sleep(1);
		}

		/* If not finished and request <= availabile */
		if (data.req[a][i] <= work[a] && data.req[b][i] <= work[b] && data.req[c][i] <= work[c])
		{
			/* If not finished */
			if (finished[i] == 0)
			{
				work[a] += data.alloc[a][i];
				work[b] += data.alloc[b][i];
				work[c] += data.alloc[c][i];
				finished[i] = 1;
			}
		}
		else{

			/* Create data to write to output file */
			sprintf(line,"Potential Deadlock: %s: Requested: %d %d %d  Allocated: %d %d %d  Available: %d %d %d\n", &data.processId[i], data.req[a][i], data.req[b][i], data.req[c][i], data.alloc[a][i], data.alloc[b][i], data.alloc[c][i], data.avail[a], data.avail[b], data.avail[c]);
			//printf("%s",line);
			//exit(1);
            //printf("%s",line);
			/* Append data to file */
			appendToFile(OUTPUT_FILE,line);

			/* Output to cosole */
			printf("%s",line);

			///* Signal finished writing with SIGUSR1 */
			// if(signal(SIGUSR1, signalFunction) == SIG_ERR) {
				// exit(0);
			// }

			//kill(getpid(),SIGUSR1);
		}
		i++;
	}



	/* Signal stuff */


	 //struct sigaction act;

    //act.sa_handler = ouch; /* Pointer to function. */

//    if(sigemptyset(&act.sa_mask) < 0)
//    {
//		printf("Error: sigemptyset() failed, terminating!\n");
//		exit(1);
//    }
//    act.sa_flags = 0;
//
//    if(sigaction(SIGINT, &act, 0) < 0)
//    {
//		printf("Error: sigaction() failed, terminating!\n");
//        exit(2);
//    }
//
//
//	while(1)
//    {
//       printf("Waiting to catch signal. PID: %d!\n",getpid());
//
//       sleep(1);
//    }





	return 0;
}

