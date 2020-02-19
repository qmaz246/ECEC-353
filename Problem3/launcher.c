/* Program to launch the barber and customer threads. 
 *
 * Compile as follows: gcc -o launcher launcher.c -std=c99 -Wall -lpthread -lm
 *
 * Run as follows: ./launcher 
 *
 * Author: Naga Kandasamy
 * Date: February 7, 2020
 *
 * Student/team: FIXME
 * Date: FIXME
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

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
    char arg1[20];
    char arg2[20];
    char arg3[20];
    char arg4[20];
    char arg5[20];


    /* FIXME: create the needed *named* semaphores and initialize them to the correct values */
    sem_t *waiting_room = sem_open("/waiting_room_eqm23", O_CREAT, S_IRUSR | S_IWUSR, WAITING_ROOM_SIZE);
    sem_t *barber_chair = sem_open("/barber_chair_eqm23", O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t *done_with_customer = sem_open("/done_with_customer_eqm23", O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem_t *barber_bed = sem_open("/barber_bed_eqm23", O_CREAT, S_IRUSR | S_IWUSR, 0);

    
    /* Create the barber process. */
    printf ("Launcher: creating barber proccess\n"); 
    pid = fork ();
    switch (pid) {
        case -1:
            perror ("fork");
            exit (EXIT_FAILURE);

        case 0: /* Child process */
            /* Execute the barber process, passing the waiting room size and semaphores */ 
             
            snprintf (arg1, sizeof (arg1), "%d", WAITING_ROOM_SIZE);
            snprintf (arg2, sizeof (arg2), "%s", "/done_with_customer_eqm23");
            snprintf (arg3, sizeof (arg3), "%s", "/barber_bed_eqm23");

	    
            execlp ("./barber", "barber", arg1, arg2, arg3, (char *) NULL);
            perror ("exec"); /* If exec succeeds, we should get here */
            exit (EXIT_FAILURE);
        
        default:
            break;
    }

    /* Create the customer processes */
    srand (time (NULL));
    num_customers = rand_int (MIN_CUSTOMERS, MAX_CUSTOMERS);
    printf ("Launcher: creating %d customers\n", num_customers);

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
    if(sem_unlink("/waiting_room_eqm23") == 0 || sem_unlink("/barber_chair_eqm23") == 0 || sem_unlink("/done_with_customer_eqm23") == 0 || sem_unlink("/barber_bed_eqm23") == 0){
	perror("sem_unlink");
	exit(EXIT_FAILURE);
    }
    
    exit (EXIT_SUCCESS);
}
