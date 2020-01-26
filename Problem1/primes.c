 /* Skeleton code from primes.
  *
  * Author: Naga Kandasamy
  *
  * Date created: June 28, 2018
  * Date updated: January 16, 2020 
  *
  * Build your code as follows: gcc -o primes primes.c -std=c99 -Wall
  * 
  * Team Members: Quinn Mazzilli & Omer Odemis
  *
  * */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <signal.h>

#define FALSE 0 // FALSE = 0
#define TRUE !FALSE // TRUE is the opposite of FALSE | TRUE = 1

static void custom_signal_handler (int);

unsigned long int num_found; /* Number of prime numbers found */
unsigned long int last_prime[5] = {0, 0, 0, 0, 0};

int 
is_prime (unsigned int num)
{
    unsigned long int i;

    if (num < 2) {
        return FALSE;
    }
    else if (num == 2) {
        return TRUE;
    }
    else if (num % 2 == 0) {
        return FALSE;
    }
    else {
        for (i = 3; (i*i) <= num; i += 2) {
            if (num % i == 0) {
                return FALSE;
            }
        }
        return TRUE;
    }
}

/* Complete the function to display the last five prime numbers found by the 
 *  * program, either upon normal program termination or upon being terminated 
 *   * via a SINGINT or SIGQUIT signal. 
 *    */
void 
report ()
{
    printf("\n");
    for (int j = 0; j < 5; j++){
       	fflush(stdout);
   	printf("%lu\n", last_prime[j]);
    }
}

int 
main (int argc, char** argv)
{
    signal (SIGINT, custom_signal_handler);
    signal (SIGQUIT, custom_signal_handler);
//    unsigned long int last_prime[5] = {0, 0, 0, 0, 0};
    unsigned long int num;

    num_found = 0;

    printf ("Beginning search for primes between 1 and %lu. \n", LONG_MAX);
    for (num = 1; num < 200; num++) {
        if (is_prime (num)) {
            num_found++;
	    printf ("\n");
            printf ("%lu\n", num);

	    fflush(stdout);
	    last_prime[(num_found % 5)] = num;
		
	    sleep(1);
        }
    }
    // print last 5 prime numbers on program completion
    report(); 
    exit (EXIT_SUCCESS);
}

static void 
custom_signal_handler (int signalNumber)
{
    switch (signalNumber){
        case SIGINT:
            signal (SIGINT, custom_signal_handler); /* Restablish the signal handler for the next occurrence */
            printf ("Ouch...Caught the Control+C signal. \n");
	    report();
	    break;

         case SIGQUIT: 
            printf ("Caught the Quit signal. \n");
            printf ("Going through an orderly shutdown process. \n");
	    report();
            exit (EXIT_SUCCESS);

         default: 
            break;

    }
}
