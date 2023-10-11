#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <time.h>

void lockLive() {
    char mode[] = "0700";
	char buf[100] = "/var/www/html/Live";
	int i;
	i = strtol(mode, 0, 8);
	if (chmod (buf,i) < 0)
	{
		printf("Error Locking Live Directory\n");
	} 
    else 
    {
        printf("Successfully locked Live Directory\n");
    }
}

void lockIntranet() {
    char mode[] = "0700";
	char buf[100] = "/var/www/html/Intranet";
	int i;
	i = strtol(mode, 0, 8);
	if (chmod (buf,i) < 0)
	{
		printf("Error Locking Intranet Directory\n");
	} 
    else 
    {
        printf("Successfully locked Intranet Directory\n");
    }
}

void unlockLive() {
    char mode[] = "0777";
	char buf[100] = "/var/www/html/Live";
	int i;
	i = strtol(mode, 0, 8);
	if (chmod (buf,i) < 0)
	{
		printf("Error Unlocking Live Directory\n");
	} 
    else 
    {
        printf("Successfully unlocked Live Directory\n");
    }
}

void unlockIntranet() {
    char mode[] = "0777";
	char buf[100] = "/var/www/html/Intranet";
	int i;
	i = strtol(mode, 0, 8);
	if (chmod (buf,i) < 0)
	{
		printf("Error unlocking Intranet Directory\n");
	} 
    else 
    {
        printf("Successfully unlocked Intranet Directory\n");
    }
}
