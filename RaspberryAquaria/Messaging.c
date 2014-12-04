#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//*********** message FIFO -> server ************ 
int ToSrv_fd;																// app to Server message fifo handler
int FrmSrv_fd;																// Server to application message fifo
   
char *ToServMesFifo = "/tmp/fifoToSrv";
char *FrmServMesFifo = "/tmp/fifoFrmSrv";

#define		MAX_MESS_BUFF		1024
char buf[MAX_MESS_BUFF];
//************************************************************************

//************************************************************************
void Com_Fifo_update(void)
{
	int nbytes;

	nbytes = read(FrmSrv_fd,buf, MAX_MESS_BUFF);
	if(nbytes){
		write(ToSrv_fd, "Hi", sizeof("Hi"));
	}
}
//************************************************************************
void Com_Fifo_init(void)
{
//************************************************************
	//			Initialize communication FIFO
	//************************************************************
//#if 0
    //mkfifo(ToServMesFifo, 0666);
	//mkfifo(FromServMesFifo, 0666);


	mkfifo(ToServMesFifo, 0666);
	mkfifo(FrmServMesFifo, 0666);

	//system("mkfifo /tmp/PIPE_FRM_SRV");
    //system("mkfifo /tmp/PIPE_TO_SRV");
    // open file descriptors of named pipes to watch
    ToSrv_fd = open(ToServMesFifo, O_RDWR | O_NONBLOCK);
    if (ToSrv_fd == -1) {
        perror("open");
        return(1);

    }	
	
//#endif


}
//************************************************************************
void Com_Fifo_kill(void)
{
	close(ToSrv_fd);
	close(FrmSrv_fd);
    /* remove the FIFO */
    unlink(ToServMesFifo);
	//unlink(FromServMesFifo);
}
//************************************************************************
//************************************************************************