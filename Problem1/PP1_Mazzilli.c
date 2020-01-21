 /* Code for Programming Problem 1
  * written based on skeleton code
  * from primes.
  *
  * Skeleton Author: Naga Kandasamy
  * Author: Edward Mazzilli (eqm23)
  *
  * Date created: June 28, 2018
  * Date updated: January 22, 2020 
  * Date submitted: January 26, 2020 
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
 * program, either upon normal program termination or upon being terminated 
 * via a SINGINT or SIGQUIT signal. 
 */
void report()
{
    for (int j = 0; j < 5; j++){
       	fflush(stdout);
   	printf("%lu\n", last_prime[j]);
    }

}


int main (int argc, char** argv)
{
    unsigned long int num;
//    unsigned long int last_prime[5] = {0, 0, 0, 0, 0};
    num_found = 0;


    printf ("Beginning search for primes between 1 and %lu. \n", LONG_MAX);
    for (num = 1; num < 20; num++) { //LONG_MAX
        if (is_prime (num)) {
            num_found++;
	    printf ("\n");
            printf ("%lu\n", num);

	    fflush(stdout);
	    last_prime[(num_found % 5)] = num;
	    report();    
        }
    }

    exit (EXIT_SUCCESS);
}
