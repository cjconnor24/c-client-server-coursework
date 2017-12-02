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

// READ STRING FROM SERVER
void read_string(int socket){

	size_t payload_length = sizeof(size_t);
	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	   

	//printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);//DEBUG

	// CALCULATE AND ALLOCATE MEMORY FOR THE RESULT
	size_t memallocation = (sizeof(char)*payload_length)+1;
	char *result = (char *)malloc(memallocation);

	if(result!=NULL){
	
	// INITIALISE EVERY PART OF MEM - WITHOUT WAS CAUSING VALG ISSUES
	memset(result,'\0',memallocation);
	
	readn(socket, (unsigned char *) result, payload_length);

	//printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);//DEBUG

	// PRINT STRING TO CONSOLE
	printf("%s\n",result);

	// FREE UP THE RESULT
	free(result);
	result = NULL;

	}

}

// READ UTSNAME STRUCT FROM SERVER
void read_server_details(int socket){

	size_t payload_length = sizeof(struct utsname);

	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	   

	//printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);//DEBUG

	struct utsname *uts = malloc(sizeof(struct utsname));
//	payload_length = sizeof(struct utsname);

	// MAKE SURE THE STRUCT ISN'T NULL BEFORE DOING SOMETHING WITH IT	
	if(uts!=NULL){

		readn(socket, (unsigned char *) uts, payload_length);

		printf("Node name:    %s\n", uts->nodename);
		printf("System name:  %s\n", uts->sysname);
		printf("Release:      %s\n", uts->release);
		printf("Version:      %s\n", uts->version);
		printf("Machine:      %s\n", uts->machine);
	
//	printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);//DEBUG

		//FREE UP THE STRUCT
		free(uts);
		uts = NULL;

	}

}

// SEND USERS MENU CHOICE ACROSS TO SERVER AND HANDLE THE RESPONSE
void send_menu_choice(int socket, char choice, read_cb readfunction){

	size_t payload_length = sizeof(char);

	//printf("payload_length is: %zu (%zu bytes)\n", payload_length, payload_length);

	    // SEND THE VALUE OF INT
	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));	
	writen(socket, (unsigned char *) &choice, payload_length);

	// CALLBACK DEPENDING ON DATA
	readfunction(socket);

}


//TODO: REMOVE
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

// DISPLAY MENU OPTIONS TO USER
void displaymenu()
{
    
	printf("-------------------\n");
	printf("   MAIN MENU\n");
	printf("-------------------\n");
	printf("[0]\tRe-display menu\n");
	printf("-------------------\n");
	printf("[1]\tGet Student Information \n");
	printf("[2]\tGet server timestamp\n");
	printf("[3]\tGet server information\n");
	printf("[4]\tGet server file list\n");
	printf("[6] Exit\n");

}

// SMALL HELPER FUNCTION TO FORMAT DATA CONSISTENTLY
void display_heading(char *message){

printf("\n%s\n-------------------\n",message);

}

// PROGRAM MAIN ENTRY POINT
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
    serv_addr.sin_port = htons(50031);
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

		// GET OPTION FROM USER
		printf("option> ");
		fgets(name, INPUTSIZ, stdin);	
		name[strcspn(name, "\n")] = 0;
		input = name[0];

		// ENSURE ONLY 1 CHARACTER LONG
		if (strlen(name) > 1)
		input = 'x';	

		switch (input) {
		case '0':
		    displaymenu();
		    break;
		case '1':
			display_heading("Student Information");
			send_menu_choice(sockfd, '1',read_string);
			break;
		case '2':
			display_heading("Server Timestamp");
			send_menu_choice(sockfd, '2',read_string);
			break;
		case '3':
			display_heading("Server Information");
			send_menu_choice(sockfd, '3',read_server_details);
			break;
		case '4':
			display_heading("Server File List");
			send_menu_choice(sockfd, '4',read_string);
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

	// GRACEFULLY CLOSE SOCKETS AND EXIT
	close(sockfd);
	exit(EXIT_SUCCESS);

} // end main()
