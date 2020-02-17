/* Skeleton code for the server side code. 
 * 
 * Compile as follows: gcc -o chat_server chat_server.c -std=c99 -Wall -lrt
 *
 * Author: Naga Kandasamy
 * Date created: January 28, 2020
 *
 * Student/team name: FIXME
 * Date created: FIXME  
 *
 */

#define _POSIX_C_SOURCE 2

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "msg_structure.h"

/* FIXME: Establish signal handler to handle CTRL+C signal and 
 * shut down gracefully. 
 */

// Sample Code Included:
// create_mq.c
// 
//

static void 
usage_error (const char *program_name)
{
    fprintf (stderr, "Usage: %s [-cx] [-m maxmsg] [-s msgsize] mq-name [octal-perms] \n", program_name);

    fprintf (stderr, " -c               Create queue (O_CREAT) \n");
    fprintf (stderr, " -m maxmsg        Set maximum number of messages \n");
    fprintf (stderr, " -s msgsize       Set maximum message size \n");
    fprintf (stderr, " -x               Create exclusively (O_EXCL) \n");

    exit (EXIT_FAILURE);
}

int 
main (int argc, char **argv) 
{
    int flags, c_flags, opt;
    mode_t perms;
    mqd_t server_mqd;
    struct mq_attr attr, *attrp;
    void *msg;
    ssize_t in_msg;
    unsigned int priority;

    /* Set the default message queue attributes. */
    attrp = NULL;
    attr.mq_maxmsg = 10;    /* Maximum number of messages on queue */
    attr.mq_msgsize = 2048; /* Maximum message size in bytes */
    flags = O_RDONLY;         /* Create or open the queue for reading and writing */
    c_flags = O_WRONLY;
    c_flags |= O_CREAT;
    /* Parse command-line arguments and set the message queue attributes as specified */
    while ((opt = getopt (argc, argv, "cm:s:x")) != -1) {
        switch (opt) {
            case 'c': 
                flags |= O_CREAT;   /* Create the MQ if it doesn't already exist */
                break;

            case 'm':
                attr.mq_maxmsg = atoi (optarg);
                attrp = &attr;
                break;

            case 's':
                attr.mq_msgsize = atoi (optarg);
                attrp = &attr;
                break;

            case 'x':
                flags |= O_EXCL;    /* With O_CREAT, create the queue exclusively */
                break;

            default: 
                usage_error (argv[0]);
        }
    }

    /* In case of incorrect cmd-line agruments, show usage to user. */
    if (optind >= argc)
        usage_error (argv[0]);
    
    perms = S_IRUSR | S_IWUSR;  /* rw------- permissions on the queue */

    server_mqd = mq_open (argv[optind], flags, perms, attrp);
    if (server_mqd == (mqd_t)-1) {
        perror ("mq_open");
        exit (EXIT_FAILURE);
    }
    char* message;
    char option[2], name1[20];
    char queue_name[100];
    char queue; 
    while (1) {
       /* FIXME: Server code here */
	msg = malloc(sizeof(attr.mq_msgsize));		
	if(mq_getattr(server_mqd, &attr) == -1){
		perror("mq_getattr");
		exit(EXIT_FAILURE);
	}else if(attr.mq_curmsgs > 0){
		in_msg = mq_receive(server_mqd, msg, attr.mq_msgsize, &priority);
		if (in_msg == -1){
			perror("mq_recieve");
			exit(EXIT_FAILURE);
		}
		//printf("Read %ld bytes; priority = %u \n", (long) in_msg, priority);
		/*if (write (STDOUT_FILENO, msg, in_msg) == -1) {
			perror("write");
			exit(EXIT_FAILURE);
		}*/
		printf("I got this far\n");
		fflush(stdout);
		message = strtok(msg, ":");
		sprintf(option, "%s", strtok(NULL, ":"));
		switch(atoi(option)){

			case 'B':
				break;

			case 'P':
				break;

			case 'E':
				break;

			default:
				printf("I have gotten a C in this class\n");
				fflush(stdout);
				strcat(queue_name, "/eqm23_oto25_");
				sprintf(name1, "%s", strtok(NULL, ":"));
				strcat(queue_name, name1);
				sprintf(&queue, "%s", queue_name);
				const char *client_queue = &queue;
				mqd_t client_mqd = mq_open (client_queue, c_flags, perms, attrp);
				if (client_mqd == (mqd_t)-1) {
					printf("server\n");
					fflush(stdout);
        				perror ("mq_open");
        				exit (EXIT_FAILURE);
    				}
				printf("successfull server\n");
				fflush(stdout);
				break;

		}
		sprintf(name1, "%s", strtok(NULL, ":"));
		printf("%s\n", message);
		printf("%s\n", option);
		printf("%s\n", name1);
		fflush(stdout);
	}
	
    }
    exit (EXIT_SUCCESS);
}
