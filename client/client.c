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

// CREATING TO ALLOW CALLBACK
typedef void (*read_cb)(int socket);
void send_string(int socket,char* response);
char *read_string(int socket);
void send_menu_choice(int socket, char choice, read_cb readfunction);

void read_get_file(int socket){

	//TODO: READ WHICH FILE WOULD YOU LIKE
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
	printf("WE REACH READ GET FILE");
	// PRINT STRING TO CONSOLE
	printf("%s\n",result);

//			send_menu_choice(socket, '7',read_string);
	send_string(socket,"fixed.txt");
	
	
	//TODO: SEND THE FILE
	//TODO: READ THE RESPONSE

	// FREE UP THE RESULT
	free(result);
	result = NULL;

	}
	

}

// REUSABLE FUNCTION TO SEND A STRING BACK TO CLIENT
void send_string(int socket, char *response){

        size_t payload_length = strlen(response);

        //printf("PAYLOAD: %s //EOL%zu\n",response,payload_length);//DEBUG

        writen(socket, (unsigned char *) &payload_length, sizeof(size_t));

        //printf("DATA: %s //EOL%zu\n",response,payload_length);//dEBUG

        writen(socket, (unsigned char *)response, payload_length);

//      free(response);

}

// READ STRING FROM SERVE
char *read_string(int socket){

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
	//printf("%s\n",result);

	return result;

	// FREE UP THE RESULT
	//free(result);
	//result = NULL;

	} else {

		return NULL;	

	}

}

// READ UTSNAME STRUCT FROM SERVER
struct utsname *read_server_details(int socket){

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
	//	free(uts);
	//	uts = NULL;
	return uts;

	} else {

		return NULL;
	}

}

// SEND USERS MENU CHOICE ACROSS TO SERVER AND HANDLE THE RESPONSE
/*void send_menu_choice(int socket, char choice, read_cb readfunction){

	size_t payload_length = sizeof(char);

	//printf("payload_length is: %zu (%zu bytes)\n", payload_length, payload_length);

	    // SEND THE VALUE OF INT
	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));	
	writen(socket, (unsigned char *) &choice, payload_length);

	// CALLBACK DEPENDING ON DATA
	char *response = readfunction(socket);
	free(response);

}*/


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
	printf("[1]\tGet Student Information \n");
	printf("[2]\tGet server timestamp\n");
	printf("[3]\tGet server information\n");
	printf("[4]\tGet server file list\n");
	printf("[5]\tGet a file frin the server\n");
	printf("[6]\tExit\n");
	printf("-------------------\n");

}

// SMALL HELPER FUNCTION TO FORMAT DATA CONSISTENTLY
void display_heading(char *message){

printf("\n%s\n-------------------\n",message);

}

// GET FILE NAME
char *get_file_name(){


	// GET OPTION FROM USER
	printf("Please enter the name of the file\n");
	printf("that you would like to download\n\n");
	printf("Filename> ");

	int input_size = 255;
	char filename[input_size];	

	do {

		fgets(filename, input_size, stdin);	
		filename[strcspn(filename, "\n")] = 0;

		if(strlen(filename)<=0){

			printf("You must type a filename\n");

		}

	} while(strlen(filename) <= 0);

	//printf("The string is %zu chars long\n",strlen(filename));
	//input = name[0];

	char *result = (char *)malloc((sizeof(char)*strlen(filename)+1));
	memset(result,'\0',strlen(filename)+1);
	
	if(result!=NULL){
	strcpy(result,filename);
	return result;
	} else {
	return NULL;
	}
	
//	printf("You entered %s\n",filename);
}

// GET STUDENT INFO FROM SERVER
void get_student_info(int socket){

	// SEND THE MENU CHOICE
	send_string(socket,"1");

	// READ BACK THE DATA
	char *result = read_string(socket);

	// MAKE SURE IT ISN'T NULL
	if(result!=NULL){

	// DISPLAY AND CLEAR THE MEMORY
	printf("%s\n",result);
	free(result);
	result = NULL;

	}

}
// GET TIME FROM SERVER
void get_server_time(int socket){


	// SEND THE MENU CHOICE
	send_string(socket,"2");

	// READ BACK THE DATA
	char *result = read_string(socket);

	// MAKE SURE IT ISN'T NULL
	if(result!=NULL){

	// DISPLAY AND CLEAR THE MEMORY
	printf("%s\n",result);
	free(result);
	result = NULL;

	}

}
// GET SERVER INFORMATION
void get_server_info(int socket){

	// SEND THE MENU CHOICE
	send_string(socket,"3");

	// READ BACK THE DATA
	struct utsname *result = read_server_details(socket);

	// MAKE SURE IT ISN'T NULL
	if(result!=NULL){

	// DISPLAY AND CLEAR THE MEMORY
	printf("Node name:    %s\n", result->nodename);
	printf("System name:  %s\n", result->sysname);
	printf("Release:      %s\n", result->release);
	printf("Version:      %s\n", result->version);
	printf("Machine:      %s\n", result->machine);

	free(result);
	result = NULL;

	}

}
// GET FILE LIST FROM SERVER
void get_file_list(int socket){

	// SEND THE MENU CHOICE
	send_string(socket,"4");

	// READ BACK THE DATA
	char *result = read_string(socket);

	// MAKE SURE IT ISN'T NULL
	if(result!=NULL){

	// DISPLAY AND CLEAR THE MEMORY
	printf("%s\n",result);
	free(result);
	result = NULL;

	}
}

void get_file(int socket){


	// SEND THE MENU CHOICE
	send_string(socket,"5");

	// READ BACK THE DATA
	char *result = read_string(socket);

	// MAKE SURE IT ISN'T NULL
	if(result!=NULL){

	// DISPLAY AND CLEAR THE MEMORY
	printf("%s\n",result);
	free(result);
	result = NULL;

	// GET FILENAME FROM USER
	char *filename = get_file_name();
	
		// MAKE SURE IT ISN'T NULL
		if(filename!=NULL){

			// SEND THAT ACCROSS TO THE SERVER
			send_string(socket,filename);
			
			// HANDLE THE FILE FROM THE SERVER
			//TODO: ACTUALLY GET THE FILE
			char *server_response = read_string(socket);
			printf("You request:\n%s\n",server_response);
			free(server_response);		

		}

	free(filename);

	}

}

void close_connection(int socket){

	send_string(socket,"6");
	char *response = read_string(socket);
	printf("Exiting now...\n");
	free(response);

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
			get_student_info(sockfd);
			//send_menu_choice(sockfd, '1',read_string);
			break;
		case '2':
			display_heading("Server Timestamp");
			get_server_time(sockfd);
			//send_menu_choice(sockfd, '2',read_string);
			break;
		case '3':
			display_heading("Server Information");
			get_server_info(sockfd);
			//send_menu_choice(sockfd, '3',read_server_details);
			break;
		case '4':
			display_heading("Server File List");
			get_file_list(sockfd);
			//send_menu_choice(sockfd, '4',read_string);
			break;
		case '5':
			//send_menu_choice(sockfd,'5',read_get_file);
			//get_file_name();
			//printf("Retrieve file list\n");
			//display_heading("Server File List");
			//send_menu_choice(sockfd, '4',read_string);
			get_file(sockfd);
			break;
		case '6':
			close_connection(sockfd);
			break;
		default:
			printf("Invalid choice - 0 displays options...!\n");
			displaymenu();
		    break;
	}

	} while (input != '6');


	// GRACEFULLY CLOSE SOCKETS AND EXIT
	close(sockfd);
	exit(EXIT_SUCCESS);

} // end main()
