//# Team 4, members:
//# 1. Christian Schirmer
//# 2. Owen Wilcox
//# 3. Bora Özgen

#define _GNU_SOURCE // for execvpe()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h> // Directory management
#include <fcntl.h>	// open()

#define ENVAR_VAR_LENGTH		100
#define ENVAR_VALUE_LENGTH		100

// Linked list structure for environment variables
struct envar {
	// TODO: can we dynamically allocate these too? Also: only one string is enough eg. 'shell=path'
	char variable[ENVAR_VAR_LENGTH + 1];
	char value[ENVAR_VALUE_LENGTH + 1];
	struct envar *next;
};

// Linked list head and current node pointers
struct envar* head = NULL;
struct envar* current = NULL;

// Function prototypes
void print_prompt(void);
int get_argc(char* command);
struct envar* find(char* key);
struct envar* delete(char* key);
void add_shell_envar(char* gbsh_path);
int check_redirect_output(char* command);
void restore_stdout(int saved_stdout);
void check_redirect_input(char* command);

// Environment variables (declared in unistd.h)
extern char **environ;

// Main function
int main(int argc, char *argv[]) {
	
	printf("\nWelcome to the gbsh shell!\n\n");

	// Command buffer
	char command[1024];
	command[1023] = '\0';

	// Get path of gbsh
	char gbsh_path[ENVAR_VAR_LENGTH];
	getcwd(gbsh_path, ENVAR_VAR_LENGTH);
	strncat(gbsh_path, "/gbsh", 5);

	// Add initial 'shell' envar
	add_shell_envar(gbsh_path);

	// Main loop
	while (1)
	{
		// Print command prompt
		print_prompt();
		// Wait for command
		fgets(command, 1023, stdin);

		// Process commands: Assignment 1
		if (!strncmp(command, "exit", 4))
		{
			// Exit program
			exit(0);
		}
		else if (!strncmp(command, "pwd", 3))
		{
			int saved_stdout = check_redirect_output(command);

			// Working directory buffer
			char work_dir[1024];
			work_dir[1023] = '\0';
			// Get working directory
			getcwd(work_dir, 1023);
			printf("%s\n", work_dir);

			restore_stdout(saved_stdout);
		}
		else if (!strncmp(command, "clear", 5))
		{
			// Clear the screen
			system("clear");
		}
		// Process commands: Assignment 2
		// TODO: somehow invalidate 'lsxxx' type of commands. Plus general faulty input checks.
		else if (!strncmp(command, "ls", 2))
		{
			// Check argument count
			if (get_argc(command) > 0)
			{
				int saved_stdout = check_redirect_output(command);

				// Get directory string from the argument
				char * pch;
				pch = strtok(command, " \n\t");
				pch = strtok(NULL, " \n\t");

				DIR *dp;
				struct dirent *ep;

				dp = opendir (pch);
				if (dp != NULL)
				{
					printf("Contents of the directory %s:\n", pch);

				  	while ( (ep = readdir (dp)) )
				    	puts (ep->d_name);
				  	(void) closedir (dp);
				}
				else
					perror ("Couldn't open the directory. Error");

				restore_stdout(saved_stdout);
			}
			else
			{
				// Invalid arguments
				printf("Invalid arguments.\n");
			}

		}
		else if (!strncmp(command, "cd", 2))
		{
			int argc = get_argc(command);
			// Check argument count
			if (argc == 1)
			{
				// Get directory string from the argument
				char * pch;
				pch = strtok(command, " \n\t");
				pch = strtok(NULL, " \n\t");

				if (!chdir(pch))
				{
					printf("Changing directory to %s\n", pch);
				}
				else
				{
					perror ("Couldn't change directory. Error");
				}
			}
			else if (argc == 0)
			{
				// No directory specified, changing to the home directory
				if (!chdir(getenv("HOME")))
				{
					printf("Changing directory to %s\n", getenv("HOME"));
				}
				else
				{
					perror ("Couldn't change directory. Error");
				}
			}
			else
			{
				// Invalid arguments
				printf("Invalid arguments.\n");
			}
		}
		else if (!strncmp(command, "environ", 7))
		{
			if (head != NULL)
			{
				int saved_stdout = check_redirect_output(command);

				struct envar *ptr = head;
				printf("Current environment variables:\n");

				// Traverse through the linked list
				while (ptr != NULL) {
					printf("%s=%s\n", ptr->variable, ptr->value);
					ptr = ptr->next;
				}

				restore_stdout(saved_stdout);
			}
			else
			{
				printf("No environment variables found.\n");
			}
		}
		else if (!strncmp(command, "setenv", 6))
		{
			int argc = get_argc(command);

			// Check argument count
			if (argc == 1 || argc == 2) 
			{
				// Get environment string from the argument
				char* arg1;
				strtok(command, " \n\t");
				arg1 = strtok(NULL, " \n\t");

				if (!find(arg1))
				{
					// Create new environment variable node
					struct envar *link = (struct envar*) malloc(sizeof(struct envar));
					// Place null terminators for security
					link->variable[ENVAR_VAR_LENGTH] = '\0';
					link->value[ENVAR_VALUE_LENGTH] = '\0';
					// Copy inputs to envar
					strncpy(link->variable, arg1, ENVAR_VAR_LENGTH);
					if (argc == 2)
					{
						char* arg2;
						arg2 = strtok(NULL, " \n\t");
						strncpy(link->value, arg2, ENVAR_VALUE_LENGTH);
						printf("Setting environment variable %s to %s.\n", arg1, arg2);
					}
					else
					{
						strncpy(link->value, "\0", ENVAR_VALUE_LENGTH);
						printf("Setting environment variable %s to the empty string.\n", arg1);
					}

					// Point it to old first node
					link->next = head;
					// Point first to new first node
					head = link;
				}
				else
				{
					// Envar already created
					printf("Error: Environment variable already exists.\n");
				}
			}
			else
			{
				// Invalid arguments
				printf("Invalid arguments.\n");
			}
		}
		else if (!strncmp(command, "unsetenv", 8))
		{
			int argc = get_argc(command);
			
			// Check argument count
			if (argc == 1) 
			{
				// Get environment string from the argument
				char* arg1;
				strtok(command, " \n\t");
				arg1 = strtok(NULL, " \n\t");

				if (delete(arg1))
				{
					printf("Environment variable %s deleted.\n", arg1);
				}
				else
				{
					printf("Environment variable %s not found.\n", arg1);
				}
			}
			else
			{
				// Invalid arguments
				printf("Invalid arguments.\n");
			}
		}
		else
		{
			int argc = get_argc(command);

			if (argc < 0)
			{
				// No command provided. Do nothing.
			}
			else
			{
				// Assume program invocation

				pid_t pid = fork();

				if (pid == -1)
				{
					// Error, failed to fork()
					perror("Could not fork. Error:");
				} 
				else if (pid > 0)
				{
					// Parent process waits for the child
					int status;
					waitpid(pid, &status, 0);
					printf("Child return status: %d\n", status);
				}
				else 
				{
					// Child process executes the program
					char* pch;		// Dummy char pointer for strtok
					char* path;		// Path pointer
					char** argv;	// Argument array pointer
					// char* envp[2];	// Environment variable array pointer

					// TODO: '< input > output' is ok but '> output < input' does not work. Probably functions damage the main string.
					check_redirect_output(command);
					check_redirect_input(command);

					// Get first token (in this case command path)
					pch = strtok(command, " \n\t");
					// Save executable path
					path = malloc(strlen(pch)+1);
					strncpy(path, pch, strlen(pch));
					printf("Command path: %s\n", path);

					// Allocate memory for argument string pointers
					argv = malloc((argc + 2) * sizeof(char*)); // arguments + 2 for the executable itself and NULL pointer

					// NULL pointer check
					if (argv != NULL)
					{
						// First element of argument array is the path itself
						argv[0] = malloc(strlen(pch)+1);
						strcpy(argv[0], pch);
						int i=1;
						// Allocate memory for strings
						for (i=1; i < argc+1; i++)
						{
							// Get next token (in this case argument)
							pch = strtok(NULL, " \n\t");
							// Break if < or > sign is found
							if (!strncmp(pch, ">", 1) || !strncmp(pch, "<", 1)) break;
							// Allocate memory
							argv[i] = malloc(strlen(pch)+1);
							// Copy string
							strcpy(argv[i], pch);

							printf("Argument %d: %s\n", i, argv[i]);
						}
						// NULL terminate the array
						argv[++i] = NULL;
					}
					else
					{
						printf("Could not malloc.\n");
					}
    				
    				/* TODO: How to pass environment variables?
    					execve does not execute without specifying the path.
    					Also passing PATH as an envar does not help.
    					A possibility: adding the required 'parent' envar to char** environ.
    					*/
    				/*
					extern char** environ;
					envp = malloc(sizeof(environ) + 1*sizeof(char*));
					memcpy(&envp, &environ, sizeof(environ));*/

					/*
					// Add environment variable 'parent'
					envp[0] = malloc(strlen(gbsh_path) + 10); // 10 for additional envar name
					strcpy(envp[0], "parent=");
					strcat(envp[0], gbsh_path);
					envp[1] = malloc(50);
					strcpy(envp[1], "PATH=/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin");

					char *env_args[] = { "PATH=/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin", "USER=boraozgen", NULL };

					printf("environment variable: %s\n", envp[0]);
					// execve(path, argv, env_args);
					*/
					
					// Problems when executing with argv[0] instead of path. Why?
					execvp(path, argv);
					perror("exec error");
					_exit(EXIT_FAILURE);   // exec never returns

					// Free allocated memory
					free(path);
					for(int i=0; i < argc+2; i++)
					{
						free(argv[i]);
					}
					free(argv);
				}
			}
			
			/* Assigment 1 - deprecated
			// Split input into tokens
			char * pch;
			pch = strtok(command, " \n\t");
			while (pch != NULL)
			{
				printf("%s\n", pch);
				pch = strtok(NULL, " \n\t");
			}
			*/
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
	printf("~%s > ", relative_path);
}

// Function to get argument count
int get_argc(char* command)
{
	// Initialize argument count as -1. Hence, one token means command with no args. Two tokens means 1 argument is provided etc.
	int argc = -1;
	// Duplicate the command for token processing
	char command_dup[1024];
	strncpy(command_dup, command, 1023);
	// Split input into tokens
	char * pch;
	pch = strtok(command_dup, " \n\t");
	while (pch != NULL)
	{
		pch = strtok(NULL, " \n\t");
		argc++;
	}

	return argc;
}

// Find an environment variable with given string key
struct envar* find(char* key) 
{
	//start from the first link
	struct envar* current = head;

	//if list is empty
	if(head == NULL) 
	{
		return NULL;
	}

	//navigate through list
	while(strncmp(current->variable, key, ENVAR_VAR_LENGTH)) 
	{
		//if it is last node
		if(current->next == NULL) 
		{
			return NULL;
		} 
		else 
		{
			//go to next link
			current = current->next;
		}
	}      

	//if data found, return the current Link
	return current;
}

// Delete an environment variable with given string key
struct envar* delete(char* key) {

	//start from the first link
	struct envar* current = head;
	struct envar* previous = NULL;

	//if list is empty
	if(head == NULL) {
	  return NULL;
	}

	//navigate through list
	while(strncmp(current->variable, key, ENVAR_VAR_LENGTH)) {

	  //if it is last node
	  if(current->next == NULL) {
	     return NULL;
	  } else {
	     //store reference to current link
	     previous = current;
	     //move to next link
	     current = current->next;
	  }
	}

	//found a match, update the link
	if(current == head) {
	  //change first to point to next link
	  head = head->next;
	} else {
	  //bypass the current link
	  previous->next = current->next;
	}

	// Free allocated memory
	free(current);

	return current;
}

// Add initial shell envar
void add_shell_envar(char* gbsh_path)
{
	// Create new environment variable node
	struct envar *link = (struct envar*) malloc(sizeof(struct envar));
	// Place null terminators for security
	link->variable[ENVAR_VAR_LENGTH] = '\0';
	link->value[ENVAR_VALUE_LENGTH] = '\0';
	// Copy strings to envar
	strncpy(link->variable, "shell", ENVAR_VAR_LENGTH);
	strncpy(link->value, gbsh_path, ENVAR_VALUE_LENGTH);
	
	// Point it to old first node
	link->next = head;
	// Point first to new first node
	head = link;
}

// Check if command includes argument '>'. If so, open file using open() and duplicate output using dup2()	
int check_redirect_output(char* command)
{
	int saved_stdout = 0;

	char* output_symbol_location = strstr(command, " > ");
	if (output_symbol_location)
	{
		// Get the output filename
		char* file = strtok(output_symbol_location + 3, " \n");
		if (file)
		{
			// Open file write only, append, create if not exists. Set read rights to creater.
			int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
			if (fd > 0)
			{
				// Store current stdout
				saved_stdout = dup(1);

				printf("Redirecting stdout to file: %s\n", file);

				// File opened, redirect stdout (1) to file descriptor
				if (dup2(fd, 1) != -1)
				{
					close(fd); // File no longer needed.
				}
				else
				{
					perror("Unable to dup2. Error");
				}
			}
			else
			{
				perror("Unable to open file. Error");
			}
		}
		else
		{
			printf("No file specified.\n");
		}
	}

	return saved_stdout;
}

// Check input. If stdout is redirected, restore it.
void restore_stdout(int saved_stdout)
{
	if (saved_stdout)
	{
		dup2(saved_stdout, 1);
		close(saved_stdout);
	}
}

// Check if command includes argument '<'. If so, open file using open() and duplicate input using dup2()
void check_redirect_input(char* command)
{
	char* input_symbol_location = strstr(command, " < ");
	if (input_symbol_location)
	{
		// Get the input filename
		char* file = strtok(input_symbol_location + 3, " \n");
		printf("Filename: %s\n",file );
		if (file)
		{
			// Open file read-only
			int fd = open(file, O_RDONLY);
			if (fd > 0)
			{
				printf("Redirecting stdin to file: %s\n", file);
				// File opened, redirect stdin (0) to file descriptor
				if (dup2(fd, 0) != -1)
				{
					close(fd); // File no longer needed.
				}
				else
				{
					perror("Unable to dup2. Error");
				}
			}
			else
			{
				perror("Unable to open file. Error");
			}
		}
		else
		{
			printf("No file specified.\n");
		}
	}
}
