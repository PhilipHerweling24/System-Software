#include  <stdio.h>
#include  <sys/types.h>
#include  <stdlib.h>
#include  <time.h>
 
void backup() {

	time_t t;
	time(&t);

   //Destination folder for backup and renaiming the folder to the current date and time
   char destination[] = "/var/www/html/Backup/";
   char name[80];
   strcpy(name, ctime(&t));
   strcat(destination,name);

	char* cp_list[] = {"cp","-R", "/var/www/html/Live/", destination, NULL};
     	pid_t  pid;
     
     	pid = fork();

        if (pid == 0) // child
        {
           printf ("Executing command cp...\n");
           execvp ("/bin/cp", cp_list);
        }
        else // parent
        {
          int status=0;
          wait(&status);
          printf ("Child process is returned with: %d.\n",status);
        }
     
}
