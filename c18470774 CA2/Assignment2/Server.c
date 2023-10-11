// 
// C socket sever exa,ple, handles multiple clients using threads
// compile
// gcc server.c -lpthread -o server
//

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LENGTH 512

#define NUM_THREADS 3

pthread_mutex_t lock_x;

//The thread function
void *connection_handler(void *);

int main(int argc, char *argv[]){
	int socket_desc, client_sock, c;
	struct sockaddr_in server, client;
	//create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc == -1) {
		printf("could not create socket");
	}
	puts("socket Create");
	//Prepare the sockaddr in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);
	//Bind
	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) <0) {
		//Print error message
		perror("blind failed. Error");
		return 1;
	}
	puts("bind done");
	//Listen
	listen(socket_desc, 3);
	// Accept and incoming connection
	puts("waiting for incoming connection....");
	c = sizeof(struct sockaddr_in);
	// Accept and incoming connection
	
	pthread_t thread_id;
	while((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
		puts("connection accepted");
		if(pthread_create( &thread_id, NULL, connection_handler, (void*) &client_sock) <0 ){
			perror("could not create thread");
			return 1;
		}
		//Now join the thread, so that we dont terminate before the thread 
		//pthread_join(thread_id, NULL);
		puts("Handler assigned");
	}
	if(client_sock < 0) {
		perror("accept failed");
		return 1;
	}
	return 0;
}

//
// This will handle connection for each client
//

//////////Connection Handler//////////////////////
void *connection_handler(void *socket_desc) {
	// get the socket descripter
	int sock = *(int*)socket_desc;
	int read_size;
	char msg[500];
	int READSIZE;
	char *message, client_message[2000];

	char userName[500];

	//Username//////////////////////////////////////////////////////////////
	memset(msg, 0, 500);
	READSIZE = recv(sock, msg, 500, 0);
	
	if(strlen(msg) >= 1) {
		//printf("File\n");
		write(sock, "Username Recieved", strlen("Username Recieved"));
		//printf("Username: %s\n", msg);
		strcpy(userName, msg);
		printf("Username: %s\n", userName);
		//memset(msg, 0, 500);

	}
	////////////////////////////////////////////////////////////////////////

	//User ID///////////////////////////////////////////////////////////////
	memset(msg, 0, 500);
	READSIZE = recv(sock, msg, 500, 0);

	char userID[500];


	if(strlen(msg) >= 1) {

		write(sock, "User ID Recieved", strlen("User ID Recieved"));
		//printf("Username: %s\n", msg);
		strcpy(userID, msg);
		printf("User ID: %s\n", userID);
	}

	//////////////////////////////////////////////////////////////////////////
	
	
	//Groups user id is linked with///////////////////////////////////////////

    int myUID = atoi(userID);
    gid_t supp_groups[] = {};

    int j, ngroups;
    gid_t *groups;
	char *fpath;


    ngroups = 10;
    groups = malloc(ngroups * sizeof (gid_t)); // max limit of 10 groups (random!!)

	if(getgrouplist(userName, myUID, groups, &ngroups) == -1){
        printf("Error: Retrieve Group List Failed\n");
    }

    // get all the groups associated with a given user
    for(j=0; j < ngroups; j++) {
        supp_groups[j] = groups[j];
        printf(" - %d\n", supp_groups[j]);

		if(supp_groups[j] == 1004){
			//Path
			fpath = "/var/www/html/Intranet/Distribution/";
			break;
		}else if (supp_groups[j] == 1002){
			//Path
			fpath = "/var/www/html/Intranet/Manufacturing/";
			break;
		}
		//else{
		//	printf("Error: no groups were a match\n");
		//	//return 1;
		//}
    }

	//////////////////////////////////////////////////////////////////////////
	memset(msg, 0, 500);
	//READSIZE = read(cs,message,500);
	READSIZE = recv(sock, msg, 500, 0);
	//printf("Client said: %s\n", message);
	
	if (strcmp(msg, "initTransfer")==0) {
		printf("Init Transfer\n");
		write(sock, "filename", strlen("filename"));
		memset(msg, 0, 500);
	}
	
	memset(msg, 0, 500);
	READSIZE = recv(sock, msg, 500, 0);
	
	//Lock
	pthread_mutex_lock(&lock_x);
	if(strcmp(msg, "initTransfer")!=0 && strlen(msg)>0) {
		printf("File\n");
		write(sock, "begin", strlen("begin"));
		printf("Filename: %s\n", msg);
		
		//*************************************
		/*Receive File from Client */
		char* fr_path = fpath;
		printf(fr_path);
		char revbuf[LENGTH];
		char * fr_name = (char *) malloc(1 + strlen(fr_path)+ strlen(msg) );
		strcpy(fr_name, fr_path);
		strcat(fr_name, msg);
		FILE *fr = fopen(fr_name, "w");
		if(fr == NULL)
			printf("File %s Cannot be opened file on server.\n", fr_name);
		else {
			bzero(revbuf, LENGTH);
			int fr_block_sz = 0;
			int i=0;
			while((fr_block_sz = recv(sock, revbuf, LENGTH, 0)) > 0 ){
				printf("Data Received %d = %d\n",i,fr_block_sz);
				int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
				if(write_sz < fr_block_sz)
					herror("File write failed on server.\n");
				bzero(revbuf, LENGTH);
				i++;
			}
			if(fr_block_sz < 0){
                if(errno == EAGAIN){
                    printf("recv() timed out");
                } else {
                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
                exit(1);
                }
            }   
		
		printf("Ok received from client!\n");
		fclose(fr);
		}
		
		//Changes the person who owns the file
		if(chown(fr_name, myUID, myUID) == -1){
  			printf("Failed chown\n");
		}
		
		memset(msg, 0, 500);
	}

	//Unlock
	pthread_mutex_unlock(&lock_x);
	
	if(read_size == 0){
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1) {
		perror("recv failed");
	}
	
	return 0;
}


	


