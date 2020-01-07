/* This program prints the locations of the two functions main() and func_a(). It then shows how the stack grows downward, letting func_a 
 * print the address of successive instantiations of its local variable stack_var. (stack_var is purposely declared auto, to emphasize that it's on 
 * the stack.) It then shows the location of memory allocated by alloca(). Finally it prints the locations of data and BSS variables, and then of 
 * memory allocated directly through sbrk().
 *
 * Author: Naga Kandasamy 
 * Date modified: 6/23/2015
 * Notes: From: Advanced UNIX programming from Stevens.
 *
 * Compile the code as follows: gcc -o address_space address_space.c -std=c99 -Wall
 *
 * Execute as follows:
 * $ ./address_space
 * */

#define _BSD_SOURCE
   
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <alloca.h>

void func_a (void);

int uninitialized_variable; /* Should be auto-initialized by exec to 0 and stored in the BSS segment. */
int initialized_variable = 5; /* Should be stored in the data segment. */

int 
main (int argc, char **argv)
{
    printf ("Text segment locations: \n");
    printf ("\t Address of main: %p\n", main);
    printf ("\t Address of func_a: %p\n", func_a);
		  
    printf ("Stack location:\n");
    func_a ();
	
    printf ("Heap location:\n");
    int chunk_size = 32; // Bytes of memory that we wish to allocate
    char *p = (char *) malloc (chunk_size);
    if (p != NULL) {
        printf ("\t Start of malloc()'ed array: %p\n", p);
        printf ("\t End of malloc()'ed array: %p\n", p + chunk_size - 1);
    }
		  
    printf("Data Locations:\n");
    printf("\tAddress of initialized_variable: %p\n", &initialized_variable);

    printf("BSS Locations:\n");
    printf("\tAddress of uninitialized_variable: %p\n", &uninitialized_variable);
	
    char *b = sbrk((ptrdiff_t) 32);   /* Grow address space */
    char *nb = sbrk((ptrdiff_t) 0);
	
    printf("Heap Locations:\n");
    printf("\tInitial end of heap: %p\n", b);
    printf("\tNew end of heap: %p\n", nb);

    b = sbrk((ptrdiff_t) -16);  /* Shrink it */
    nb = sbrk((ptrdiff_t) 0);
    printf("\tFinal end of heap: %p\n", nb);
	
    /* Simulate some processing */
    while (1);

    exit(0);
}


/* The function stack grows from high memory to low memory */
void 
func_a (void)
{
    static int level = 0;       /* recursion level */
    auto int local_variable;        /* automatic variable, on stack */
	
    if (++level == 3)           /* avoid infinite recursion */
        return;
		
    printf ("\tStack level %d: address of local_variable: %p\n", level, &local_variable);
    func_a ();                    /* recursive call */
}
