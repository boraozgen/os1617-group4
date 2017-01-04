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

	/* Log file */
	char* filename = "/tmp/bora/ipcd.log";
        
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
		printf("Daemon created. Check log file %s\n", filename);
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);       
	        
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

	/* Create directory if it does not exist */
	struct stat st = {0};
	if (stat("/tmp/bora", &st) == -1) {
	    mkdir("/tmp/bora", 0755);
	}

	/* Get daemon PID and create log file */
	int daemon_pid = getpid();
	log_message(filename, "ipcd started with PID: %d", daemon_pid);

	/* Create shared memory segment */
	shm_unlink(SHM_NAME); // Unlink any memory which has been created before. Not doing this fails ftruncate.

	int shmid; /* shared memory file descriptor */
	if ((shmid = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) == -1) {
		log_message(filename, "shm_open: shm_open failed:%s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	/* Set the size of the memory segment (one-time operation) */
	if (ftruncate(shmid, sizeof(shmseg_t)) == -1) {
		log_message(filename, "ftruncate: ftruncate failed:%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Attach shared memory segment */
	char* shmaddr; /* pointer to the attached segment */
	if ((shmaddr = mmap(NULL, sizeof(shmseg_t), PROT_READ|PROT_WRITE, MAP_SHARED, shmid, 0)) == (char *)-1) {
		log_message(filename, "mmap: mmap failed, error:%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Initialize data structure with zeroes */
	shmseg_t* my_data = (shmseg_t*) shmaddr;
	my_data->input = 0;
	my_data->result = 0;

	/* Initialize semaphore */
	sem_t* semaphore;
	if ((semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 0)) == (sem_t *)-1) {
		log_message(filename, "sem_open: sem_open failed");
		exit(EXIT_FAILURE);
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
	}

 	exit(EXIT_SUCCESS);
}

// Daemon skeleton code source: http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html