/* Program to launch the barber and customer threads. 
 *
 * Compile as follows: gcc -o launcher launcher.c -std=c99 -Wall -lpthread -lm
 *
 * Run as follows: ./launcher 
 *
 * Author: Naga Kandasamy
 * Date: February 7, 2020
 *
 * Student/team: Edward Mazzilli and Omer Odermis 
 * Date: February 23rd, 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MIN_CUSTOMERS 10
#define MAX_CUSTOMERS 20
#define WAITING_ROOM_SIZE 8

/* Returns a random integer value, uniformly distributed between [min, max] */ 
int
rand_int (int min, int max)
{
    return ((int) floor ((double) (min + ((max - min + 1) * ((float) rand ()/(float) RAND_MAX)))));
}
    
int 
main (int argc, char **argv)
{
    int i, pid, num_customers;
    char arg[40];
    char arg1[40];
    char arg2[40];
    char arg3[40];
    char arg4[40];
    char arg5[40];
    char arga[40];
    char argb[40];
    char argd[40];
    int oflag = O_CREAT;
    oflag |= O_EXCL;
    mode_t perms = S_IRUSR | S_IWUSR;

    /* Create the needed *named* semaphores and initialize them to the correct values */
    sem_t *waiting_room = sem_open("/waiting_room_eqm23", oflag, perms, WAITING_ROOM_SIZE);
    sem_t *barber_chair = sem_open("/barber_chair_eqm23", oflag, perms, 1);
    sem_t *done_with_customer = sem_open("/done_with_customer_eqm23", oflag, perms, 0);
    sem_t *barber_bed = sem_open("/barber_bed_eqm23", oflag, perms, 0);


    /* Create the barber process. */
    printf ("=== Launcher: creating barber proccess ===\n"); 
    fflush(stdout);
    pid = fork ();
    switch (pid) {
        case -1:
            perror ("fork");
            exit (EXIT_FAILURE);

        case 0: /* Child process */
            /* Execute the barber process, passing the waiting room size and semaphores */ 
             
            snprintf (arga, sizeof (arga), "%d", WAITING_ROOM_SIZE);
            snprintf (argb, sizeof (argb), "%s", "/done_with_customer_eqm23");
            snprintf (argd, sizeof (argd), "%s", "/barber_bed_eqm23");

	    
            execlp ("./barber", "barber", arga, argb, argd, (char *) NULL);
            perror ("exec"); /* If exec succeeds, we should get here */
            exit (EXIT_FAILURE);
        
        default:
            break;
    }

    /* Create the customer processes */
    srand (time (NULL));
    num_customers = rand_int (MIN_CUSTOMERS, MAX_CUSTOMERS);
    printf ("=== Launcher: creating %d customers ===\n", num_customers);
    fflush(stdout);
    for (i = 0; i < num_customers; i++){
        pid = fork ();
        switch (pid) {
            case -1:
                perror ("fork");
                exit (EXIT_FAILURE);

            case 0: /* Child code */
                /* Pass the customer number, and semahpore as arguments */
                snprintf (arg, sizeof (arg), "%d", i);
		snprintf (arg1, sizeof (arg1), "%d", WAITING_ROOM_SIZE);
           	snprintf (arg2, sizeof (arg2), "%s", "/waiting_room_eqm23");
            	snprintf (arg3, sizeof (arg3), "%s", "/barber_chair_eqm23");
            	snprintf (arg4, sizeof (arg4), "%s", "/done_with_customer_eqm23");
            	snprintf (arg5, sizeof (arg5), "%s", "/barber_bed_eqm23");

                execlp ("./customer", "customer", arg, arg1, arg2, arg3, arg4, arg5, (char *) NULL);
                perror ("exec");
                exit (EXIT_FAILURE);

            default:
                break;
        }
    }

    /* Wait for the barber and customer processes to finish */
    for (i = 0; i < (num_customers + 1); i++)
        wait (NULL);

    /* Unlink all the semaphores */
    sem_close(waiting_room);
    sem_close(barber_chair);
    sem_close(done_with_customer);
    sem_close(barber_bed);
    
    sem_unlink("/waiting_room_eqm23");
    sem_unlink("/barber_chair_eqm23");
    sem_unlink("/done_with_customer_eqm23");
    sem_unlink("/barber_bed_eqm23");
    
    exit (EXIT_SUCCESS);
}
