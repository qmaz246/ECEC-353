 /* Skeleton code from primes.
  *
  * Author: Naga Kandasamy
  *
  * Date created: June 28, 2018
  * Date updated: January 16, 2020 
  *
  * Build your code as follows: gcc -o primes primes.c -std=c99 -Wall
  *
  * */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#define FALSE 0
#define TRUE !FALSE

unsigned long int num_found; /* Number of prime numbers found */

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
 * program, either upon normal program termination or upon being terminated 
 * via a SINGINT or SIGQUIT signal. 
 */
void 
report ()
{
}

int 
main (int argc, char** argv)
{
    unsigned long int num;
    num_found = 0;

    printf ("Beginning search for primes between 1 and %lu. \n", LONG_MAX);
    for (num = 1; num < LONG_MAX; num++) {
        if (is_prime (num)) {
            num_found++;
            printf ("%lu \n", num);
        }
    }

    exit (EXIT_SUCCESS);
}
