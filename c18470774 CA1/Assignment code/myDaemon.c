// Orphan Example
// The child process is adopted by init process, when parent process dies.
#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <time.h>
#include "sync.h"
#include "backup.h"
#include "access.h"

void sig_handler(int sigNum)
{
  if (sigNum == SIGINT){

	 printf("SIGINT Interrupt Received\n");
    openlog("Logs", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Start logging");
    closelog();
  }

  else if(sigNum == SIGIO){

	  	//Locking the intranet folder
		lockIntranet();
		//calling sync function
		sync();
		//unlocking Intranet folder
		unlockIntranet();

  }

  else if(sigNum == SIGPWR){

	    //locking live folder
		lockLive();
		//calling backup method
        backup();
        //unlocking live folder
        unlockLive();

  }
    


}
 
int main()
{
    time_t now;
    struct tm newyear;
    double seconds;
    time(&now);  /* get current time; same as: now = time(NULL)  */
    newyear = *localtime(&now);
    newyear.tm_hour = 1; 
    newyear.tm_min = 0; 
    newyear.tm_sec = 0;
    /*newyear.tm_mon = 0;  
    newyear.tm_mday = 1;*/

    // Implementation for Singleton Pattern if desired (Only one instance running)

    // Create a child process      
    int pid = fork();
 
    if (pid > 0) {
        // if PID > 0 :: this is the parent
        // this process performs printf and finishes
        //printf("Parent process");
        //sleep(10);  // uncomment to wait 10 seconds before process ends
        exit(EXIT_SUCCESS);
    } else if (pid == 0) {
       // Step 1: Create the orphan process
       //printf("Child process");
       
       // Step 2: Elevate the orphan process to session leader, to loose controlling TTY
       // This command runs the process in a new session
       if (setsid() < 0) { exit(EXIT_FAILURE); }

       // We could fork here again , just to guarantee that the process is not a session leader
       int pid = fork();
       if (pid > 0) {
          exit(EXIT_SUCCESS);
       } else {
       
          // Step 3: call umask() to set the file mode creation mask to 0
          // This will allow the daemon to read and write files 
          // with the permissions/access required 
          umask(0);

          // Step 4: Change the current working dir to root.
          // This will eliminate any issues of running on a mounted drive, 
          // that potentially could be removed etc..
          if (chdir("/") < 0 ) { exit(EXIT_FAILURE); }

          // Step 5: Close alaureportl open file descriptors
          /* Close all open file descriptors */
          int x;
          /*for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
          {
             close (x);
          } */

          // Signal Handler goes here
          if (signal(SIGINT, sig_handler) == SIG_ERR) 
          {
			  printf("\nSomething went wrong!!\n");

          }
		   //sync
		   else if (signal(SIGIO, sig_handler) == SIG_ERR)
		   {   
			   printf("\nSomething went wrong!!\n");

		   }
		   //backup
		   else if (signal(SIGPWR, sig_handler) == SIG_ERR)
		   {
			   printf("\nSomething went wrong!!\n");
			  	
		   }

          // Log file goes here
          //starting watch on Intranet directory
          system("sudo auditctl -w /var/www/html/Intranet/ -p wrxa");


          // Orphan Logic goes here!! 
          // Keep process running with infinite loop
          // When the parent finishes after 10 seconds, 
          // the getppid() will return 1 as the parent (init process)
         
          while(1) {
             sleep(1);
             //printf("child 1: my parent is: %i\n", getppid());
             time(&now);
             seconds = difftime(now,mktime(&newyear));
             printf("\n%.f", seconds);
             if (seconds == 0) {

               //audit doing the audit
               system("sudo ausearch -f /var/www/html/Intranet/ | aureport -f -i > /var/www/html/Logs/Auditlog.txt");

               //Locking the intranet folder
		    	   lockIntranet();
		         //calling sync function
		         sync();
		         //unlocking Intranet folder
		         unlockIntranet();
			
			
		         //locking live folder
		         lockLive();
		         //calling backup method
               backup();
               //unlocking live folder
               unlockLive();
              		
           	
	         }//end outer if
          }//end while
       }
    }
 
    return 0;
}
