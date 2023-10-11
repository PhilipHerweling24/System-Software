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
#include <pwd.h>

#define _POSIX_SOURCE
#define LENGTH 512

int main(int argc, char *argv[])
{
	int SID;
	struct sockaddr_in server;
	char clientMessage[500];
	char ServerMessage[500];
	
	char filename[500];

	uid_t uid = getuid();
    uid_t gid = getgid();
    uid_t ueid = geteuid();
    uid_t geid = getegid();

    printf("User ID: %d\n", uid);
    printf("Group ID: %d\n", gid);
    printf("E User ID: %d\n", ueid);
    printf("E Group ID: %d\n", geid);

	//Found This code at https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getpwuid-access-user-database-by-user-id
	//It finds the username of the user 
	//And stores it in the variable username
	struct passwd *p = getpwuid(getuid());  // Check for NULL!
    //printf("User name: %s\n", p->pw_name);
	char userName[500];
	strcpy(userName, p->pw_name);
	printf("User name: %s\n", userName);


	strcpy(filename, argv[1]);
	
	//Create socket
	SID = socket(AF_INET, SOCK_STREAM, 0);
	if (SID == -1)
	{
		printf("Error creating socket");
	}else 
	{
		printf("socket created");
	}
	
	//set sockaddr_in variables
	server.sin_port = htons(8888);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	
	//Connect to server
	if(connect(SID, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("connect failed. ERROR");
		return 1;
	}
	
	printf("connected to server ok!\n");
	
	//Keep communicating with server

	memset(ServerMessage, 0, 500);


	//Send username//////////////////////////////////////////////////////
	if(send(SID, userName, strlen(userName), 0) <0)
	{
		printf("Username Send failed");
		return 1;
	}

	//Receive a reply from the server after recieving username
	if(recv(SID, ServerMessage, 500, 0) <0)
	{
		printf("IO error");
	}
	printf("server sent us: %s of length %d\n", ServerMessage, strlen(ServerMessage));
	///////////////////////////////////////////////////////////////////
	
	char userID[500];
	sprintf(userID, "%d", uid);
	memset(ServerMessage, 0, 500);

	//Send ID//////////////////////////////////////////////////////////////
	if(send(SID, userID, strlen(userID), 0) <0)
	{
		printf("UserID Send failed");
		return 1;
	}

	//Receive a reply from the server after recieving User ID
	if(recv(SID, ServerMessage, 500, 0) <0)
	{
		printf("IO error");
	}
	printf("server sent us: %s of length %d\n", ServerMessage, strlen(ServerMessage));
	/////////////////////////////////////////////////////////////////////////

	memset(ServerMessage, 0, 500);
	//printf("\nEnter Message: ");
	//scanf("%s" , clientMessage);
	
	//Send some data
	if(send(SID, "initTransfer", strlen("initTransfer"), 0) <0)
	{
		printf("Send failed");
		return 1;
	}
	
	//Receive a reply from the server
	if(recv(SID, ServerMessage, 500, 0) <0)
	{
		printf("IO error");
	}
	printf("server sent us: %s of length %d\n", ServerMessage, strlen(ServerMessage));
	
	if (strcmp(ServerMessage, "filename") == 0)
	{
		printf("\nSending Filename %s\n", filename);
		if(send(SID, filename, strlen(filename), 0)<0)
		{
			printf("Send failed");
			return 1;
		}
	}
	
	memset(ServerMessage, 0, 500);
	//Recieve a reply from the server
	if (recv(SID, ServerMessage, 500, 0) < 0)
	{
		printf("IO error");
	}
	printf("Server sent us: %s of length %d\n", ServerMessage, strlen(ServerMessage));
	
	if(strcmp(ServerMessage, "begin") == 0){

        //getchar();
        printf("Sending File: %s", filename);
        char* fs_path = "/var/www/html/Intranet/Client/";

        char * fs_name = (char *) malloc(1 + strlen(fs_path) + strlen(filename));
        strcpy(fs_name, fs_path);
        // Concatonating the path and the filename
        strcat(fs_name, filename);

        char sdbuf[LENGTH];
        printf("Client Sending: %s to the Server\n", fs_name);
        FILE *fs = fopen(fs_name, "r");
        if(fs == NULL){
            printf("Error, File Not Found! %s\n", fs_name);
            return 1;
        }
        
        bzero(sdbuf, LENGTH);
        int fs_block_sz, i = 0;
        
        while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0){
            printf("Data Sent %d = %d\n", i, fs_block_sz);

            if(send(SID, sdbuf, fs_block_sz, 0) < 0){
                fprintf(stderr, "Error, Failed to send File: %s, (errno = %d)\n", fs_name, errno);
                exit(1);
            }
            bzero(sdbuf, LENGTH);
            i++;
        }
    }
    close(SID);
	return 0;
	
}
		
	
	
	
	