// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "rdwrn.h"
#include <sys/utsname.h>

#define INPUTSIZ 10

typedef struct {
    int id_number;
    int age;
    float salary;
} employee;

// CREATING TO ALLOW CALLBACK
typedef void (*read_cb)(int socket);

// how to send and receive structs
void send_and_get_employee(int socket, employee *e)  
{
    size_t payload_length = sizeof(employee);

    // send the original struct
    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));	
    writen(socket, (unsigned char *) e, payload_length);	 		

    // get back the altered struct
    readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	   
    readn(socket, (unsigned char *) e, payload_length);

    // print out details of received & altered struct
    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);    
} // end send_and_get_employee()

void read_string(int socket){

	size_t payload_length = sizeof(size_t);
	//size_t n =
	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	   

	//printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);

	char *result = malloc(sizeof(char)*payload_length);
//	result = 0;
	
	//n = 
	readn(socket, (unsigned char *) result, payload_length);

	//printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);

	printf("%s\n",result);

	free(result);
}

void read_server_details(int socket){

	size_t payload_length = sizeof(struct utsname);

	//size_t n =
	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	   

	//printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);

	struct utsname *uts = malloc(sizeof(struct utsname));

	//n = 
	readn(socket, (unsigned char *) uts, payload_length);

/*    if (uname(uts) == -1) {
        printf("This did not work");
        //perror("uname error");
        //exit(EXIT_FAILURE);
        return NULL;
    }*/

    printf("Node name:    %s\n", uts->nodename);
    printf("System name:  %s\n", uts->sysname);
    printf("Release:      %s\n", uts->release);
    printf("Version:      %s\n", uts->version);
    printf("Machine:      %s\n", uts->machine);

//	printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);

//	printf("%s\n",e);

	free(uts);
}

void send_menu_choice(int socket, char choice, read_cb readfunction){


	size_t payload_length = sizeof(char);

	//printf("payload_length is: %zu (%zu bytes)\n", payload_length, payload_length);

	    // SEND THE VALUE OF INT
	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));	
	writen(socket, (unsigned char *) &choice, payload_length);

	// CALLBACK DEPENDING ON DATA
	readfunction(socket);

/*	if(choice=='3'){
	read_server_details(socket);
	} else {

	read_string(socket);*/
}

/*	payload_length = sizeof(size_t);
	size_t n =readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	   

	printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);

	char *e = malloc(sizeof(char)*payload_length);

	n = readn(socket, (unsigned char *) e, payload_length);

	printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);

	printf("%s\n",e);

free(e);*/


// how to receive a string
void get_hello(int socket)
{
    char hello_string[32];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) hello_string, k);

    printf("Hello String: %s\n", hello_string);
    printf("Received: %zu bytes\n\n", k);
} // end get_hello()

void displaymenu()
{
    
	printf("[0]\tDisplay menu\n");
	printf("-------------------\n");
	printf("[1]\tGet Student Information \n");
	printf("[2]\tGet server timestamp\n");
	printf("[3]\tGet server information\n");
	printf("[4]\tGet server file list\n");
	printf("-------------------\n");
	printf("[6] Exit\n");

}

int main(void)
{
    // *** this code down to the next "// ***" does not need to be changed except the port number
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Error - could not create socket");
	exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;

    // IP address and port of server we want to connect to
    serv_addr.sin_port = htons(50001);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // try to connect...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
	perror("Error - connect failed");
	exit(1);
    } else
       printf("Connected to server...\n");

    // ***
    // your own application code will go here and replace what is below... 
    // i.e. your menu etc.


    // get a string from the server
    get_hello(sockfd);

char input;
    char name[10];

    displaymenu();

    do {
	printf("option> ");
	fgets(name, INPUTSIZ, stdin);	
	name[strcspn(name, "\n")] = 0;
	input = name[0];
	if (strlen(name) > 1)
	    input = 'x';	

	switch (input) {
	case '0':
	    displaymenu();
	    break;
	case '1':
	//printf("MENU ONE");
	send_menu_choice(sockfd, '1',read_string);
	    break;
	case '2':
send_menu_choice(sockfd, '2',read_string);
	    break;
	case '3':
send_menu_choice(sockfd, '3',read_server_details);
	    break;
	case '4':
	send_menu_choice(sockfd, '4',read_string);
	//sleep(4);
//	    printf("Goodbye!\n");
	    break;
	case '6':
	send_menu_choice(sockfd,'6',read_string);
	break;
	default:
	    printf("Invalid choice - 0 displays options...!\n");
		displaymenu();
	    break;
	}
    } while (input != '6');

    // send and receive a changed struct to/from the server
/*    employee *employee1;		
    employee1 = (employee *) malloc(sizeof(employee));

    // arbitrary values
    employee1->age = 23;
    employee1->id_number = 3;
    employee1->salary = 13000.21;

    int i;
    for (i = 0; i < 5; i++) {
         printf("(Counter: %d)\n", i);
	 send_and_get_employee(sockfd, employee1);
         printf("\n");
    }

    free(employee1);*/

    // *** make sure sockets are cleaned up

    close(sockfd);

    exit(EXIT_SUCCESS);
} // end main()
