/* Program illustrating how the ptrace system call can be used to intercept system calls.
 * This example intercepts the write() system call, and modifies the contents of the 
 * buffer that is to be printed out by the child. 
 *
 * Authors: Quinn Mazzilli and Omer Odermis
 * Skeleton-Author: Naga Kandasamy
 * Date created: February 20, 2020
 * Date modified: March 20, 2020
 *
 * Compile as follows: gcc -o sandbox sandbox.c -std=c99 -Wall 
 * Execute as follows: ./sandbox ./program-name 
 * 
 * The tracee program is in the same directory as your simple_strace program.
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

        /* When wait() returns, the registers for the tracee that made the 
         * system call are filled with the syscall number and its 
         * arguments. However, the kernel has not yet serviced this system 
         * call. We can now gather the system call information. 
         *
         * On the x86-64 architecture, the following registers hold the 
         * relevant information.
         *
         * rax: system call number. For internal kernel purposes, the system call 
         *      number is stored in orig_rax rather than in rax.
         * rdi, rsi, rdx, r10, r8, r9: Upto six arguments passed via registers (note ordering of registers)
         *
         * The Linux system call table for x86-64 can be found at:
         *
         * https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/
         *
         * The table contains syscall numbers as well as information as to how arguments are 
         * passed to the system call. 
         *
         * The syscall table for x86-64 can also be downloaded from the Linux master branch:
         *
         * https://github.com/torvalds/linux/blob/master/arch/x86/entry/syscalls/syscall_64.tbl
         *
	 * Our focus: sys_openat
	 * orig_rax: 257
	 * rdi: dfd
	 * rsi: filename
	 * rdx: flags
	 * r10: mode
         */
        struct user_regs_struct regs;
        long syscall;
	int flags;
        unsigned char *buffer; 
	unsigned char tempAddr[] = "/tmp/";
	unsigned char *temp = &tempAddr[0];
	int blocked = 0;	

        ptrace (PTRACE_GETREGS, pid, 0, &regs);             /* Read tracee registers into regs */
        syscall = regs.orig_rax;                            /* System call number */
        switch (syscall) {
            case 257:
                /* Print syscall information */
		fprintf (stderr, "\nsys_open information:\n");
                fprintf (stderr, "%ld (%ld, %ld, %ld, %ld, %ld, %ld)\n",\
                         syscall,\
                         (long) regs.rdi, (long) regs.rsi, (long) regs.rdx,\
                         (long) regs.r10, (long) regs.r8, (long) regs.r9);
		flags = regs.rdx;
                /* Register rsi contains the starting address of the buffer to 
                 * be printed out. Register rdx contains the number of bytes to 
                 * write out. */
		fprintf(stderr, "Flags = %d\n", flags);
		buffer = read_buffer_contents (pid, 30, regs.rsi); // Read tracee buffer
                print_buffer_contents (buffer, 30); // Print contents of tracee buffer 
		fprintf(stderr, "\n");
		if (flags == (O_RDONLY))
			fprintf (stderr, "Read Only\n");
		else if (flags == (O_CREAT|O_RDWR)){
			fprintf (stderr, "Create | ReadWrite\n");
                	fprintf (stderr, "Tracee intends to open at address located at %p\n", (void *) regs.rsi);
			if (memcmp(temp, buffer, 5) != 0){
				fprintf(stderr, "You have been blocked\n");
				blocked = 1;
			}
		}
		

		free ((void *) buffer);
                buffer = NULL;
                break;

            default:
		fprintf(stderr, "%ld\n", syscall);
                break;

        }
        /* Execute the system call and stop the tracee on exiting the call */
	if (blocked) {
		ptrace(PTRACE_SYSCALL, pid, 0, 0);
		regs.rax = -EPERM;
		ptrace(PTRACE_SETREGS, pid, 0, &regs);
	} else {
		ptrace(PTRACE_SYSCALL, pid, 0, 0);
		blocked = 0;
	}
        //ptrace (PTRACE_SYSCALL, pid, 0, 0);
        waitpid (pid, 0, 0);

        /* Get the result of the system call */
        if (ptrace (PTRACE_GETREGS, pid, 0, &regs) == -1) {
            if (errno == ESRCH) {   /* System call was exit() or similar */ 
                fprintf (stderr, "\n");
                exit (regs.rdi);
            }                            
            
            perror ("ptrace");
            exit (EXIT_FAILURE);
        }
/*
	if (blocked) {
		// Set errno to EPERM (operation not permitted) in rax
		if (ptrace (PTRACE_SETREGS, pid, 0, &regs) == -1) {
			perror ("ptrace");
			exit (EXIT_FAILURE);
		}
	}
	*/

        /* Print result of system call */
        switch (syscall) {
            case SYS_write:
                fprintf (stderr, "Number of bytes written = %ld\n", (long) regs.rax);
                break;

            default:
                break;
        }
    }
    
    exit (EXIT_SUCCESS);
}

/* Read contents of the buffer for the write() system call located at specified address */
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
    i = 0;
    if (lop_off > 0) {
        data = ptrace (PTRACE_PEEKDATA, pid, (void *) (address + i * sizeof (long)), 0);
        c = (unsigned char *) &data;
        for (j = 0; j < lop_off; j++)
		buffer[idx++] = c[i++];
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
