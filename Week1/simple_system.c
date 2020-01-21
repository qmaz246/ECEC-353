/* A simple implementation of the system() call.
 *
 * Author: Naga Kandasamy
 * Date created: January 3, 2019
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int
my_system (char *command)
{
    int status;
    int pid;

    pid = fork ();
    switch (pid) {
        case -1: /* Error */
            return -1;
        
        case 0: /* Child code */
            execl ("/bin/sh", "sh", "-c", command, (char *)NULL);
            exit (EXIT_SUCCESS);
        
        default: /* Parent code */
            if (waitpid (pid, &status, 0) == -1)
                return -1;
            else
                return status;
    }
}

