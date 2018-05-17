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
    char processId[NB_PROCESSES][MESSLENGTH];
    int alloc[3][NB_PROCESSES];
    int req[3][NB_PROCESSES];
};


/* Process Input File */
void readDataFromFile(struct Data *data)
{
    FILE * inputFile;
    inputFile = fopen(INPUT_FILE, "r");

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
        matched = fscanf(inputFile, "%s %d %d %d %d %d %d %d %d %d",
            procId,
            &allocA, &allocB, &allocC,
            &reqA, &reqB, &reqC,
            &availA, &availB, &availC);

        if(matched == 7 || matched == 10)
        {
            //printf("size of procId: %d\n")
            //procId[2] = '0/';
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

    //exit(0);
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

    for(i = 0; i < 3; i++)
    {
        work[i] = data->avail[i];
        prevWork[i] = work[i];
    }

    for (i = 0; i < NB_PROCESSES; i++)
        finished[i] = 0;

	for (i = 0; allFinished == 0; i++)
	{

		/* Reset loop through processes */
		if (i >= NB_PROCESSES)
		{
			if (work[a] == prevWork[a] && work[b] == prevWork[b] && work[c] == prevWork[c])
			{
				//allFinished = 0;
				for(j = 0; j < NB_PROCESSES; j++)
				{
					if(j==0)
						allFinished = finished[j];
					else
						allFinished &= finished[j];
				}

				if (allFinished == 0)
				{
                    sprintf(temp,", DEADLOCKED\n");
                    strcat(result1,temp);
                    sprintf(temp, "DEADLOCKED\n");
                    strcat(result2,temp);
				}

				break;
			}
			//printf("No Potential Deadlocks\n");
			i = 0;
			loop++;
			deadlock = 0;
			//sprintf(temp,"Loop %d: \n", loopNb);
			//strcat(deadlocks,temp);
            //printf("A = %d:%d  B = %d:%d  C = %d:%d\n",prevWork[a],work[a],prevWork[b],work[b],prevWork[c],work[c]);
			prevWork[a] = work[a];
			prevWork[b] = work[b];
			prevWork[c] = work[c];


			//sleep(1);
		}

		/* If not finished and request <= availabile */

		if(finished[i] == 0)
		{
            if(data->req[a][i] <= work[a]
            && data->req[b][i] <= work[b]
            && data->req[c][i] <= work[c])
            {
                work[a] += data->alloc[a][i];
				work[b] += data->alloc[b][i];
				work[c] += data->alloc[c][i];
				finished[i] = 1;
				if (first == 1)
                {
                    sprintf(temp,"%s", data->processId[i]);
                    first = 0;
                }

                else
                    sprintf(temp,", %s", data->processId[i]);
				strcat(result1,temp);
            }
            else
            {
                if(deadlock == 0)
                {
                    deadlock = 1;
                    sprintf(temp, "Loop %d:\n",loop);
                    strcat(result2,temp);
                }
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

                strcat(result2,temp);

                //printf("%s",temp);

                ///* Signal finished writing with SIGUSR1 */
                // if(signal(SIGUSR1, signalFunction) == SIG_ERR) {
                    // exit(0);
                // }


            }

		}

        //printf("%d\n",i);
        //sleep(1);

	}
	/* Append data to file */
	//if(allFinished)
        appendToFile(OUTPUT_FILE,result1);
    //else
        appendToFile(OUTPUT_FILE,result2);

    printf("Result 1:\n%s\n\n",result1);
    printf("Result 2:\n%s\n\n",result2);

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

//    int i;
//    for (i = 0;i < NB_PROCESSES; i++){
//        printf("%s %d %d %d   %d %d %d\n",
//        data.processId[i],
//        data.alloc[0][i],
//        data.alloc[1][i],
//        data.alloc[2][i],
//        data.req[0][i],
//        data.req[1][i],
//        data.req[2][i]);
//    }



    detectDeadlocks(&data);
//	FILE *readFile;
//	FILE *writeFile;
//	char line[MESSLENGTH];
//	int i;
//	int processId[MESSLENGTH];
//	int processNb = 9;






	/* Loop through processes forever */
	//int loopNb = 1;

	//char temp[MESSLENGTH];
	//sprintf(temp, "Loop %d: \n", loopNb);

	//strcat(deadlocks,temp);





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

