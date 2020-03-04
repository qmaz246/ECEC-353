/*  Purpose: Calculate definite integral using trapezoidal rule.
 *
 * Input:   a, b, n, num_threads
 * Output:  Estimate of integral from a to b of f(x)
 *          using n trapezoids, with num_threads.
 *
 * Compile: gcc -o trap trap.c -O3 -std=c99 -Wall -lpthread -lm
 * Usage:   ./trap
 *
 * Note:    The function f(x) is hardwired.
 *
 * Author: Naga Kandasamy
 * Date modified: February 21, 2020
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

double compute_using_pthreads (float, float, int, float, int);
double compute_gold (float, float, int, float);
void *compute_silver (void *);


typedef struct args_for_thread
{
	int	tid;
	float	a;
	float	b;
	int	n;
	float	h;
	double 	*partial_integral; 

} ARGS_FOR_THREAD;

int main (int argc, char **argv) 
{
	if (argc < 5) {
        	printf ("Usage: %s lower-limit upper-limit num-trapezoids num-threads\n", argv[0]);
        	printf ("lower-limit: The lower limit for the integral\n");
        	printf ("upper-limit: The upper limit for the integral\n");
        	printf ("num-trapezoids: Number of trapeziods used to approximate the area under the curve\n");
        	printf ("num-threads: Number of threads to use in the calculation\n");
        	exit (EXIT_FAILURE);
    	}

	float a = atoi (argv[1]); /* Lower limit */
	float b = atof (argv[2]); /* Upper limit */
	float n = atof (argv[3]); /* Number of trapezoids */

	float h = (b - a)/(float) n; /* Base of each trapezoid */  
	printf ("The base of the trapezoid is %f\n", h);

	double reference = compute_gold (a, b, n, h);
	printf ("Reference solution computed using single-threaded version = %f\n", reference);

	/* Write this function to complete the trapezoidal rule using pthreads. */
  	int num_threads = atoi (argv[4]); /* Number of threads */
	double pthread_result = compute_using_pthreads (a, b, n, h, num_threads);
	printf ("Solution computed using %d threads = %f\n", num_threads, pthread_result);

   	exit (EXIT_SUCCESS);
} 

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Defines the integrand
 * Input args:  x
 * Output: sqrt((1 + x^2)/(1 + x^4))

 */
float f (float x) 
{
	return sqrt ((1 + x*x)/(1 + x*x*x*x));
}

/*------------------------------------------------------------------
 * Function:    compute_gold
 * Purpose:     Estimate integral from a to b of f using trap rule and
 *              n trapezoids using a single-threaded version
 * Input args:  a, b, n, h
 * Return val:  Estimate of the integral 
 */
double compute_gold (float a, float b, int n, float h) 
{
  	double integral;
  	int k;
	
  	integral = (f(a) + f(b))/2.0;

  	for (k = 1; k <= n-1; k++) 
    		integral += f(a+k*h);
   
   	integral = integral*h;

   	return integral;
}  

/* FIXME: Complete this function to perform the trapezoidal rule using pthreads. */
double compute_using_pthreads (float a, float b, int n, float h, int num_threads)
{
	double integral = 0.0;
	pthread_t	*thread_id;
	pthread_attr_t	attributes;
	pthread_attr_init(&attributes);
	int i;
	
	thread_id = (pthread_t *) malloc (sizeof(pthread_t) * num_threads);
	double *partial_integral = (double *) malloc (sizeof (double) * num_threads);
	ARGS_FOR_THREAD *args_for_thread = (ARGS_FOR_THREAD *) malloc (sizeof (ARGS_FOR_THREAD) * num_threads);

	float ab_step = (b-a)/num_threads;
	float divided_n = n/num_threads;

	for(i = 0; i < num_threads; i++){
		args_for_thread[i].tid = i;
		args_for_thread[i].a = a;
		args_for_thread[i].b = a+ab_step;
		args_for_thread[i].n = divided_n;
		args_for_thread[i].h = h;
		args_for_thread[i].partial_integral = partial_integral;
		a = a + ab_step;	
	}

	for(i = 0; i < num_threads; i++)
		pthread_create(&thread_id[i], &attributes, compute_silver, (void *) &args_for_thread[i]);

	for(i = 0; i < num_threads; i++)
		pthread_join(thread_id[i], NULL);

	for(i = 0; i < num_threads; i++)
		integral += partial_integral[i];

	// Free data structures
    	free ((void *) thread_id);
    	free ((void *) args_for_thread);
    	free ((void *) partial_integral);
	
	return integral;
}

void *
compute_silver (void *args)
{
	int k;
	ARGS_FOR_THREAD *args_for_me = (ARGS_FOR_THREAD *) args;

	double partial_integral;

	partial_integral = (f(args_for_me->a) + f(args_for_me->b))/2.0;

  	for (k = 1; k <= args_for_me->n-1; k++) 
    		partial_integral += f(args_for_me->a+k*args_for_me->h);
   
   	partial_integral = partial_integral*args_for_me->h;

	args_for_me->partial_integral[args_for_me->tid] = (float) partial_integral;
	pthread_exit((void *) 0);
}


