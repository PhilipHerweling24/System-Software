#include  <stdio.h>
#include  <sys/types.h>
#include  <stdlib.h>

void sync() {
     	char* rsync_list[] = {"rsync","-r", "/var/www/html/Intranet/", "/var/www/html/Live/", NULL};
     	pid_t  pid;
   
        pid = fork();

        if (pid == 0) // child
        {
           printf ("Executing command rsync...\n");
           execvp ("/bin/rsync", rsync_list);
        }
        else // parent
        {
          int status=0;
          wait(&status);
          printf ("Child process is returned with: %d.\n",status);
        }
     
}

     

     
