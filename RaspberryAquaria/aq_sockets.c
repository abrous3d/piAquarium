//================================================================================
//					  Open Aquarium controller
//                          27/9/2013
//							aq_sockets.c
//				  Copyright (c) A.Brousalis InviBIT, 2014
//						abrous3d@hotmail.com
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "defines.h"
#include "config.h"
#include "database.h"
//==============================================================================
extern REALMEAS Real;	
extern OUTP Outputs;								// 	Structure containing all the io states
extern ADC	ADCa;
extern ADC	ADCb;
extern ALARMS alrm;
//==============================================================================
void error(char *msg) {
    perror(msg);
    exit(1);
}
//==============================================================================
//						socket globals
//================================================================================	
#define		AQ_PORT			4440					// Fix me -> move this to configuration

int sockfd, newsockfd, portno;
socklen_t clilen;
struct sockaddr_in serv_addr, cli_addr;
//================================================================================
int SockProcess(void)
{
	static unsigned int SockInit;
	int n;
	char buffer[256];
	char obuf[1024];

	if(SockInit != 0xDEADCAFE){

		printf("\nIPv4 TCP Server Started...\n");        	
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			error("ERROR opening socket");
		bzero((char *) &serv_addr, sizeof(serv_addr));
		fcntl(sockfd, F_SETFL, O_NONBLOCK);
		portno = AQ_PORT;

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		//server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		serv_addr.sin_port = htons(portno);

    
		//Sockets Layer Call: bind()
		if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
			error("ERROR on binding");

		//Sockets Layer Call: listen()
		listen(sockfd, 5);
		clilen = sizeof(cli_addr);
		SockInit = 0xDEADCAFE;
	}
    //Sockets Layer Call: accept()
   
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);	
    if (newsockfd < 0){
		//error("ERROR on accept");					// handle this error case
		return(-1);
	}
    //Sockets Layer Call: inet_ntoa()	
    printf("Incoming connection from client having IPv4 address: %s\n",    inet_ntoa(cli_addr.sin_addr));

    memset(buffer,0, 256);
    
    //Sockets Layer Call: recv()
    //n = recv(newsockfd, buffer, 255, 0);
    //if (n < 0)	error("ERROR reading from socket");

    //printf("Message from client: %s\n", buffer);

    int len = sprintf(obuf,"%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%01d,%03d,%01d,%03d,%01d,%03d,%d,%d,%01d,%01d,%01d,%01d\0", Real.TankpH, Real.TankTemp, Real.EnvTemp1, Real.EnvHumidity1,Real.EnvTemp2, Real.EnvHumidity2,Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity,ADCa.chA_fltr, ADCa.chB_fltr,alrm.TempHigh,alrm.TempLow,alrm.pH_High,alrm.pH_Low);	
	
	//int len = sprintf(obuf,"{\"pH\":\"%0.2f\"}, {\"temp\":\"%0.2f\"}, {\"envtmp\":\"%0.2f\"}, {\"%0.2f,%0.2f,%0.2f,%01d,%03d,%01d,%03d,%01d,%03d,%d,%d,%01d,%01d,%01d,%01d\0", 
	//	Real.TankpH, Real.TankTemp, Real.EnvTemp1, Real.EnvHumidity1,Real.EnvTemp2, Real.EnvHumidity2,Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity,ADCa.chA_fltr, ADCa.chB_fltr,alrm.TempHigh,alrm.TempLow,alrm.pH_High,alrm.pH_Low);	
	//Sockets Layer Call: send()
    n = send(newsockfd, obuf, len, 0);
    if (n < 0)
        error("ERROR writing to socket");
    
    //Sockets Layer Call: close()
    //close(sockfd);
    close(newsockfd);

	//SockInit = 0;
    
    return 0;
}