/* Skeleton code for the barber program. 
 *
 * Compile as follows: gcc -o barber barber.c -std=c99 -Wall -lpthread -lm 
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

#define MIN_TIME 2
#define MAX_TIME 5
#define TIME_OUT 10

/* Simulate cutting hair, by sleeping for some time between [min. max] in seconds */
void
cut_hair (int min, int max) 
{
    sleep ((int) floor((double) (min + ((max - min + 1) * ((float) rand ()/(float) RAND_MAX)))));
    return;
}

int 
main (int argc, char **argv)
{
    srand (time (NULL)); 

    printf ("Barber: Opening up the shop\n");
    
    int waiting_room_size = atoi (argv[1]);
    printf ("Barber: size of waiting room = %d\n", waiting_room_size);


    /* FIXME: Unpack the semaphore names from the rest of the command-line arguments 
     * and open them for use.
     */
    sem_t *waiting_room = sem_open("/waiting_room_eqm23", O_CREAT, S_IRUSR | S_IWUSR, WAITING_ROOM_SIZE);
    sem_t *barber_chair = sem_open("/barber_chair_eqm23", O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t *done_with_customer = sem_open("/done_with_customer_eqm23", O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem_t *barber_bed = sem_open("/barber_bed_eqm23", O_CREAT, S_IRUSR | S_IWUSR, 0);
	

    /* FIXME: Stay in a loop, cutting hair for customers as they arrive. 
     * If no customer wakes the barber within TIME_OUT seconds, barber
     * closes shop and goes home.
     */


    cut_hair (MIN_TIME, MAX_TIME);

    printf ("Barber: all done for the day\n");

    exit (EXIT_SUCCESS);
}
