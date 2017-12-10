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
#include <signal.h>
#define INPUTSIZ 10

// CREATING TO ALLOW CALLBACK - ENDED UP CHANGING
typedef void (*read_cb)(int socket);
void send_string(int socket,char* response);
char *read_string(int socket);
void send_menu_choice(int socket, char choice, read_cb readfunction);

void close_connection(int);
// DECLARING GLOBAL TO CLOSE CONNECTION GRACEFULLY
int sockfd;

// SIGNAL HANDLER
static void handler(int sig, siginfo_t *siginfo, void *context)
{

	// DEBUG
        printf("The signal no was %d\n",sig);
        printf("PID: %ld, UID: %ld\n",
        (long) siginfo->si_pid, (long) siginfo->si_uid);

	// MEANS CONNECTION HAS FAILED
	if(siginfo->si_signo==13){
	
		printf("Connection to the server has been lost...\n\nExiting now\n");
	
		// CHECK TO SEE IF SOCKET CLOSES
		if(close(sockfd)==-1){
			perror("The socket couldn't be closed");
		}
	
		exit(EXIT_FAILURE);

	}
	
	// HANDLE SIGINT - CLOSE SOCKET GRACEFULLY
	if(siginfo->si_signo==2){

		printf("You have force quit the connection to the server.\n");
			
		// LET THE SERVER KNOW AND CLOSE	
		close_connection(sockfd);


		//TRY AND GRACEFULLY CLOSE SOCKETS AND EXIT
		if(close(sockfd)==-1){
	
			perror("Didn't close sockfd successfully");
			exit(EXIT_FAILURE);

		}

		exit(EXIT_SUCCESS);

	}

}

// LAUNCH SCREEN FOR DEMO PURPOSES TO SHOW MY DETAILS
void launch_screen(){
	
	printf("  _______ _____ _____   _______ _      _____ ______ _   _ _______ \n");
	printf(" |__   __/ ____|  __ \\ / / ____| |    |_   _|  ____| \\ | |__   __|\n");
	printf("    | | | |    | |__) / / |    | |      | | | |__  |  \\| |  | |   \n");
	printf("    | | | |    |  ___/ /| |    | |      | | |  __| | . ` |  | |   \n");
	printf("    | | | |____| |  / / | |____| |____ _| |_| |____| |\\  |  | |   \n");
	printf("    |_|  \\_____|_| /_/   \\_____|______|_____|______|_| \\_|  |_|   \n");
        printf("--------------------------------------------------------------------------\n\n");
        printf("\tStudent: Chris Connor\n");
        printf("\t    SID: S1715477\n");
        printf("\t  Email: cconno208@caledonian.ac.uk / chris@chrisconnor.co.uk\n");
        printf("\t GitHub: https://www.github.com/cjconnor24/sp-c-coursework (PRIVATE REPO)\n\n");
        printf("--------------------------------------------------------------------------\n\n");

}

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

unsigned char *read_data(int socket){


	// CALCULATE AND ALLOCATE MEMORY FOR THE RESULT
	size_t memallocation = BUFSIZ;
	unsigned char *result = (unsigned char *)malloc(memallocation);

	if(result!=NULL){
	
	// INITIALISE EVERY PART OF MEM - WITHOUT WAS CAUSING VALG ISSUES
	memset(result,'\0',memallocation);
	
	readn(socket, (unsigned char *) result, BUFSIZ);


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


	struct utsname *uts = malloc(sizeof(struct utsname));

	// MAKE SURE THE STRUCT ISN'T NULL BEFORE DOING SOMETHING WITH IT	
	if(uts!=NULL){

		readn(socket, (unsigned char *) uts, payload_length);

	return uts;

	} else {

		return NULL;
	}

}

// DISPLAY MENU OPTIONS TO USER
void displaymenu()
{
    
	printf("+-----------------------------------+\n");
	printf("|      MENU                         |\n");
	printf("+-----------------------------------+\n");
	printf("|  [0]\tRe-display menu             |\n");
	printf("+-----------------------------------+\n");
	printf("|  [1]\tGet Student Information     |\n");
	printf("|  [2]\tGet server timestamp        |\n");
	printf("|  [3]\tGet server information      |\n");
	printf("|  [4]\tGet server file list        |\n");
	printf("|  [5]\tGet a file from the server  |\n");
	printf("+-----------------------------------+\n");
	printf("|  [6]\tExit                        |\n");
	printf("+-----------------------------------+\n\n");

}

// SMALL HELPER FUNCTION TO FORMAT DATA CONSISTENTLY
void display_heading(char *message){

printf("\n%s\n-------------------\n",message);

}

// FUNCTION TO BUILD FULL PATHNAME OF REQUESTED FILE
char *get_full_path(char *filename){

	char dirname[] = "./download/";	

        // CREATE THE MEMORY
        size_t strsize =(sizeof(char)*(strlen(filename)+strlen(dirname)+1));
        char *full_path = (char *)malloc(strsize);

        // INITIALISE
        memset(full_path,'\0',strsize);

        // WRITE THE PATH
        snprintf(full_path,strsize,"%s%s",dirname,filename);

        return full_path;

}

// GET FILE NAME FROM USER
char *get_file_name(){

	// DISPLAY INSTRUCTION TO USER
	//printf("Please enter the name of the file\n");
	//printf("that you would like to download\n\n");
	printf("Filename> ");

	// SET ASIDE SOME SPACE FOR THE RESPONSE
	int input_size = 255;
	char filename[input_size];	

	// LOOP UNTIL INPUT IS NOT BLANK
	do {
		
		// GET THE INPUT FROM THE USER
		fgets(filename, input_size, stdin);	
		filename[strcspn(filename, "\n")] = 0;
		
		// MAKE SURE ISN"T BLANK, IF SO REDISPLAY INSTRUCTION
		if(strlen(filename)<=0){

			printf("You must type a filename\n");
			printf("Filename> ");

		}

	} while(strlen(filename) <= 0);


	// CREATE A POINT TO ALLOCATE THE RIGHT SIZE
	char *result = (char *)malloc((sizeof(char)*strlen(filename)+1));
	memset(result,'\0',strlen(filename)+1);
	
	// MAKE SURE ISNT NULL POINTER
	if(result!=NULL){

		strcpy(result,filename);
		return result;

	} else {

		return NULL;

	}
	
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
	//printf("Node name:    %s\n", result->nodename);
	printf("System name:  %s\n", result->sysname);
	printf("Release:      %s\n", result->release);
	printf("Version:      %s\n", result->version);
	//printf("Machine:      %s\n", result->machine);

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

/*void write_file(char *filename, unsigned char *data){

// OPEN THE NEW FILE
FILE *fnew = fopen(filename,"wb");

// GET THE SIZE
int sz = strlen(data);


// WRITE THE DATA TO THE BUFFER
fwrite(data,1,sz,fnew);

//CLOSE THE NEW FILE
fclose(fnew);


// FREE THE HEAP MEME
//  free(buffer);


}*/

// TODO: REFACTOR CODE TO REDUCE NESTED IFS
// DOWNLOAD FILE FROM SERVER
void get_file(int socket){


	// SEND THE MENU CHOICE
	send_string(socket,"5");

	// READ BACK THE DATA - i.e. Which file would you like?
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
	
				// SEND FILE NAME TO SERVER
				send_string(socket,filename);
				
				// CHECK IF EXISTS AND GET THE SIZE - (-1) MEANS DOESNT EXIST
				char *filesize = read_string(socket);
				int sizeint = atoi(filesize);
	
				// CHECK FILE EXISTS - IF SO GO AHEAD
				if(sizeint!=-1){
		
					//printf("The filesize is %d\n",sizeint); // DEBUG
				
					// GET FULL PATH TO DOWNLOAD DIRECTORY	
					char *fullpath = get_full_path(filename);	
		
					// TODO: CHECK IF FILE EXISTS LOCALLY
					FILE *temp = fopen(fullpath,"r");
					
					// FILE EXISTS, LET USER KNOW
					if(temp){
	
						// CLOSE THE FILE	
						fclose(temp);
	
						printf("%s already exists. Please enter a new filename:\n",filename);
						char *newfilename;
						int check = -1;
		
							// LOOP UNTIL DOESNT = OLD NAME
							do {
			
								newfilename = get_file_name();
							
								// IF THEY ENTERED THE SAME NAME	
								if(*filename==*newfilename){
	
									printf("You entered the same name\n");
								
									// RELEASE ANY USEAGE	
									if(newfilename!=NULL){
	
										free(newfilename);
	
									}
	
								} else {
									// NAME DOESNT MATCH - BREAK LOOP
									check = 0;
	
								}
			
							}while(check);
						
						// MAKE THE NEW FILE PATH
						free(fullpath);
						fullpath = get_full_path(newfilename);
	
						free(filename);
						filename = newfilename;
						
						//DONT NEED TO FREE AS SAME MEMORY LOCATION NOW
						//free(newfilename);
	
					}
						
					// OPEN THE NEW FILE TO WRITE ON CLIENT SIDE
					FILE *newfile = fopen(fullpath,"wb");
	
					
					// BLOCK SIZE TO RECEIVE
					int sendbuffer = 30;			
					
					// LOOP UNTIL WHOLE FILE HAS BEEN RECEIVED
					while(sizeint > 0){
						
						// CREATE A WRITE BUFFER SO AS NOT TO WRITE TOO MUCH DATA
						int writebuffer = (sizeint < sendbuffer ? sizeint : sendbuffer);
					
						// SET ASIDE SOME SPACE TO STORE THE CURRENT BLOCK	
						unsigned char *temp = (unsigned char *)malloc(writebuffer);
						readn(socket, (unsigned char *)temp, writebuffer);
						
						// WRITE THE CURRENT BLOCK TO THE FILE
						fwrite(temp,1,writebuffer,newfile);
						
						// FREE UP THE TEMP AREA	
						free(temp);
						sizeint = sizeint - sendbuffer;
					}
	
					// FREE UP ALLOCATED RESOURCES	
					fclose(newfile);
					free(filesize);
					free(fullpath);
	
					printf("%s was successfully downloaded.\n\n",filename);
	
				} else {
	
					// LET USER KNOW, THE FILE DOESNT EXIST
					printf("Sorry, %s does not exist on the server\n",filename);
					if(filesize!=NULL){
						free(filesize);
					}	
	
				}


			}
		free(result);
		free(filename);

	}
	
	
	if(result!=NULL){
	free(result);
	result=NULL;
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

	// LAUNCH SCREEN FOR DEMO
	launch_screen();

        //TODO: SIGNAL HANDLER
        struct sigaction act;
        memset(&act, '\0', sizeof(act));

        // this is a pointer to a function
        act.sa_sigaction = &handler;
        // the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler
        act.sa_flags = SA_SIGINFO;

        // DEBUG
	//printf("Sig Handler Assigned\n");

        // HANDLE SIGPIPE
        if (sigaction(SIGPIPE, &act, NULL) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
        }

	// HAND SIGINT
        if (sigaction(SIGINT, &act, NULL) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
        }

    // *** this code down to the next "// ***" does not need to be changed except the port number
	sockfd = 0;
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
       printf("Successfully connected to server...\n\n");


	char input;
	char name[10];

	displaymenu();

	do {

		// GET OPTION FROM USER
		printf("Enter your choice> ");
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
			// INVALID OPTION
			printf("\nSorry, that was an invalid choice. Press [0] to display the menu.\n\n");
		    break;
	}

	} while (input != '6');


	//TRY AND GRACEFULLY CLOSE SOCKETS AND EXIT
	if(close(sockfd)==-1){

		perror("Didn't close sockfd successfully");
		exit(EXIT_FAILURE);

	}

	exit(EXIT_SUCCESS);

} // end main()
