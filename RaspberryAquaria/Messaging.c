#include <sys/select.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/poll.h>
#include <unistd.h>
#include "defines.h"
#include "config.h"
#include "database.h"

//*********** message FIFO -> server ************ 
int ToSrv_fd;																// app to Server message fifo handler
int FrmSrv_fd;																// Server to application message fifo
   
char *ToServMesFifo = "/tmp/fifoToSrv";
char *FrmServMesFifo = "/tmp/fifoFrmSrv";

#define		MAX_MESS_BUFF		1024
char buf[MAX_MESS_BUFF];
char obuf[MAX_MESS_BUFF];

extern REALMEAS Real;	
//************************************************************************

//************************************************************************
void Com_Fifo_update(void)
{
	int nbytes;

	nbytes = read(FrmSrv_fd,buf, MAX_MESS_BUFF);
	if(nbytes){
		if(strstr (buf, "#DATA_1") != NULL)
		{
			sprintf(obuf,"%0.2f/r/n",Real.TankTemp);
		}
	
		write(ToSrv_fd, obuf, sizeof(obuf));
	}
}
//************************************************************
//			Initialize communication FIFO
//************************************************************
void Com_Fifo_init(void)
{
	mkfifo(ToServMesFifo, 0666);
	mkfifo(FrmServMesFifo, 0666);
 // system("mkfifo /tmp/PIPE_FRM_SRV");
 // system("mkfifo /tmp/PIPE_TO_SRV");
 // open file descriptors of named pipes to watch
  
	ToSrv_fd = open(ToServMesFifo, O_RDWR | O_NONBLOCK);
    if (ToSrv_fd == -1) {
        perror("open");
        return(1);
    }	
	
}
//************************************************************************
void Com_Fifo_kill(void)
{
	close(ToSrv_fd);
	close(FrmSrv_fd);
 /* remove the FIFO */
    unlink(ToServMesFifo);
 // unlink(FromServMesFifo);
}
//************************************************************************
//************************************************************************