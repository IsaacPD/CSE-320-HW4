# Homework #4

/*
 * Takes in a single parameter, the size in bytes to be allocated,
 * Returns a pointer to the beginning of this place in memory.
 */
void * 	cse320_malloc(size_t);

/*
 * Frees any pointer that was created with cse320_malloc()
 * Takes in pointer as only parameter.
 * If ptr does not exist in underlying data structure or ref_count corresponding to ptr
 * is 0, then function exits with return value -1 and sets errno appropriately.
 */
void 	cse320_free(void *);

/*
 * Takes in const char * filename of the file to be opened and const char * mode
 * the mode in which to open the given filename.
 * The program then internally calls fopen(filename, mode)
 * Returns NULL if fopen returns NULL, otherwise returns the FILE*
 */
FILE * 	cse320_fopen(const char *, const char *);

/*
 * Decreases the ref_count by 1 of the file_in_use struct corresponding to the given parameter filename.
 * If ref_count is 0 as a result, fclose() is called on the corresponding file.
 * If ref_count is 0 prior to execution or the file has not been opened,
 * function exits with return value -1 and sets errno appropriately.
 */
void 	cse320_fclose(const char *);

/*
 * Closes all open files and frees all memory with ref_count > 0.
 * Ensures no memory leaks if called at end of execution.
 */
void	cse320_clean();

/*
 * Creates a child process and sets an alarm to reap child processes every N seconds.
 * Timer interval can be set with cse320_settimer(), default value of 5.
 * Returns pid of child to parent process, and 0 to child process
 */
pid_t	cse320_fork();

/*
 * Sets time to reap children to given parameter.
 * Default value for timer is 5 seconds.
 */
void	cse320_settimer(int);

/*
 * Must be called exactly once at the beginning of program execution prior to any subsequent
 * calls to functions in this header. If not called program will cease to 
 * function and must be killed from another process :)
 * Initializes semaphores for thread safety.
 */
void	cse320_init();
