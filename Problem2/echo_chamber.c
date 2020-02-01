/* This program is to create echo chamber between a parent process and its child.
 *
 * Compile the code as follows: gcc -o simple_pipe simple_pipe.c -std=c99 -Wall
 *
 * The user enters a string to the program through the stdin. The parent process creates the pipes, forks a child, and passes 
 * this string via the pipe for the child to convert to upper case before sending it back to the parent to print.
 *
 * Skeleton Code - Author: Naga Kandasamy
 * Date created: December 22, 2008
 *
 *
 * Authors: Edward Mazzilli & Omer Somethingorother
 * Date due: Febrary 2, 2020
*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#define BUF_LEN 256

int 
main (int argc, char **argv)
{
    int pid;      
    int fd[2];    /* Array to hold the read and write file descriptors for the pipe. */
    int fd2[2];   /* second array for second pipe */
    int j = 0;
    int n; 
    int m;
    char ch;
    char buffer[BUF_LEN];
    char c_buff[BUF_LEN];
    int status;

   /* if (argc < 2) {             * Check if we have command line arguments. 
      printf ("Usage: %s string\n", argv[0]);
      exit (EXIT_SUCCESS);
    }*/ 
    while(1){
    	if (pipe (fd) < 0) {        /* Create the pipe data structure. */
        	perror ("pipe");
        	exit (EXIT_FAILURE);
    	}

    	if (pipe (fd2) < 0) {	/* Create second pipe data structure. */
		perror ("pipe");
		exit (EXIT_FAILURE);
    	}


    	if ((pid = fork ()) < 0) {  /* Fork the parent process. */
       		perror ("fork");
       		exit (EXIT_FAILURE);
    	}
	if (pid > 0) {                                                                 			/* Parent code */
		printf ("Enter text for the parent to send to the child: ");
		fgets(buffer, BUF_LEN, stdin);								/* Obtain string from STDIN */
    		strtok(buffer, "\n");	
		close (fd[0]);                                                              		/* Close the reading end of parent pipe fd*/
        	printf ("PARENT: Writing %d bytes to the pipe fd: \n", (int) strlen (buffer));
        	write (fd[1], buffer, strlen (buffer));                                     		/* Write the buffer contents to the pipe fd*/
    	}
    	else {                                                                          		/* Child code */
    	    close (fd[1]);                                                              		/* Close writing end of child pipe fd*/
    	    n = read (fd[0], buffer, BUF_LEN);                                          		/* Read n bytes from the pipe fd*/
    	    buffer[n] = '\0';                                                           		/* Terminate the string */
	    strcpy (c_buff, buffer);						    		       	/* Copy the piped string into c_buff */
	    close (fd2[0]);								    		/* Close the reading end of child pipe fd2*/
	    printf ("CHILD has received: %s , proceeding to upper-casify\n", buffer);
            while (c_buff[j]) { 
            	ch = c_buff[j]; 
        	c_buff[j] = toupper(ch);
        	j++;
            } 
            printf ("CHILD: Writing %d bytes to the pipe fd2: \n", (int) strlen (c_buff));
	    write (fd2[1], c_buff, strlen (c_buff));
            exit (EXIT_SUCCESS);                                                        /* Child exits */
	}
    
  
    	/* Parent code */
    	pid = waitpid (pid, &status, 0);                                                /* Wait for child to terminate */
    	printf ("PARENT: Child has terminated. \n"); 
    	close (fd2[1]);
    	m = read (fd2[0], c_buff, BUF_LEN);
    	c_buff[m] = '\0';
    	printf ("PARENT has received: %s \n", c_buff);
    }
    exit (EXIT_SUCCESS);
}
