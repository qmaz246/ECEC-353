/* Skeleton code for the customer program. 
 *
 * Compile as follows: gcc -o customer customer.c -std=c99 -Wall -lpthread -lm 
 *
 * Author: Naga Kandasamy
 * Date: February 7, 2020
 *
 * */

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

#define MIN_TIME 2
#define MAX_TIME 10

/* Simulate walking to barber shop, by sleeping for some time between [min. max] in seconds */
void
walk (int min, int max) 
{
    sleep ((int) floor((double) (min + ((max - min + 1) * ((float) rand ()/(float) RAND_MAX)))));
    return;
}

int 
main (int argc, char **argv)
{
    srand (time (NULL)); 
//    int oflag = O_CREAT;

    int customer_number = atoi (argv[1]);
    /* Unpack the semaphore names from the rest of the arguments */
    sem_t *waiting_room = sem_open(argv[2], 0);
    sem_t *barber_chair = sem_open(argv[3], 0);
    sem_t *done_with_customer = sem_open(argv[4], 0);
    sem_t *barber_bed = sem_open(argv[4], 0);

    
    printf ("Customer %d: Aight, time to get a new doo\n", customer_number);
    fflush(stdout);			
    walk (MIN_TIME, MAX_TIME);

    /* Get hair cut by barber and go home. */
    printf("Customer %d: I'm here bitches\n", customer_number);
    fflush(stdout);
    sem_wait(waiting_room);

    // Deadlocks here 
    printf("Mr. Customer %d Have a seat\n", customer_number);
    fflush(stdout);
    sem_wait(barber_chair);
    sem_post(waiting_room);
    sem_post(barber_bed);
    sem_wait(done_with_customer);
    sem_post(barber_chair);

    printf ("Customer %d: Looks ok, i won't kill you today\n", customer_number);
    fflush(stdout);

    exit (EXIT_SUCCESS);
}
