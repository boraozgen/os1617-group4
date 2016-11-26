#define _GNU_SOURCE // for execvpe()
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pipes.h"

// TODO: check function descriptions

#if 0
int main(void)
{
	char input[] = "ls -la | grep txt | wc -m\n";

	fork_pipes(input);

	return 0;
}
#endif

/**
 * Get the arguments array of pipeline
 * Example: 'ls -la | grep txt' returns array of argument arrays ['ls', '-la', NULL] 
 * and ['grep', 'txt', NULL] followed by a NULL pointer
 *
 * @param  input Input by the user
 * @return Array of argument arrays
 */
char*** get_argv_array(char* input)
{
	char** commands = NULL;
	commands = split_into_tokens(input, "|");

	char*** argv_array = NULL;
	int i=0;
	while(commands[i])
	{
		argv_array = realloc (argv_array, sizeof (char**) * (i+1));

		if (argv_array == NULL)
			exit (-1); /* memory allocation failed */

		argv_array[i] = split_into_tokens(commands[i], " \n\t");
		i++;
	}

	argv_array = realloc (argv_array, sizeof (char**) * (i+1));
	argv_array[i] = 0;

	int j=0;
	for(j = 0; j < i; ++j)
	{
		printf("Command %d: ", j);
		int k=0;
		while(argv_array[j][k])
		{
			printf("[%s] ", argv_array[j][k]);
			k++;
		}
		printf("\n");
	}

	return argv_array;
}

/**
 * Splits a given string into tokens.
 *
 * @param  input Input string
 * @param  delimiter Delimiter
 * @return Array of tokens
 */
char** split_into_tokens(char* input, char* delimiter)
{
	char ** res  = NULL;
	char *  p    = strtok (input, delimiter);
	int n_spaces = 0;

	// First split using pipe symbols

	/* split string and append tokens to 'res' */

	while (p) {
		res = realloc (res, sizeof (char*) * ++n_spaces);

		if (res == NULL)
			exit (-1); /* memory allocation failed */

		res[n_spaces-1] = p;

		p = strtok (NULL, delimiter);
	}

	/* realloc one extra element for the last NULL */

	res = realloc (res, sizeof (char*) * (n_spaces+1));
	res[n_spaces] = 0;

	/* print the result */
	#if 0
	int i;
	for (i = 0; i < (n_spaces+1); ++i)
		printf ("res[%d] = %s\n", i, res[i]);
	#endif
	/* free the memory allocated */

	//free (res);

	return res;
}

/**
 * Forks and executes a program after redirecting its 
 * read and write locations if necessary
 *
 * @param  in Input file descriptor
 * @param  out Output file descriptor
 * @return Child PID. Does not return if it executes a program successfully.
 */
int spawn_proc (int in, int out, char*** cmd)
{
	pid_t pid;

	if ((pid = fork ()) == 0)
	{
		if (in != 0)
		{
			dup2 (in, 0);
			close (in);
		}

		if (out != 1)
		{
			dup2 (out, 1);
			close (out);
		}

		return execvp (*cmd[0], *cmd);
	}

	return pid;
}

/**
 * Create pipes and connect to each other
 * Uses get_argv_array as a middleware
 *
 * @param  input User input
 * @return exec returns only if there is an error
 */
int fork_pipes (char* input)
{
	char*** cmd = get_argv_array(input);

	int i = 0;
	int in, fd [2];

	/* The first process should get its input from the original file descriptor 0.  */
	in = 0;

	/* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
	while (cmd[i+1])
	{
		pipe (fd);

		/* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
		spawn_proc (in, fd [1], cmd + i);

		/* No need for the write end of the pipe, the child will write here.  */
		close (fd [1]);

		/* Keep the read end of the pipe, the next child will read from there.  */
		in = fd [0];

		i++;
	}

	/* Last stage of the pipeline - set stdin be the read end of the previous pipe
	 and output to the original file descriptor 1. */  
	if (in != 0)
	dup2 (in, 0);

	/* Execute the last stage with the current process. */
	return execvp (cmd[i][0], cmd[i]);

	// No need to free memory, since program image is overwritten by exec
}


/**
 * Check if command includes argument '|'
 *
 * @param  input User input
 * @return true if piping is requested
 */
bool check_pipe(char* input)
{
	// Duplicate the command for token processing
	char input_copy[1024];
	strncpy(input_copy, input, 1023);

	char* pipe_symbol_location = strstr(input_copy, " | ");
	if(pipe_symbol_location)
	{
		printf("Pipe command received.\n");
		return true;
	}
	else
	{
		return false;
	}
}