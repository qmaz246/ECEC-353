/* Skeleton code for the client side code. 
 *
 * Compile as follows: gcc -o chat_client chat_client.c -std=c99 -Wall -lrt
 *
 * Author: Naga Kandsamy
 * Date created: January 28, 2020
 * Date modified:
 *
 * Student/team name: FIXME
 * Date created: FIXME 
 *
*/

#define _POSIX_C_SOURCE 2 // For getopt()

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "msg_structure.h"

void 
print_main_menu (void)
{
    printf ("\n'B'roadcast message\n");
    printf ("'P'rivate message\n");
    printf ("'E'xit\n");
    return;
}

int 
main (int argc, char **argv)
{
    char user_name[USER_NAME_LEN];
    char initiate[50];
    char client_name;

    if (argc != 2) {
        printf ("Usage: %s user-name\n", argv[0]);
        exit (EXIT_FAILURE);
    }

    strcpy (user_name, argv[1]); /* Get the client user name */

    mqd_t server_mqd = mq_open("/eqm23_oto25_queue", O_WRONLY);
    if (server_mqd == (mqd_t)-1) {
        perror ("mq_open");
        exit (EXIT_FAILURE);
    }
    sprintf(initiate, "phoney text:C:%s", user_name);
    mq_send(server_mqd, initiate, strlen(initiate), 0); 			
 
    sleep(4);

    sprintf(&client_name, "/eqm23_oto25_%s", user_name);
    const char *queue_name = &client_name;
    mqd_t client_mqd = mq_open(queue_name, O_RDONLY);
    if (client_mqd == (mqd_t)-1) {
	printf("client\n");
	fflush(stdout);
        perror ("mq_open");
        exit (EXIT_FAILURE);
    }
    printf ("User %s connecting to server\n", user_name);
    

    /* Operational menu for client */
    char line; //, *option;
    char message[4096];
    char private[4096];
    struct mq_attr attr;
    ssize_t in_msg;
    unsigned int priority;
    void *msg;

    while (1) {
        print_main_menu ();
        line = getchar();

        switch (line) {
            case 'B':
		/* sends message to server, appended by username */
		while(getchar() != '\n');
		fgets(message, 4096, stdin);	
		message[strcspn(message, "\n")] = 0;	
		strcat(strcat(message, ":B:"), user_name);
		printf("%s\n", message);
		mq_send(server_mqd, message, strlen(message), 0); 			
		printf("message sent\n");
		fflush(stdout);

		sleep(1);
                break;

            case 'P':
                /* FIXME: Get name of private user and send the private 
                 * message to server to be sent to private user */
		/* test for nothing in queue message recieve */
		while(getchar() != '\n');
		printf("To who do you want to send this message? ");
		fflush(stdout);
		fgets(private, 4096, stdin);
		message[strcspn(private, "\n")] = 0;	
		fgets(message, 4096, stdin);	
		message[strcspn(message, "\n")] = 0;	
		strcat(strcat(message, ":P:"), strcat(strcat(private,"|"),user_name));
		printf("%s\n", message);

		mq_send(server_mqd, message, strlen(message), 0); 			
		printf("message sent\n");
		fflush(stdout);

		sleep(1);

                break;

            case 'E':
                printf ("Chat client exiting\n");
                /* FIXME: Send message to server that we are exiting */
		sprintf(message, "has left the server:E:%s", user_name);
		mq_send(server_mqd, message, strlen(message), 0); 			
                exit (EXIT_SUCCESS);

            default:
                printf ("Unknown option\n");
                break; 
		
        }
	msg = malloc(sizeof(attr.mq_msgsize));

	if(mq_getattr(client_mqd, &attr) == -1){
		printf("Client\n");
		fflush(stdout);
		perror("mq_getattr");
		exit(EXIT_FAILURE);
	}else if(attr.mq_curmsgs > 0){
		in_msg = mq_receive(client_mqd, msg, attr.mq_msgsize, &priority);
		if (in_msg == -1){
			perror("mq_recieve");
			exit(EXIT_FAILURE);
		}
		//printf("Read %ld bytes; priority = %u \n", (long) in_msg, priority);
		if (write (STDOUT_FILENO, msg, in_msg) == -1) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	} 
    }
         
    exit (EXIT_SUCCESS);
}
