#include "ipcd.h"
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define MAX_FIB_ARG			45
#define CHILD_COUNT 		5
#define MULTI_INPUT_VALUE 	30

int main(int argc, char *argv[]) {

	/* Parse argument n from the command line */
	int input;
	if (argc != 2) 
	{
		printf("Invalid argument count.\n"
			"Usage: ipc-client [input]\n"
			"Multi-client test (assignment 5): ipc-client multi\n");	
		exit(EXIT_FAILURE);
	}
	else
	{
		/* Multi client mode */
		if (!strcmp(argv[1], "multi"))
		{
			int i;
			pid_t pid;

			/* Welcome message */
			fprintf(stdout, "Welcome to the ipc-client multi mode!\n");

			for(i = 0; i < CHILD_COUNT; i++)
			{
				/* Fork off the parent process */
				pid = fork();
				/* Assign input values to child processes */
				input = i + MULTI_INPUT_VALUE;
				/* If child, break and continue with the code */
				if (pid == 0) break;
				/* Error check */
				else if (pid < 0) {
					exit(EXIT_FAILURE);
				}
			}

			/* Parent process */
			if (pid > 0) {
				/* Wait for all child processes */
				while ((pid = waitpid(-1, NULL, 0))) {
				   if (errno == ECHILD) {
				      break;
				   }
				}
				/* Exit after all children have finished */
				exit(EXIT_SUCCESS);
			}
		}
		/* Single input mode */
		else
		{		
			input = (int) strtol(argv[1], (char **)NULL, 10);

			if (input < 0)
			{
				printf("Invalid input. Input can not be lower than zero!\n");
				exit(EXIT_FAILURE);
			}
			else if (input > MAX_FIB_ARG)
			{
				printf("Invalid input. Input can not be higher than %d!\n", MAX_FIB_ARG);
				exit(EXIT_FAILURE);
			}

			/* Welcome message */
			fprintf(stdout, "Welcome to the ipc-client!\n");
		}
	}

	/* Open previously created shared memory segment */
	int shmid;
	if ((shmid = shm_open(SHM_NAME, O_RDWR, 0666)) == -1) {
		perror("shm_open: shm_open failed");
		exit(EXIT_FAILURE);
	}

	/* Attach shared memory segment */
	char* shmaddr; /* pointer to the attached segment */
	if ((shmaddr = mmap(NULL, sizeof(shmseg_t), PROT_READ|PROT_WRITE, MAP_SHARED, shmid, 0)) == (char *)-1) {
		perror("mmap: mmap failed");
		exit(EXIT_FAILURE);
	}

	/* Create a shmseg_t pointer to the shared memory segment */
	shmseg_t* my_data = (shmseg_t*) shmaddr;

	/* Open previously created begin semaphore */
	sem_t* semaphore_begin;
	if ((semaphore_begin = sem_open(SEM_NAME_BEGIN, 0)) == (sem_t *)-1) {
		perror("sem_open: sem_open failed");
		exit(EXIT_FAILURE);
	}

	/* Open previously created end semaphore */
	sem_t* semaphore_end;
	if ((semaphore_end = sem_open(SEM_NAME_END, 0)) == (sem_t *)-1) {
		perror("sem_open: sem_open failed");
		exit(EXIT_FAILURE);
	}

	/* Open previously created mutex semaphore */
	sem_t* semaphore_mutex;
	if ((semaphore_mutex = sem_open(SEM_NAME_MUTEX, 0)) == (sem_t *)-1) {
		perror("sem_open: sem_open failed");
		exit(EXIT_FAILURE);
	}

	/* Wait for the mutex to be released */
	sem_wait(semaphore_mutex);

	fprintf(stdout, "Computing fib(%d)...\n", input);

	/* Set input as the argument provided */
	my_data->input = input;

	/* Post semaphore to indicate that the input is available */
	sem_post(semaphore_begin);

	/* Wait for the daemon to compute */
	sem_wait(semaphore_end);

	/* Print result */
	fprintf(stdout, "fib(%d) Result = %d\n", my_data->input, my_data->result);

	/* Release mutex so that other clients can input their data */
	sem_post(semaphore_mutex);

	/* Detach shared memory */
	if (munmap((void*)shmaddr, sizeof(shmseg_t)) == -1) {
		perror("mmap: mmap failed");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS); // exit normally	
}
