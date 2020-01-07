/* Program displays the argument and environment list provided to it. 
 *
 * Compile as follows: gcc -o envargs envargs.c -std=c99 -Wall
 * 
 * Author: Naga Kandsasmy
 * Date created: December 26, 2019
 * Date modified: 
 *
 */

#include <stdlib.h>
#include <stdio.h>

extern char **environ; /* Global variable pointing to the environment list. */

int 
main (int argc, char **argv) 
{
    int j;
    char **ep;

    /* Print the list of input arguments. */
    for (j = 0; j < argc; j++)
        printf ("argv[%d] = %s \n", j, argv[j]);

    /* Print the contents of the environment list. */
    for (ep = environ; *ep != NULL; ep++)
        printf ("environ: %s \n", *ep);

    exit (EXIT_SUCCESS);

}
