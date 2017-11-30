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


// thread function
void *client_handler(void *);

typedef struct {
    int id_number;
    int age;
    float salary;
} employee;

// PROTOTYPES
void get_and_send_employee(int, employee *);
void send_hello(int);
char *get_ip_address();
void send_string(int socket, char *response);

// you shouldn't need to change main() in the server except the port number
int main(void)
{
    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(50001);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1) {
	perror("Failed to listen");
	exit(EXIT_FAILURE);
    }
    // end socket setup

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while (1) {
	printf("Waiting for a client to connect...\n");
	connfd =
	    accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
	printf("Connection accepted...\n");

	pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
	if (pthread_create
	    (&sniffer_thread, NULL, client_handler,
	     (void *) &connfd) < 0) {
	    perror("could not create thread");
	    exit(EXIT_FAILURE);
	}
	//Now join the thread , so that we dont terminate before the thread
	//pthread_join( sniffer_thread , NULL);
	printf("Handler assigned\n");
    }

    // never reached...
    // ** should include a signal handler to clean up
    exit(EXIT_SUCCESS);
} // end main()

char *get_time(){

   time_t t;    // always look up the manual to see the error conditions
    //  here "man 2 time"
    if ((t = time(NULL)) == -1) {
        perror("time error");
        exit(EXIT_FAILURE);
    }

    // localtime() is in standard library so error conditions are
    //  here "man 3 localtime"
    struct tm *tm;
    if ((tm = localtime(&t)) == NULL) {
        perror("localtime error");
        exit(EXIT_FAILURE);
    }

    //printf("%s", asctime(tm));
	return asctime(tm);

}

// SEND THE STUDENT NAME AND NUMBER
void send_student_info(int socket){

	char *ipaddress = get_ip_address();
	char name[] = "Christopher Connor";
	char sid[] = "S1234567";
	
	char *response = malloc(sizeof(char)*100);
	snprintf(response,100,"%s,%s,%s",name,sid,(char *)ipaddress);

	//printf("%s\n",response);


	// FREE UP THE MEMORY IN IPADDRESS

	//send_string(&socket,response);

	//printf("SENDING ACROSS: %s //EOL\n",response);
	send_string(socket,response);
	//size_t payload_length = strlen(response)+1;
	//writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
	//writen(socket, (unsigned char *)response, payload_length);

	free(ipaddress);
	free(response);
}

void send_string(int socket, char *response){

	size_t payload_length = strlen(response);

	// TEMP DEBUG
	printf("PAYLOAD: %s //EOL%zu\n",response,payload_length);

	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
	
	// TEMP DEBUG
	printf("DATA: %s //EOL%zu\n",response,payload_length);

	writen(socket, (unsigned char *)response, payload_length);

}

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
    /* Display result */
	int ipsize = 16;
	char *ipaddress = (char *)malloc(sizeof(char)*ipsize);
    snprintf(ipaddress,ipsize,"%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    //printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	return ipaddress;

}

void get_server_details(){

    struct utsname uts;

    if (uname(&uts) == -1) {
	printf("This did not work");
	//perror("uname error");
	//exit(EXIT_FAILURE);
    }

    printf("Node name:    %s\n", uts.nodename);
    printf("System name:  %s\n", uts.sysname);
    printf("Release:      %s\n", uts.release);
    printf("Version:      %s\n", uts.version);
    printf("Machine:      %s\n", uts.machine);

}

void get_menu_choice(int socket, char *choice){

	size_t payload_length = sizeof(char);
	size_t n = readn(socket, (unsigned char *) &payload_length, sizeof(size_t));

	printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
	//unsigned char result[payload_length];

printf("Reaches here?");
	 n = readn(socket, (unsigned char *) choice, payload_length);

	
	printf("The receipt of data was:%c\n",*choice);

	// DONT WRITE BACK HERE
//    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
 //   writen(socket, (unsigned char *) choice, payload_length);

//free(result);

}

// thread function - one instance of each for each connected client
// this is where the do-while loop will go
void *client_handler(void *socket_desc)
{
    //Get the socket descriptor
    int connfd = *(int *) socket_desc;

    send_hello(connfd);


// TODO: GET MENU OPTION
char *menu_choice = (char *)malloc(sizeof(char));

do {
	// GET MENU CHOICE AND SET THE CHOICE POINTER
	get_menu_choice(connfd,menu_choice);

	switch(*menu_choice) {

	case '1':
	printf("GET MENU CHOICE\n");
	send_student_info(connfd);
	break;

	case '2':
	printf("SEND THE TIME\n");
	char *time = get_time();
	//send_student_info(connfd);
	send_string(connfd,time);
	break;
	case '3':
	get_server_details();
	send_student_info(connfd);
	break;
	default:
	printf("DEFAULT MENU\n");
	send_student_info(connfd);
	//break;
	//send_string(connfd,"NO OPTION THIS IS DEFAULT");
	}

} while(*menu_choice!='4');


//	send_student_info(connfd);
free(menu_choice);
//TODO: SEND STUDENT ID

/*    employee *employee1;
    employee1 = (employee *) malloc(sizeof(employee));

    int i;
    for (i = 0; i < 5; i++) {
	printf("(Counter: %d)\n", i);
	get_and_send_employee(connfd, employee1);
	printf("\n");
    }

    free(employee1);*/

    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

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

// as before...
void get_and_send_employee(int socket, employee * e)
{

	sleep(1);
    size_t payload_length;

    size_t n = readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
    printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
    n = readn(socket, (unsigned char *) e, payload_length);

    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);
    printf("(%zu bytes)\n", n);

    // make arbitrary changes to the struct & then send it back
    e->age++;
    e->salary += 1.0;

    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
    writen(socket, (unsigned char *) e, payload_length);
}  // end get_and_send_employee()
