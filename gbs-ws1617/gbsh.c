# Team 4, members:
# 1. Christian Schirmer
# 2. Owen Wilcox
# 3. Bora Özgen


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void print_prompt(void);

int main(int argc, char *argv[]) {
	
	printf("\nWelcome to the gbsh shell!\n\n");

	// Command buffer
	char command[1024];
	command[1023] = '\0';

	// Main loop
	while (1)
	{
		// Print command prompt
		print_prompt();
		// Wait for command
		fgets(command, 1023, stdin);
		// printf("%s\n", command);

		// Process commands
		if (!strncmp(command, "exit\n", 1024))
		{
			// Exit program
			exit(0);
		}
		else if (!strncmp(command, "pwd\n", 1024))
		{
			// Working directory buffer
			char work_dir[1024];
			work_dir[1023] = '\0';
			// Get working directory
			getcwd(work_dir, 1023);
			printf("%s\n", work_dir);
		}
		else if (!strncmp(command, "clear\n", 1024))
		{
			// Clear the screen
			system("clear");
		}
		else
		{
			// Split input into tokens
			char * pch;
			pch = strtok(command, " \n\t");
			while (pch != NULL)
			{
				printf("%s\n",pch);
				pch = strtok(NULL, " \n\t");
			}
		}
	}

	exit(0); // exit normally	
}

// Function that prints the prompt line
void print_prompt(void)
{
	// Username buffer
	char username[1024];
	username[1023] = '\0';
	// Get username
	// getlogin_r(username, 1023); // Does not work on ubuntu. Use getenv instead.
	strncpy(username, getenv("USER"), 1023);

	// Hostname buffer
	char hostname[1024];
	hostname[1023] = '\0';
	// Get hostname
	gethostname(hostname, 1023);

	// Working directory buffer
	char work_dir[1024];
	work_dir[1023] = '\0';
	// Get working directory
	getcwd(work_dir, 1023);

	// Get relative path of the working directory
	char relative_path[1024];
	relative_path[1023] = '\0';
	strncpy(relative_path, strrchr(work_dir, '/') + 1, 1023); // +1 for not including the / sign

	// Print prompt line
	printf("%s@", username);
	printf("%s ", hostname);
	printf("%s > ", relative_path);
}
