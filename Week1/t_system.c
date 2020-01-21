/* Program illustrating the use of system() call to execute an 
 * arbitrary shell command. 
 *
 * compile as follows: gcc -o t_system t_system.c -std=c99 -Wall 
 *
 * Author: Naga Kandasamy
 * Date created: January 3, 2020
 *
 * Original source: M. Kerrisk, The Linux Programming Interface
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

extern int my_system (char *);

#define MAX_BUF_SIZE 100 

int 
main (int argc, char **argv)
{

    char str[MAX_BUF_SIZE];
    int status;

    while (1) {

        printf ("Command: ");
        fflush (stdout); 
        if (fgets (str, MAX_BUF_SIZE, stdin) == NULL)
            break; /* Break out of loop on ^D */

        status = my_system (str); /* Execute the command */
        printf ("system returned a status of %d\n", status);
        if (status == -1) {
            perror ("status");
            exit (EXIT_FAILURE);
        } 
    }

    exit (EXIT_SUCCESS);
}

