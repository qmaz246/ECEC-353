/* This program illustrates the use of the vfork() system call to create a child process. The vfork() is an 
 * optimization over the fork() call in that it does not replicate the parent's text and data regions in the 
 * child's address space. The parent and child reference the same data and text regions.
 *
 * Author: Naga Kandasamy
 * Date created: December 22, 2008 
 * Date modified: January 3, 2020
 *
 * Compile as follows: gcc -o simple_fork_v3 simple_fork_v3.c -std=c99 -Wall
 * Execute as follows: ./simple_fork_v3
*/ 

/* Feature test macro to ensure that the compiler does not throw up a warning when using vfork () */
#define _BSD_SOURCE || _SVID_SOURCE || \\
    (_XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) && \\
    !(_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int global_variable = 10;

int 
main (int argc, char **argv)
{
    int pid; /* Process ID returned by vfork */
    int status;
    int local_variable = 100; 
  
    if ((pid = vfork ()) < 0) { /* Fork the process. vfork does not replicate the address space */
        perror ("fork");
        exit (EXIT_FAILURE);
    }  
    else if (pid == 0) { /* Child code */
        global_variable++; /* Increment global variable */
        local_variable++; /* Increment local variable */
        printf ("Child: Global variable = %d, Local variable = %d\n", global_variable, local_variable);
        exit (EXIT_SUCCESS); /* Child exits */
    }

    /* Parent code */
    pid = waitpid (pid, &status, 0); /* Wait for child to terminate */
    printf ("Parent: Global variable = %d, Local variable = %d\n", global_variable, local_variable);
    exit (EXIT_SUCCESS);
}
