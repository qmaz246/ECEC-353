/* Program illustrates use of the fork() system call to create a child process.
 * 
 * Author: Naga Kandasamy
 * Date created: December 22, 2008
 * Date modified: Jamuary 3, 2020
 *
 * Compile as follows: gcc -o simple_fork simple_fork.c -std=c99 -Wall
 *
 * Execute as follows:
 * $ ./simple_fork
*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int 
main (int argc, char **argv)
{
    int pid; /* Process ID returned by fork. */
    int status;
  
    printf ("Process ID of parent: %d\n", getpid ()); /* Print parent's process ID. */
    
    if ((pid = fork ()) < 0) { /* Fork the process. */
        perror ("fork");
        exit (EXIT_FAILURE);
    }
  
    if (pid > 0) { /* Parent code. */
        printf ("Process ID of child: %d\n", pid); /* Print child's process ID. */
    }
    else if (pid == 0) { /* Child code. */
        printf ("Process ID of child: %d\n", getpid ()); /* Print child's process ID. */
        sleep (3); /* Sleep for a few seconds to simulate some processing. */
        exit (EXIT_SUCCESS); /* Child exits. */
    }

    /* Parent code. */
    pid = waitpid (pid, &status, 0); /* Wait for child to terminate and write the status. */
    printf ("Child %d has terminated with status %d. Parent terminating. \n", pid, status); 
    exit (EXIT_SUCCESS);
}
