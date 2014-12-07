//==============================================================================
//						      Messaging.c
//						Messaging pipe handling
//							Open piAquarium	
//                            27/9/2013
//				  Copyright (c) A.Brousalis InviBIT, 2014
//================================================================================
//  This file is part of piAquarium:
//	https://sites.google.com/site/openpiaquarium/
//
//    piAquarium is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    piAquarium is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with piAquarium.
//    If not, see <http://www.gnu.org/licenses/>.
//==============================================================================

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
extern OUTP Outputs;								// 	Structure containing all the io states
extern ADC	ADCa;
extern ADC	ADCb;
extern ALARMS alrm;
//************************************************************************
//		Com_Fifo_update 
//		Responds to requests from server. Commands supprorted :
//	- $REAL : returns the pH,Temperature,Env1 Temperature,Env1 Humidity,Env2 Temperature,Env2 Humidity  
//	- $ROW  : returns the row filtered AD values  (ADCa.chA,ADCa.chB,ADCb.chA,ADCb.chB,ADCb.chB)
//	- $ALARMS : returns the current alarm status 
//  - $LIGHTS : returns the Lights status as <State,Intensity>  (1,99,0,0,1,50)
//	- @TIME	  : returns the system time 
//
//************************************************************************
void Com_Fifo_update(void)
{
	int nbytes,len;

	nbytes = read(FrmSrv_fd,buf, MAX_MESS_BUFF);	
	if(nbytes > 0){
		//*******************************************
		if(strstr (buf, "$REAL") != NULL)
		{			
			len = sprintf(obuf,"%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f\0",Real.TankpH, Real.TankTemp, Real.EnvTemp1, Real.EnvHumidity1,Real.EnvTemp2, Real.EnvHumidity2);			
			write(ToSrv_fd, obuf, len);
		}else	
		//*******************************************
		if(strstr (buf, "$LIGHTS") != NULL)
		{			
			len = sprintf(obuf,"%01d,%03d,%01d,%03d,%01d,%03d\0",Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity);			
			write(ToSrv_fd, obuf, len);
		}else
		//*******************************************
		if(strstr (buf, "$ROW") != NULL)
		{			
			len = sprintf(obuf,"%d,%d\0",ADCa.chA_fltr, ADCa.chB_fltr);		
			write(ToSrv_fd, obuf, len);
		}else
		//*******************************************
		if(strstr (buf, "$ALARMS") != NULL)
		{			
			len = sprintf(obuf,"%01d,%01d,%01d,%01d\0",alrm.TempHigh,alrm.TempLow,alrm.pH_High,alrm.pH_Low);
			write(ToSrv_fd, obuf, len);
		}
		//*******************************************

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
  
	FrmSrv_fd = open(FrmServMesFifo, O_RDWR | O_NONBLOCK);
    if (FrmSrv_fd == -1) {
        perror("open");
        return;
    }

	ToSrv_fd = open(ToServMesFifo, O_RDWR | O_NONBLOCK);
    if (ToSrv_fd == -1) {
        perror("open");
        return;
    }	
	
}
//************************************************************************
void Com_Fifo_kill(void)
{
	close(ToSrv_fd);
	close(FrmSrv_fd);
 /* remove the FIFO */
    unlink(ToServMesFifo);
    unlink(FrmServMesFifo);
}
//************************************************************************
//************************************************************************