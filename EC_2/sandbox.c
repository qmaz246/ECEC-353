/* Program to effectively block if a tracee tries to create a new file
 * outside the /tmp/ directory.
 *
 * Author: Quinn Mazzilli and Omer Odermis
 * Date created: March 18, 2020
 *
 * Compile as follows: gcc -o simple_strace simple_strace.c -std=99 -Wall 
 * Execute as follows: ./sandbox ./program-name 
 *
 *
 */

/* Includes from the C standard library */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
/* POSIX includes */
#include <unistd.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>

/* Linux includes */
#include <syscall.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>

/* Function prototypes */
unsigned char *read_buffer_contents (pid_t, unsigned int, long);
void print_buffer_contents (unsigned char *, unsigned int);

int 
main (int argc, char **argv)
{
    if (argc != 2) {
        printf ("Usage: %s ./program-name\n", argv[0]);
        exit (EXIT_FAILURE);
    }

    /* Extract program name from command-line argument (without the ./) */
    char *program_name = strrchr (argv[1], '/');
    if (program_name != NULL)
        program_name++;
    else
        program_name = argv[1];

    pid_t pid;
    pid = fork ();
    switch (pid) {
        case -1: /* Error */
            perror ("fork");
            exit (EXIT_FAILURE);

        case 0: /* Child code */
            /* Set child up to be traced */
            ptrace (PTRACE_TRACEME, 0, 0, 0);
            printf ("Executing %s in child code\n", program_name);
            execlp (argv[1], program_name, NULL);
            perror ("execlp");
            exit (EXIT_FAILURE);
    }

    /* Parent code. Wait till the child begins execution and is 
     * stopped by the ptrace signal, that is, synchronize with 
     * PTRACE_TRACEME. When wait() returns, the child will be 
     * paused. */
    waitpid (pid, 0, 0); 

    /* Send a SIGKILL signal to the tracee if the tracer exits.  
     * This option is useful to ensure that tracees can never 
     * escape the tracer's control.
     */
    ptrace (PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);

    /* Intercept and examine the system calls made by the tracee */
    while (1) {
        /* Wait for the tracee to begin the next system call */
        ptrace (PTRACE_SYSCALL, pid, 0, 0);
        waitpid (pid, 0, 0);

        /* rax: system call number. For internal kernel purposes, the system call 
         * number is stored in orig_rax rather than in rax.
         * rdi, rsi, rdx, r10, r8, r9: Upto six arguments passed via registers (note ordering)
         *
         * What we focus on: 
         * %rdi: address
         * %rsi: flags
         */
        struct user_regs_struct regs;
	unsigned char *buffer;
        ptrace (PTRACE_GETREGS, pid, 0, &regs);                         /* Read tracee registers into regs */
        long syscall = regs.orig_rax;                                   /* System call number */
	long flags;

	switch (syscall) {
            case SYS_open:
		/* Print syscall information */
		fprintf (stderr, "\nsyscall information:\n");
                fprintf (stderr, "address: %ld (flags: %ld, %ld, %ld, %ld, %ld, %ld)\n",\
                         syscall,\
                         (long) regs.rdi, (long) regs.rsi, (long) regs.rdx,\
                         (long) regs.r10, (long) regs.r8, (long) regs.r9);
		flags = (long) regs.rsi;
		if (flags == (O_RDONLY)){
			printf("Flags = O_RDONLY\n");
		} else if (flags == (O_CREAT|O_RDWR)){
			printf("Flags = O_CREAT|O_RDWR\n");
		} else{
			printf("Something else\n");
		}	

		break;

	    default:
		break;
	}

        /* Run the system call and stop on exiting the call */
        ptrace (PTRACE_SYSCALL, pid, 0, 0);
        waitpid (pid, 0, 0);

        /* Get the result of the system call */
        if (ptrace (PTRACE_GETREGS, pid, 0, &regs) == -1) {
            if (errno == ESRCH) 
                exit (regs.rdi);                                        /* System call was exit() or similar */
            perror ("ptrace");
            exit (EXIT_FAILURE);
        }

        /* Print result of system call */
        printf (" = %ld\n", (long) regs.rax);
    }

    exit (EXIT_SUCCESS);
}

unsigned char * 
read_buffer_contents (pid_t pid, unsigned int count, long address)
{
    unsigned char *c, *buffer;
    long data;
    unsigned int i, j, idx, num_words, lop_off;
        
    /* Allocate space to store the contents of the buffer */
    buffer = (unsigned char *) malloc (sizeof (unsigned char) * count);

    num_words = count/sizeof (long);
    lop_off = count - num_words * sizeof (long);
    idx = 0;
    
    /* Peek into the tracee's address space, each time returning a word (eight bytes) of data. 
     * Typecast the word into characters and store in our buffer.
     */
    for (i = 0; i < num_words; i++) {
        data = ptrace (PTRACE_PEEKDATA, pid, (void *) (address + i * sizeof (long)), 0);
        c = (unsigned char *) &data;
        for (j = 0; j < sizeof (long); j++)
            buffer[idx++] = c[j];
    }

    /* If the number of bytes is not a perfect multiple of the word length,
     * read and store the rest of the characters in our buffer.
     */
    if (lop_off > 0) {
        data = ptrace (PTRACE_PEEKDATA, pid, (void *) (address + i * sizeof (long)), 0);
        c = (unsigned char *) &data;
        for (j = 0; j < lop_off; j++)
            buffer[idx++] = c[j];
    }

    return buffer;
}


/* Helper function to print contents of buffer */
void 
print_buffer_contents (unsigned char *buffer, unsigned int count)
{
    int i;
	
    for (i = 0; i < count; i++)
        putc (buffer[i], stderr);
    
    return;
}
