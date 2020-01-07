/* Program that demonstrates the use of execve. 
 * The parent program creates an argument list and an environment list for the new program. Then calls execve, using 
 * its command line argument (argv[1]) as the pathname to be executed.
 *
 * Compile as follows: gcc -o t_execve t_execve.c -std=c99 -Wall 
 * Execute as follows: ./t_execve ./envargs
 *
 * Author: Naga Kandasamy
 * Date created: December 26, 2019
 * Date modified: 
 * 
 * Notes: Original program from M. Kerrisk, The Linux Programming Interface.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int 
main (int argc, char **argv)
{
    char *arg_vec[10]; /* Array to store the arguments. */
    char *env_vec[] = {"GREET=salut", "BYE=adieu", NULL};

    if (argc != 2) {
        printf ("Usage: %s pathname \n", argv[0]);
        exit (EXIT_SUCCESS);
    }

    /* Extract the name of the program from argv[1]. */
    arg_vec[0] = strrchr (argv[1], '/'); 
    if (arg_vec[0] != NULL) 
        arg_vec[0]++; 
    else 
        arg_vec[0] = argv[1];

    /* Fill in the list of arguments with some values. */
    arg_vec[1] = "hello world";
    arg_vec[2] = "goodbye";
    arg_vec[3] = NULL; /* List must be NULL terminated. */

    execve (argv[1], arg_vec, env_vec);

    perror ("execve"); /* execve never returns; so shouldn't get here unless in case of error. */
    exit (EXIT_FAILURE);
}


