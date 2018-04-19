# Homework #4


Takes in a single parameter, the size in bytes to be allocated, and adds it to the internal list
settings it's internal ref_count = 1.
If the list is full the function calls exit(-1) and sets errno = ENOMEM.
Returns a pointer to the beginning of this place in memory.

void * 	cse320_malloc(size_t size);

Frees any pointer that was returned by cse320_malloc().
If pointer is being freed twice, exit(-1) and sets errno = EFAULT.
If ptr does not exist in underlying data structure or ref_count corresponding to ptr
is 0, then function calls exit(-1) and sets errno = EFAULT.

void 	cse320_free(void * ptr);	

Takes in the filename of the file to be opened, and the mode in which to open the given filename.
The program then internally calls fopen(filename, mode) and adds the returned file to the internal list with ref_count = 1.
If a file is opened with the same filename as that of one in the underlying list, ref_count is incremented by 1 and corresponding
FILE * is returned.
If the list is full then the function calls exit(-1) and sets errno = EMFILE
Returns NULL if fopen returns NULL, otherwise returns the FILE*

FILE * 	cse320_fopen(const char * filename, const char * mode);

Decreases the ref_count by 1 of the file_in_use struct corresponding to the given parameter filename.
If ref_count is 0 as a result, fclose() is called on the corresponding file.
If ref_count is 0 prior to execution calls exit(-1) and sets errno = EINVAL
If the file is not in the underlying list function calls exit(-1) and sets errno = ENOENT.

void 	cse320_fclose(const char *);

Closes all open files and frees all memory with ref_count > 0. Also destroys all semaphores, must be called
only once at the end of execution.
Ensures no memory leaks if called at end of execution.

void	cse320_clean();

Creates a child process and sets an alarm to reap child processes every N seconds.
Timer interval can be set with cse320_settimer(), default value of 5.
Returns pid of child to parent process, and 0 to child process

pid_t	cse320_fork();

Sets time to reap children to given parameter.
Default value for timer is 5 seconds.

void	cse320_settimer(int);

Must be called exactly once at the beginning of program execution prior to any subsequent
calls to functions in this header. If not called program will cease to 
function and must be killed from another process :)
Initializes semaphores for thread safety.

void	cse320_init();
