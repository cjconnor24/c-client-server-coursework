// Cwk2: server.c - multi-threaded server using readn() and writen()

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "rdwrn.h"
#include <sys/utsname.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>

// thread function
void *client_handler(void *);

void send_hello(int);
char *get_ip_address();
void send_string(int socket, char *response);
void stat_file(char *file);
char *get_file_list();
char *build_list(char *old, char *new);

// SIGNAL HANDLER
static void handler(int sig, siginfo_t *siginfo, void *context)
{
	printf("The signal no was %d\n",sig);
	printf("PID: %ld, UID: %ld\n",
	(long) siginfo->si_pid, (long) siginfo->si_uid);
}

// SET TIMER TO CURRENT TIME VALUE
void set_timer(struct timeval *timer){

	if (gettimeofday(timer, NULL) == -1) {
		perror("gettimeofday error");
		exit(EXIT_FAILURE);
	}

}

// you shouldn't need to change ain() in the server except the port number
int main(void)
{

	// CREATE TIMERS FOR CALCULATING EXECUTION TIME
	struct timeval start_time, end_time;
	set_timer(&start_time);

    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(50031);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1) {
	perror("Failed to listen");
	exit(EXIT_FAILURE);
    }
    // end socket setup

	//TODO: TIME TO SEE HOW LONG SERVER HAS BEEN RUNNING

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while (1) {

	printf("Waiting for a client to connect...\n");
	connfd = accept(listenfd, (struct sockaddr *) &client_addr, &socksize);


	printf("Connection accepted...\n");

	pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
	if (pthread_create(&sniffer_thread, NULL, client_handler,(void *) &connfd) < 0) {
	    perror("could not create thread");
	    exit(EXIT_FAILURE);
	}

	//Now join the thread , so that we dont terminate before the thread
	//TODO: SIGNAL HANDLER
/*	struct sigaction act;
	memset(&act, '\0', sizeof(act));

	// this is a pointer to a function
	act.sa_sigaction = &handler;
	// the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler
	act.sa_flags = SA_SIGINFO;

	// HANDLE SIGINT
	if (sigaction(SIGINT, &act, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}*/
	printf("--------------------\n");
	printf("Handler assigned\n");
	printf("--------------------\n");


	//pthread_join( sniffer_thread , NULL);
	
	// THIS NEED TO BE INSIDE A HANDLER FOR SIGNAL
	set_timer(&end_time);
	printf("Total execution time = %f seconds\n",(double)(end_time.tv_usec - start_time.tv_usec) / 1000000 + (double)(end_time.tv_sec - start_time.tv_sec));


    }

    // never reached...
    // ** should include a signal handler to clean up
    exit(EXIT_SUCCESS);
} // end main()


// GET SYSTEM TIME
char *get_time(){

	time_t t;

	if ((t = time(NULL)) == -1) {
	perror("time error");
	//exit(EXIT_FAILURE);
	}

	struct tm *tm;
	if ((tm = localtime(&t)) == NULL) {
		perror("localtime error");
	//        exit(EXIT_FAILURE);
	}

	// RETURN THE TIME
	return asctime(tm);

}

// SEND THE STUDENT NAME AND NUMBER
void send_student_info(int socket){

	// BUILD UP THE STRING TO SEND
	char *ipaddress = get_ip_address();
	char name[] = "Christopher Connor";
	char sid[] = "S1234567";
	
	// ALLOCATE THE MEMORY AND COPY IN THE FORMATTED STRING
	char *response = malloc(sizeof(char)*100);
	snprintf(response,100,"%s,%s,%s",name,sid,(char *)ipaddress);

	// SEND THE DATA
	send_string(socket,response);

	// FREE UP THE ALLOCATED MEMORY
	free(ipaddress);
	free(response);

	// EXTRA SECURITY
	ipaddress=NULL;
	response=NULL;
}

// REUSABLE FUNCTION TO SEND A STRING BACK TO CLIENT
void send_string(int socket, char *response){

	size_t payload_length = strlen(response);

	//printf("PAYLOAD: %s //EOL%zu\n",response,payload_length);//DEBUG

	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
	
	//printf("DATA: %s //EOL%zu\n",response,payload_length);//dEBUG

	writen(socket, (unsigned char *)response, payload_length);

//	free(response);

}

// GET THE CLIENT IP ADDRESS
char *get_ip_address(){

	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want an IP address attached to "eth0" */
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	// CREATE POINTER AND SEND BACK TO CALLING POINT	
	int ipsize = 16;
	char *ipaddress = (char *)malloc(sizeof(char)*ipsize);
	snprintf(ipaddress,ipsize,"%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	return ipaddress;

}

// GET THE SERVER DETAILS AS A UTSNAME STRUCT
struct utsname *get_server_details(){

	// CREATE SPACE FOR THE STRUCT
	struct utsname *uts = malloc(sizeof(struct utsname));

	// MAKE SURE CAN RETRIEVE THE DETAILS
	if(uname(uts) == -1) {

		printf("This did not work");
		//perror("uname error");
		//exit(EXIT_FAILURE);
		return NULL;
	}

	// RETURN THE STRUCT POINTER TO CALLING POINT
	return uts;

}

// SEND THE SERVER FILE LIST BACK TO CLIENT
void send_file_list(int socket){

	// GET THE CURRENT FILE LIST
	char *filelist = get_file_list();

	if(filelist!=NULL){

	// SEND STRING ACROSS TO CLIENT
	send_string(socket,filelist);

	} else {

	send_string(socket,"No files");

	}

	// FREE UP THE FILELIST MEMORY
	free(filelist);
	
	// EXTRA SECURITY
	filelist=NULL;

}

// GET SERVER FILE LIST
char *get_file_list(){

	//CREATE STRUCT
	struct dirent **namelist;
	int n;
	
	//TODO: FIND WHERE THE LEAK IS HERE :-|
	if ((n = scandir("./upload/", &namelist, NULL, alphasort)) != -1){
  //      	perror("scandir");
//		return "";
//	} else {

	//printf("There are %d files in the dir\n",n);

	// TRY SETTING SPACE FOR 2 CHARS
	//char *filelist = (char *)malloc(sizeof(char)*2);

	// CREATE A BUFFER TO READ THE FILES INTO
	size_t memallocation = sizeof(char)*2048;
	char *list = (char *)malloc(memallocation);
	memset(list,'\0',memallocation);

	// LOOP THROUGH THE LIST
        while (n--) {

		// TRY AND COPY INTO STRING - THE BELOW WORKS
		strcat(list,namelist[n]->d_name);
		strcat(list,"\n");
		free(namelist[n]);

        }
	
	// MOVE PRECISE STRING INTO NEW MEMORY - THEN RELEASE THE BUFFER AREA
	size_t list_length = strlen(list);
	char *sized_list = (char *)malloc(list_length+1);
	strcpy(sized_list,list);
	free(list);
	list = NULL;

	// DEBUG LEAKING MEM
	//free(n);
	n = 0;
	//printf("The list:%s\n",filelist);
	//free(filelist);
	//return filelist;
	printf("Sized List:\n%s\n",sized_list);
	return sized_list;
        free(namelist);         //NB
	namelist = NULL;

} else {

	
	return NULL;

}



}
// SEND THE UTSNAME STRUCT ACCROSS TO CLIENT
void send_server_details(int socket){

	// GET THE STRUCT
	struct utsname *uts = get_server_details();
	size_t payload_length = sizeof(struct utsname);

	//printf("PAYLOAD: %s //EOL%zu\n",uts,payload_length);//DEBUG

	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
	
	//printf("DATA: %s //EOL%zu\n",uts,payload_length);//DEBUG

	writen(socket, (unsigned char *)uts, payload_length);
	// FREE UTS ONCE SENT
	free(uts);
	
	// EXTRA SECURITY
	uts=NULL;
}

// RETRIEVE MENU CHOICE FROM CLIENT
void get_menu_choice(int socket, char *choice){

	
	size_t payload_length = sizeof(char);
	
	// READ PAYLOAD AND DATA FROM CLIENT
	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
	readn(socket, (unsigned char *) choice, payload_length);

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

// THREAD FUNCTION TO ENCOMPASS ALL FUNCTIONALITY
void *client_handler(void *socket_desc)
{
	//Get the socket descriptor
	int connfd = *(int *) socket_desc;

	// SEND WELCOME MESSAGE
	send_hello(connfd);

	// CREATE A SPACE FOR THE MENU CHOICE
	char *menu_choice = (char *)malloc(sizeof(char));

	// LOOP UNTIL EXIT CHOICE IS RECEIVED
	do {
	
		// GET MENU CHOICE AND SET THE CHOICE POINTER
		get_menu_choice(connfd,menu_choice);

		switch(*menu_choice) {

		case '1':
			printf("Sending Student Info\n");
			send_student_info(connfd);
		break;
		case '2':
		{
			//TODO: MAKE SURE TIME IS FREED
			printf("Sending the time...\n");
			char *time = get_time();
			send_string(connfd,time);
			//free(time);
		break;
		}
		case '3':
			printf("Sending server details\n");
			send_server_details(connfd);
		break;
		case '4':
		{
			printf("Sending file list...\n");
			send_file_list(connfd);
		break;
		}
		case '5':
			
			printf("Client want's file\n");
			send_string(connfd,"Which file would you like?");
			char *filename = read_string(connfd);
			printf("The client requested: %s\n",filename);
			send_string(connfd,filename);
			free(filename);
		break;
		case '7':
			printf("The client has sent the string\n");
			read_string(connfd);
			send_string(connfd,"Which file would you like?");
		break;
		default:
			send_string(connfd,"Sorry, I didn't recognise that option");
		break;
	}

	} while(*menu_choice!='6');

	// FREE UP THE MEMORY CHOICE CHAR
	free(menu_choice);


	shutdown(connfd, SHUT_RDWR);
	close(connfd);

	printf("Thread %lu has exited.\n", (unsigned long) pthread_self());

	// always clean up sockets gracefully
	shutdown(connfd, SHUT_RDWR);
	close(connfd);

	pthread_exit(NULL);

    return 0;
}  // end client_handler()

// how to send a string
void send_hello(int socket)
{
    char hello_string[] = "hello SP student";

    size_t n = strlen(hello_string) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));	
    writen(socket, (unsigned char *) hello_string, n);	  
} // end send_hello()


