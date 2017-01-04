#include "ipcd.h"
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_FIB_ARG 40

int main(int argc, char *argv[]) {

	/* Parse argument n from the command line */
	int input;
	if (argc != 2) 
	{
		printf("Invalid argument count.\n"
			"Usage: ipc-client [input]\n");	
		exit(EXIT_FAILURE);
	}
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
	}
	
	fprintf(stdout, "Welcome to the ipc-client!\n"
		"Computing fib(%d)...\n", input);

	/* Create shared memory segment */
	key_t key = 1234; 			/* key to be passed to shmget() */ 
	int shmflg = 0666; 		/* shmflg to be passed to shmget() */ 
	int shmid; 							/* return value from shmget() */ 
	int size = sizeof(shmseg_t);		/* size to be passed to shmget() */ 

	if ((shmid = shmget(key, size, shmflg)) == -1) {
		perror("shmget: shmget failed");
		exit(EXIT_FAILURE);
	} 
	else {
		printf("shmget: shmget returned %d\n", shmid);
	}

	/* Attach shared memory segment */
	char* shmaddr; /* pointer to the attached segment */

	if ((shmaddr = shmat(shmid, NULL, 0)) == (char *)-1) {
		perror("shmat: shmat failed");
		exit(EXIT_FAILURE);
	}
	else {
		printf("shmat: shmat success\n");
	}

	/* Create a shmseg_t pointer to the shared memory segment */
	shmseg_t* my_data = (shmseg_t*) shmaddr;

	/* Initialize semaphore */
	const char* sem_name = "IPCD_SEM";
	sem_t* semaphore;
	if ((semaphore = sem_open(sem_name, O_CREAT, 0666, 0)) == (sem_t *)-1) { // TODO: 2 arguments with no O_CREAT
		perror("sem_open: sem_open failed");
		exit(EXIT_FAILURE);
	}
	else {
		printf("sem_open: sem_open success\n");
	}

	/* Set input as the argument provided */
	my_data->input = input;

	/* Post semaphore to indicate that the input is available */
	sem_post(semaphore);

	/* Wait for the daemon to compute */
	sem_wait(semaphore);

	/* Print result */
	printf("Result = %d\n", my_data->result);

	// TODO: detach shared memory

	exit(0); // exit normally	
}
