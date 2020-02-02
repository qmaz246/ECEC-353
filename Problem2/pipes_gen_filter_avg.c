/* Main program that uses pipes to connect the three filters as follows:
 *
 * ./gen_numbers n | ./filter_pos_nums | ./calc_avg
 * 
 * Authors: Edward Mazzilli & Omer Somethingorwhatever 
 * Date created: 02/01/2020
 *
 * Compile as follows: 
 * gcc -o pipes_gen_filter_avg pipes_gen_filter_avg.c -std=c99 -Wall -lm
 *
 * guidance from pipe_ls_wc.c example code
 * */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


int 
main(int argc, char **argv)
{
    int pipeGTF[2];
    int pipeFTC[2];

    if ((pipe(pipeGTF) < 0) || (pipe(pipeFTC) < 0)) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:                 /* Child code */
            if (close(pipeGTF[0]) == -1) { /* Close read end of gen_to_filter pipe */
                perror("close 1");
                exit(EXIT_FAILURE);
            }

            /* Duplicate stdout on write end of pipe; close the duplicated 
             * descriptor. */
            if (pipeGTF[1] != STDOUT_FILENO) {
                if (dup2(pipeGTF[1], STDOUT_FILENO) == -1) {
                    perror("dup2 1");
                    exit(EXIT_FAILURE);
                }
            
                if (close(pipeGTF[1]) == -1) {
                    perror("close 2");
                    exit(EXIT_FAILURE);
                }
            }
            
            /* Execute the first command. Write to pipe. */
            execlp("./gen_numbers", "./gen_numbers", argv[1], (char *) NULL);
            exit (EXIT_FAILURE);    /* Should not get here unless execlp returns an error */

        default:
            break; /* Parent falls through to create the next child */
    }

    switch (fork ()) {
        case -1:
            perror ("fork");
            exit (EXIT_FAILURE);

        case 0:                 /* Child code */
            if (close(pipeGTF[1]) == -1) {	/* close write end of gen_to_filter pipe */
                perror ("close 3");
                exit (EXIT_FAILURE);
            }
            if (close(pipeFTC[0]) == -1) {	/* close read end of filter_to_calc pipe */
                perror ("close 9");
                exit (EXIT_FAILURE);
            }

            /* Duplicate stdin on read end of pipe; close the duplicated 
             * descriptor. */
            if (pipeGTF[0] != STDIN_FILENO){
                if (dup2(pipeGTF[0], STDIN_FILENO) == -1) {
                    perror ("dup2 2");
                    exit (EXIT_FAILURE);
                }

                if (close (pipeGTF[0]) == -1){
                    perror ("close 4");
                    exit (EXIT_FAILURE);
                }
            }
	
	    // Duplicate stdout on write end of pipe; close the duplicated descriptor.
            if (pipeFTC[1] != STDOUT_FILENO) {
                if (dup2(pipeFTC[1], STDOUT_FILENO) == -1) {
                    perror("dup2 3");
                    exit(EXIT_FAILURE);
                }
            
                if (close(pipeFTC[1]) == -1) {
                    perror ("close 2");
                    exit (EXIT_FAILURE);
                }
            }
                
            /* Execute the second command. Reads from pipe. */
            execlp ("./filter_pos_nums", "./filter_pos_nums", (char *) NULL);
            exit (EXIT_FAILURE);

        default:
            break; /* Parent falls through. */
    }

    switch (fork ()) {
        case -1:
            perror ("fork");
            exit (EXIT_FAILURE);

        case 0:                 // Child code 
            if (close(pipeFTC[1]) == -1) {	// Close write end of filter_to_calc pipe 
                perror ("close 3");
                exit (EXIT_FAILURE);
            }

            // Duplicate stdin on read end of pipe; close the duplicated descriptor. 
            if (pipeFTC[0] != STDIN_FILENO){
                if (dup2(pipeFTC[0], STDIN_FILENO) == -1) {
                    perror("dup2 4");
                    exit(EXIT_FAILURE);
                }

                if (close(pipeFTC[0]) == -1){
                    perror ("close 5");
                    exit (EXIT_FAILURE);
                }
            }
                
            // Execute the second command. Reads from pipe.
            execlp ("./calc_avg", "./calc_avg", (char *) NULL);
            exit (EXIT_FAILURE);

        default:
            break; // Parent falls through.
    }

    /* Parent closes unused file descriptors for pipe and waits for the 
     * children to terminate. */
    if (close (pipeGTF[0]) == -1) {
        perror ("close 6");
        exit (EXIT_FAILURE);
    }
    
    if (close (pipeGTF[1]) == -1) {
        perror ("close 7");
        exit (EXIT_FAILURE);
    }

    if (close (pipeFTC[0]) == -1) {
        perror ("close 8");
        exit (EXIT_FAILURE);
    }
    
    if (close (pipeFTC[1]) == -1) {
        perror ("close 9");
        exit (EXIT_FAILURE);
    }

    if (wait (NULL) == -1) {
        perror ("wait 1");
        exit (EXIT_FAILURE);
    }

    if (wait (NULL) == -1) {
        perror ("wait 2");
        exit (EXIT_FAILURE);
    }
/*    
    if (wait (NULL) == -1) {
	perror ("wait 3");
	exit(EXIT_FAILURE);
    }
  */  
    exit (EXIT_SUCCESS);
}
