/*
 * Subject Realtime Operating Systems 48450
 * Project: Assignment 3 - Prg_2
 * Name: Phong Au
 * Student Number: 10692820
 * Compile Command: gcc -o Prg_2 Prg_2.c -pthread
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

/* Struct */
struct Data{
    int avail[3];
    char processId[NB_PROCESSES][MESSLENGTH];
    int alloc[3][NB_PROCESSES];
    int req[3][NB_PROCESSES];
};

/* Process Input File */
void readDataFromFile(struct Data *data)
{
    /* Declare and Initialise Variables */
    FILE * inputFile;
    char procId[MESSLENGTH];
    int allocA, allocB, allocC, reqA,reqB,reqC, availA, availB,availC;
    char processId[MESSLENGTH];
    int matched;
    int i = 0;

    /* Open File */
    inputFile = fopen(INPUT_FILE, "r");
    if(inputFile == NULL)
    {
        perror("fopen");
        exit(0);
    }


    /* While not end of file */
    while (matched != EOF)
    {
        /* Read in data */
        matched = fscanf(inputFile,
            "%s %d %d %d %d %d %d %d %d %d",
            procId,
            &allocA, &allocB, &allocC,
            &reqA, &reqB, &reqC,
            &availA, &availB, &availC
        );

        /* If found valid matches of data */
        if(matched == 7 || matched == 10)
        {
            strcpy(data->processId[i],procId);
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

            printf("%s %d %d %d   %d %d %d\n", data->processId[i], data->alloc[0][i], data->alloc[1][i], data->alloc[2][i], data->req[0][i], data->req[1][i], data->req[2][i]);
            i++;
        }
    }
}


/* Signal Function*/
void signalFunction(int sig)
{
	if(sig == SIGUSR1 || sig == SIGUSR2)
		printf("Finished Writing to file\n");
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

/* Detect Deadlocks */
void detectDeadlocks(struct Data *data)
{
    /* Declare and initialise data */
    int a = 0, b = 1, c = 2;
	int finished[NB_PROCESSES];
    int work[3];
    int prevWork[3];
    int allFinished = 0;
	int j;
    int i;
    char temp[MESSLENGTH];
    char result1[MESSLENGTH] = "Sequence: ";
    char result2[MESSLENGTH]  = "Potential Deadlocks:\n";
    int loop = 1;
    int deadlock = 0;
    int first = 1;

    /* Set starting work */
    for(i = 0; i < 3; i++)
    {
        work[i] = data->avail[i];
        prevWork[i] = work[i];
    }

    /* Set Finished to false */
    for (i = 0; i < NB_PROCESSES; i++)
        finished[i] = 0;

    /* Start Loop */
	for (i = 0; allFinished == 0; i++)
	{

		/* Reset loop through processes */
		if (i >= NB_PROCESSES)
		{
            /* If work from previous loop the same as current work */
			if (work[a] == prevWork[a] && work[b] == prevWork[b] && work[c] == prevWork[c])
			{
                /* Check finished or not */
				for(j = 0; j < NB_PROCESSES; j++)
				{
					if(j==0)
						allFinished = finished[j];
					else
						allFinished &= finished[j];
				}

                /* Check if deadlocked */
				if (allFinished == 0)
				{
                    sprintf(temp,", DEADLOCKED\n");
                    strcat(result1,temp);
                    sprintf(temp, "DEADLOCKED\n");
                    strcat(result2,temp);
				}
				break;
			}

			/* Increment Loop count and reset data */
			loop++;
			i = 0;
			deadlock = 0;

			/* Update previous work */
			prevWork[a] = work[a];
			prevWork[b] = work[b];
			prevWork[c] = work[c];
		}

		/* If not finished and request <= availabile */
		if(finished[i] == 0)
		{
            /* Check request is less than work */
            if(data->req[a][i] <= work[a]
            && data->req[b][i] <= work[b]
            && data->req[c][i] <= work[c])
            {
                /* Update work */
                work[a] += data->alloc[a][i];
				work[b] += data->alloc[b][i];
				work[c] += data->alloc[c][i];

                /* Set process to finished */
				finished[i] = 1;

                /* Just checking to put ',' or not */
				if (first == 1)
                    sprintf(temp,"%s", data->processId[i]);
                else
                    sprintf(temp,", %s", data->processId[i]);
                first = 0;

                /* Append line to result 1 */
				strcat(result1,temp);
            }
            else
            {
                /* If no dead lock was found before this potential deadlock */
                if(deadlock == 0)
                {
                    /* Set deadlock to true */
                    deadlock = 1;

                    /* Loop Number */
                    sprintf(temp, "Loop %d:\n",loop);
                    strcat(result2,temp);
                }

                /* Result of process */
                sprintf(temp, "%s\tRequested: %d %d %d\tAllocated: %d %d %d\t Work: %d %d %d\n",
                data->processId[i],
                data->req[a][i],
                data->req[b][i],
                data->req[c][i],
                data->alloc[a][i],
                data->alloc[b][i],
                data->alloc[c][i],
                work[a],
                work[b],
                work[c]);

                /* Append result to result2 for writing to file later */
                strcat(result2,temp);
            }
		}
	}

	/* Append data to file */
	//if(allFinished)
        appendToFile(OUTPUT_FILE,result1);
    //else
        appendToFile(OUTPUT_FILE,result2);

    printf("Result 1:\n%s\n\n",result1);
    printf("Result 2:\n%s\n\n",result2);

    /* Signal finished writing with SIGUSR1 */
    kill(getpid(),SIGUSR1);
}


/* Function main */
int main(void)
{
    /* Clear output file if it exists */
    FILE *dataFile;
    if ((dataFile = fopen(OUTPUT_FILE, "w")) != NULL)
        fclose(dataFile);

    /* ?Preparing Signal */
	if(signal(SIGUSR1, signalFunction) == SIG_ERR) {
		perror("signal");
		exit(0);
	}

	/* Declare and initialise variables */
    struct Data data;

    readDataFromFile(&data);
    printf("\n");
    detectDeadlocks(&data);

	return 0;
}

