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
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define MIN_TIME 2
#define MAX_TIME 5
#define TIME_OUT 10
#define TRUE 1
#define FALSE 0 
#define ALARM_TIME 10

static void alarm_handler (int); /* Signal handler to catch the ALRM signal */

int barber_go_home = FALSE;

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
//    int oflag = O_CREAT;

    srand (time (NULL)); 

    /* Set up our signal handler to catch SIGALRM */
    if (signal (SIGALRM, alarm_handler) == SIG_ERR){ 
        printf ("Cannot catch SIGALRM \n");
        exit (0);
    }

    printf ("Barber: Opening up the shop\n");
    fflush(stdout); 
    int waiting_room_size = atoi (argv[1]);
    printf ("Barber: size of waiting room = %d\n", waiting_room_size);
    fflush(stdout);

    /* Unpack the semaphore names from the rest of the command-line arguments 
     * and open them for use.
     */
    sem_t *done_with_customer = sem_open(argv[2], 0);
    sem_t *barber_bed = sem_open(argv[3], 0);
	

    /* Stay in a loop, cutting hair for customers as they arrive. 
     * If no customer wakes the barber within TIME_OUT seconds, barber
     * closes shop and goes home.
     * Still need to implement ^^^^
     */

    while(!barber_go_home){
	printf("Barber is sleeping\n");
	fflush(stdout);
	alarm (ALARM_TIME); /* Set our alarm to ring at the specified interval */ 
	sem_wait(barber_bed);

	if(!barber_go_home){
		printf("Barber: Snip Snip\n");	
		fflush(stdout);			
	    	cut_hair (MIN_TIME, MAX_TIME);
		printf("Barber: Ok sir you is done\n");
		fflush(stdout);
		sem_post(done_with_customer);
	}
	else{
	    	printf ("Barber: all done for the day\n");
		fflush(stdout);			
	}

    }
    exit (EXIT_SUCCESS);
}

/* The user-defined signal handler for SIGALRM */
static void
alarm_handler (int dummyNumber)
{
    int dumber = dummyNumber;
    dumber = dumber * 2;
    printf ("*looks right*\n*looks left*\nAyy nobody's coming i should just go home\n");
    barber_go_home = TRUE;
    
}
