/* Define boolean type */
typedef enum { false, true } bool;
/**
 * Get the arguments array of pipeline
 * Example: 'ls -la | grep txt' returns array of argument arrays ['ls', '-la', NULL] 
 * and ['grep', 'txt', NULL] followed by a NULL pointer
 *
 * @param  input Input by the user
 * @return Array of argument arrays
 */
char*** get_argv_array(char* input);

/**
 * Splits a given string into tokens.
 *
 * @param  input Input string
 * @param  delimiter Delimiter
 * @return Array of tokens
 */
char** split_into_tokens(char* input, char* token);

/**
 * Forks and executes a program after redirecting its 
 * read and write locations if necessary
 *
 * @param  in Input file descriptor
 * @param  out Output file descriptor
 * @return Child PID. Does not return if it executes a program successfully.
 */
int spawn_proc (int in, int out, char*** cmd);

/**
 * Create pipes and connect to each other
 * Uses get_argv_array as a middleware
 *
 * @param  input User input
 * @return exec returns only if there is an error
 */
int fork_pipes (char* input);

/**
 * Check if command includes argument '|'
 *
 * @param  input User input
 * @return true if piping is requested
 */
bool check_pipe(char* command);