#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <stdarg.h> // variable argument lists

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
	int uptime = 0;
	// TODO: make directory if it does not exist

	/* The Big Loop */
	while (1) {
		log_message(filename, "Daemon PID: %d, Uptime: %d s", daemon_pid, uptime++);
		sleep(1);
	}

 	exit(EXIT_SUCCESS);
}

// Daemon skeleton code source: http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html