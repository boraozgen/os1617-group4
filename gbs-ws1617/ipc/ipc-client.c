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
	
	/* Welcome message */
	fprintf(stdout, "Welcome to the ipc-client!\n"
		"Computing fib(%d)...\n", input);

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

	/* Set input as the argument provided */
	my_data->input = input;

	/* Post semaphore to indicate that the input is available */
	sem_post(semaphore_begin);

	/* Wait for the daemon to compute */
	sem_wait(semaphore_end);

	/* Print result */
	fprintf(stdout, "Result = %d\n", my_data->result);

	/* Detach shared memory */
	if (munmap((void*)shmaddr, sizeof(shmseg_t)) == -1) {
		perror("mmap: mmap failed");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS); // exit normally	
}
