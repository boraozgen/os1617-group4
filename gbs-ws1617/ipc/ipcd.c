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
#include <syslog.h>
#include <string.h>
#include <stdarg.h> // variable argument lists
#include <semaphore.h>

/* Compute fibonacci series */
int fib(int n) {
   if (n <= 1)
      return n;
   return fib(n-1) + fib(n-2);
}

/* Log message */
void log_message(char *filename, const char* format, ...) {
	FILE *logfile;
	logfile = fopen(filename, "a");
	if (!logfile) return;
	va_list list;
	va_start( list, format );
	vfprintf(logfile, format, list);
	va_end( list );
	fprintf(logfile, "\n");
	fclose(logfile);
}

int main(int argc, char *argv[]) {
	
	fprintf(stdout, "Initiating daemon...\n");
        
	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0) {
		printf("Daemon forked.\n");
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);
	        
	/* Open any logs here */        
	        
	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Daemon-specific initialization goes here */
	char* filename = "/tmp/bora/gbsd.log";
	int daemon_pid = getpid();
	// TODO: make directory if it does not exist

	/* Create shared memory segment */
	key_t key = 1234; 			/* key to be passed to shmget() */ 
	int shmflg = IPC_CREAT | 0666; 		/* shmflg to be passed to shmget() */ 
	int shmid; 							/* return value from shmget() */ 
	int size = sizeof(shmseg_t);		/* size to be passed to shmget() */ 

	// TODO: convert to POSIX shared memory API

	if ((shmid = shmget(key, size, shmflg)) == -1) {
		log_message(filename, "shmget: shmget failed");
		exit(EXIT_FAILURE);
	} 
	else {
		log_message(filename, "shmget: shmget returned %d", shmid);
	}

	/* Attach shared memory segment */
	char* shmaddr; /* pointer to the attached segment */

	if ((shmaddr = shmat(shmid, NULL, 0)) == (char *)-1) {
		log_message(filename, "shmat: shmat failed");
		exit(EXIT_FAILURE);
	}
	else {
		log_message(filename, "shmat: shmat success");
	}

	/* Initialize data structure with zeroes */
	shmseg_t* my_data = (shmseg_t*) shmaddr;
	my_data->input = 0;
	my_data->result = 0;

	/* Initialize semaphore */
	const char* sem_name = "IPCD_SEM";
	sem_t* semaphore;
	if ((semaphore = sem_open(sem_name, O_CREAT, 0666, 0)) == (sem_t *)-1) {
		log_message(filename, "sem_open: sem_open failed");
		exit(EXIT_FAILURE);
	}
	else {
		log_message(filename, "sem_open: sem_open success");
	}

	/* The Big Loop */
	while (1) {
		/* Wait using semaphore for the client input. */
		sem_wait(semaphore);

		/* compute Fib(n) and write the result to the shared memory segment */
		my_data->result = fib(my_data->input);
		log_message(filename, "Daemon PID: %d, Computation complete: fib(%d) = %d", daemon_pid, my_data->input, my_data->result);

		/* Post semaphore to indicate that the result is available */
		sem_post(semaphore);

		// TEMP
		sleep(1);
	}

 	exit(EXIT_SUCCESS);
}

// Daemon skeleton code source: http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html