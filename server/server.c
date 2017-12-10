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
void set_timer(struct timeval *timer);
char *read_string(int);
char *get_time();
void output_log(char *);
//void uptime(struct timeval *start_time, struct timeval *end_time);


// GLOBAL VARS FOR SIGNAL HANDLER
// I KNOW ITS BAD PRACTISE BUT ONLY WAY
// I COULD ACCESS POINTERS IN THE SIG HANDLER
struct timeval start_time, end_time; // START AND END TIME FOR EXECUTION TIME
int connfd;			// CONNECTION TO CLOSE IN SIGHANDLER
int listenfd;			// LISTENER TO CLOSE IN SIGHANDLER
//pthread_t sniffer_thread;	// SNIFFER THREAD WHICH WILL WAIT FOR PTHREAD_JOIN IN SIGHANDLER

// CHANGE THIS FOR UPLOAD DIRECTORY TO SEARCH FILES
char dirname[] = "./upload/";	// DIRECTORY NAME

// SIGNAL HANDLER
static void handler(int sig, siginfo_t *siginfo, void *context)
{

	// DEBUGGING INFO
	//printf("The signal no was %d\n",sig);
	//printf("PID: %ld, UID: %ld\n",
	//(long) siginfo->si_pid, (long) siginfo->si_uid);

	// WAIT ON ALL THREADS ENDING BEFORE CLOSE
	//pthread_join(sniffer_thread,NULL);
	set_timer(&end_time);

	//printf("Server will shut down...Error %d\n",siginfo->si_signo);
	// OUTPUT UPTIME TO CONSOLE
	printf("----------------------------------\n");
	printf("TOTAL UPTIME: %f SECONDS\n",(double)(end_time.tv_usec - start_time.tv_usec) / 1000000 + (double)(end_time.tv_sec - start_time.tv_sec));
	printf("----------------------------------\n");

	// HANDLE SIGPIPE	
	if(siginfo->si_signo==13){
		
		output_log("SIGPIPE (13) Signal Received");
	
		if(close(connfd)==-1){
		// IF THIS TRIGGERS -1 - THERE WAS NO ACTIVE CONNECTION FROM CLIENT
		//	perror("No active connections to close. ");
		}
	
		// TRY AND CLOSE THE LISTENER GRACEFULLY 
		if(close(listenfd)==-1){
		// IF THIS TRIGGERS -1 THE LISTENER WASN'T ACTIVE
		//	perror("No listener was active");
		}
		
	//	output_log("[!WARNING!] Server is going to shutdown in 3 seconds...");
	//	sleep(3);

		exit(EXIT_SUCCESS);
	

	}

	// HANDLE SIGINT - 2
	if(siginfo->si_signo==2){
	
		// TRY AND CLOSE ANY CLIENT CONNECTIONS GRACEFULLY
		if(close(connfd)==-1){
			// IF THIS TRIGGERS -1 - THERE WAS NO ACTIVE CONNECTION FROM CLIENT
			//perror("No active connections to close. ");
		}
	
		// TRY AND CLOSE THE LISTENER GRACEFULLY 
		if(close(listenfd)==-1){
			// IF THIS TRIGGERS -1 THE LISTENER WASN'T ACTIVE
			//perror("No listener was active");
		}

		// OUTPUT MESSAGE THEN SHUTDOWN
		output_log("[!WARNING!] Server is going to shutdown.");
		exit(EXIT_SUCCESS);

	}

}

// SET TIMER TO CURRENT TIME VALUE
void set_timer(struct timeval *timer){

	if (gettimeofday(timer, NULL) == -1) {
		perror("gettimeofday error");
		exit(EXIT_FAILURE);
	}

}

// SIMPLE LAUNCH SCREEN FOR DEMO PURPOSES AND TO SHOW MY DETAILS
void launch_screen(){
	
	printf("  _______ _____ _____   _______ ______ _______      ________ _____  \n");
	printf(" |__   __/ ____|  __ \\ / / ____|  ____|  __ \\ \\    / /  ____|  __ \\ \n");
	printf("    | | | |    | |__) / / (___ | |__  | |__) \\ \\  / /| |__  | |__) |\n");
	printf("    | | | |    |  ___/ / \\___ \\|  __| |  _  / \\ \\/ / |  __| |  _  / \n");
	printf("    | | | |____| |  / /  ____) | |____| | \\ \\  \\  /  | |____| | \\ \\ \n");
	printf("    |_|  \\_____|_| /_/  |_____/|______|_|  \\_\\  \\/   |______|_|  \\_\\\n");
	printf("--------------------------------------------------------------------------\n\n");
	printf("\tStudent: Chris Connor\n");
	printf("\t    SID: S1715477\n");
	printf("\t  Email: cconno208@caledonian.ac.uk / chris@chrisconnor.co.uk\n");
	printf("\t GitHub: https://www.github.com/cjconnor24/sp-c-coursework (PRIVATE REPO)\n\n");
	printf("--------------------------------------------------------------------------\n\n");

}

void output_log(char *message){
	
	char *current_time = get_time();

	printf("[%s]:\t%s\n",current_time,message);

	if(current_time!=NULL){
	//	free(current_time);
	}

	if(message!=NULL){
	//	free(message);
	}

}


int main(void)
{

	// CREATE TIMERS FOR CALCULATING EXECUTION TIME
	//struct timeval start_time, end_time;
	set_timer(&start_time);
	
	// DISPLAY LAUNCH SCREEN
	launch_screen();

	// SIGNAL HANDLER -----------------------
	//TODO: SIGNAL HANDLER
	struct sigaction act;
	memset(&act, '\0', sizeof(act));

	// this is a pointer to a function
	act.sa_sigaction = &handler;
	// the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler
	act.sa_flags = SA_SIGINFO;

	// DEBUG
	//output_log("Sig Handler Assigned");

        // HANDLE SIGPIPE
	/*if (sigaction(SIGPIPE, &act, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}*/

        // HAND SIGINT
        if (sigaction(SIGINT, &act, NULL) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
        }
	
        if (sigaction(SIGPIPE, &act, NULL) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
        }

	// END
	// SIGNAL HANDLER -----------------------

	listenfd = 0;
	
	// ALREADY DEFINED AS A GLOBAL
	connfd = 0;

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



	// SETUP THREAD MASK TO STOP ENTIRE PROCESS DYING ON SIGPIPE IN THREAD
/*	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	//sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	sigset_t set;
	sigaddset(&set,SIGPIPE);
	pthread_sigmask(SIG_BLOCK,&set,NULL);*/

	 //Accept and incoming connection
	//puts("Waiting for incoming connections...");
	output_log("-- WAITING FOR INCOMING CONNECTIONS --");

while (1) {


	// WAIT ON A CLIENT CONNECTION
	connfd = accept(listenfd, (struct sockaddr *) &client_addr, &socksize);

	// OUTPUT CLIENT DETAILS TO LOG
	char tmpmsg[100];
	snprintf(tmpmsg,100,"Client connection from %s",inet_ntoa(client_addr.sin_addr));
	output_log(tmpmsg);

	pthread_t sniffer_thread;
	//client_handler(&connfd);
        // third parameter is a pointer to the thread function, fourth is its actual parameter
	if (pthread_create(&sniffer_thread, NULL, client_handler,(void *) &connfd) < 0) {
	    perror("could not create thread");
	    exit(EXIT_FAILURE);
	}

	//Now join the thread , so that we dont terminate before the thread
	//printf("--------------------\n");
	//printf("Handler assigned\n");
	//printf("--------------------\n");
	
	// THIS NEED TO BE INSIDE A HANDLER FOR SIGNAL
	//set_timer(&end_time);

	pthread_join( sniffer_thread , NULL);

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

	// STRIP THE \N FROM TIME
	char *split = strtok(asctime(tm),"\n");
	// RETURN THE TIME
	//return asctime(tm);
	return split;

}

// SEND THE STUDENT NAME AND NUMBER
void send_student_info(int socket){


	// BUILD UP THE STRING TO SEND
	char *ipaddress = get_ip_address();
	char name[] = "Christopher Connor";
	char sid[] = "S1234567";
	
	// ALLOCATE THE MEMORY AND COPY IN THE FORMATTED STRING
	char *response = malloc(sizeof(char)*100);
	snprintf(response,100,"%s\n%s\n%s",name,sid,(char *)ipaddress);

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

// FUNCTION TO BUILD FULL PATHNAME OF REQUESTED FILE
char *get_full_path(char *filename){

	// CREATE THE MEMORY
	size_t strsize =(sizeof(char)*(strlen(filename)+strlen(dirname)+1));
	char *full_path = (char *)malloc(strsize);

	// INITIALISE
	memset(full_path,'\0',strsize);

	// WRITE THE PATH
	snprintf(full_path,strsize,"%s%s",dirname,filename);

	return full_path;

}

// CHECK IF FILE EXISTS - RETURNS SIZE IF EXISTS
int file_exists(char *filename){

	FILE * file;

	char *full_path = get_full_path(filename);

	file = fopen(full_path,"r");
	int result;
	
	if (file){

		// GET THE FILE SIZE AND RETURN
		fseek(file, 0L, SEEK_END);
		result = ftell(file);
		fclose(file);

	} else {

		result = -1;

	}	

	// MAKE SURE NOT NULL BEFORE FREEING MEMORY
	if(full_path!=NULL){

		free(full_path);
		full_path = 0;

	}	


	return result;

}

// GET A FILE NAME FROM THE CLIENT AND SEND THE FILE ACCROSS FOR DOWNLOAD
void send_file(int socket){

	// SIMPLE HANDSHAKE TO START PROCESS
	send_string(connfd,"Which file would you like?");

	// GET FILENAME FROM CLIENT
	char *filename = read_string(socket);

	//printf("The client requested: %s\n",filename);

	// CHECK IF THE FILE EXISTS
	int filesize = file_exists(filename);
	//printf("The filesize is %d\n",filesize);

	// SET ASIDE SPACE FOR FILESIZE AND SEND AS A STRING
	char sizestr[20];
	snprintf(sizestr,20,"%d",filesize);
	send_string(connfd,sizestr);

	// IF FILE EXISTS - GO AHEAD
	if(filesize!=-1){
			
		// GET THE FULL PATH AND OPEN TO READ
		char *path = get_full_path(filename);			
		FILE *file = fopen(path,"rb");

		// SETUP SOME BUFFERS AND LOCAL VARS FOR SENDING	
		int sendbuffer = 30;
		char buffer[sendbuffer];
		int read = 0;
				
		// LOG TO CONSOLE FILE SEND WILL BEING
		char tmpmsg[100];
		snprintf(tmpmsg,100,"%s will be sent to the client",filename);
		output_log(tmpmsg);
	//	printf("%s will now be sent to the client.\n",filename);
	
		// LOOP AND SEND IN BLOCKS
		while((read = fread(buffer, 1, sendbuffer, file)) > 0){
	
			// CREATE A WRITE BUFFER SO AS NOT TO WRITE TOO MUCH DATA
			int writebuffer = (filesize < sendbuffer ? filesize : sendbuffer);
			
			// SEND ACROSS THE DATA
			writen(socket,(unsigned char *)buffer,writebuffer);
			
			// DECREASE REMAINING DATA LEFT TO SEND
			filesize = filesize - writebuffer;
		}
		
		// FREE UP ALLOCATED RESOURCES
		fclose(file);
		free(path);

		//OUTPUT TO LOG
		snprintf(tmpmsg,100,"%s has been sent",filename);
		output_log(tmpmsg);

	} else {

		//OUTPUT TO LOG
		char tmpmsg[100];
		snprintf(tmpmsg,100,"%s does not exist",filename);
		output_log(tmpmsg);

	}

	// FREE UP THE FILENAME
	if(filename!=NULL){
		free(filename);
	}

}

// SEND THE SERVER FILE LIST BACK TO CLIENT
void send_file_list(int socket){

	// GET THE CURRENT FILE LIST
	char *filelist = get_file_list();


	if(filelist!=NULL){

	// SEND STRING ACROSS TO CLIENT
	send_string(socket,filelist);
	
	// OUTPUT TO LOG
	output_log("Sending file list");

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

	//OUTPUT TO LOG
	output_log("Generating file list");
	
	if ((n = scandir(dirname, &namelist, NULL, alphasort)) != -1){

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
	//printf("Sized List:\n%s\n",sized_list);
        free(namelist);         //NB
	namelist = NULL;

	return sized_list;
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

		//printf("CONNFD IS: %d\n",connfd);
	
		// GET MENU CHOICE AND SET THE CHOICE POINTER
		get_menu_choice(connfd,menu_choice);

		switch(*menu_choice) {

		case '1':
			//printf("Sending Student Info\n");
			//OUTPUT TO LOG
			output_log("Sending student info");
			send_student_info(connfd);
		break;
		case '2':
		{
			//TODO: MAKE SURE TIME IS FREED
			output_log("Sending the time");
			char *time = get_time();
			send_string(connfd,time);
			//free(time);
		break;
		}
		case '3':
			output_log("Sending the server details");
			send_server_details(connfd);
		break;
		case '4':
		{
			output_log("Sending file list");
			send_file_list(connfd);
		break;
		}
		case '5':
			output_log("Client is requesting a file");
			send_file(connfd);
		break;
		case '6':
			output_log("Client is going to shutdown");
		break;
		default:
			output_log("Recieved unknown menu option from client");
			send_string(connfd,"Sorry, I didn't recognise that option");
		break;
	}

	} while(*menu_choice!='6');

	// FREE UP THE MEMORY CHOICE CHAR
	free(menu_choice);


	shutdown(connfd, SHUT_RDWR);
	close(connfd);

	//OUTPUT TO LOG THE THREAD EXIT
	char temp[100];
	snprintf(temp,100,"Thread %lu has exited.", (unsigned long) pthread_self());
	output_log(temp);

	// always clean up sockets gracefully
	shutdown(connfd, SHUT_RDWR);
	close(connfd);

	//pthread_exit(NULL);

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


