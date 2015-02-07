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

extern int A[MAX_EVENTS][3];						//[0] -> Time in seconds since 00:00 [1] -> 0-100% intensity
extern int B[MAX_EVENTS][3];
extern int C[MAX_EVENTS][3];	
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
	char buffer[1024];
	char obuf[2048];

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
    //printf("Incoming connection from client having IPv4 address: %s\n",    inet_ntoa(cli_addr.sin_addr));

    memset(buffer,0, sizeof(buffer));
    
    //Sockets Layer Call: recv()
    n = recv(newsockfd, buffer, sizeof(buffer), 0);
    if (n < 0)	error("ERROR reading from socket");

	int len = 0;


	if(strstr(buffer, "getread")) 
	{
		len = sprintf(obuf,	"HTTP/1.1 200 OK\r\n" 
								"Content-type:application/JSON"
								"\r\n\r\n"
								"{\"pH\":\"%.3f\", \"temp\":\"%.3f\", \"envtmp1\":\"%.3f\", \"envhum1\":\"%.3f\", \"envtmp2\":\"%.3f\", \"envhum2\":\"%.3f\", \"l0on\":\"%01d\", \"l0int\":\"%03d\", \"l1on\":\"%01d\", \"l1int\":\"%03d\", \"l2on\":\"%01d\", \"l2int\":\"%03d\", \"row1\":\"%d\", \"row2\":\"%d\", \"al_th\":\"%01d\", \"al_tl\":\"%01d\", \"al_ph\":\"%01d\", \"al_pl\":\"%01d\", \"ther_on\":\"%01d\", \"fan_on\":\"%01d\", \"co2_on\":\"%01d\" }", Real.TankpH, Real.TankTemp, Real.EnvTemp1, Real.EnvHumidity1,Real.EnvTemp2, Real.EnvHumidity2,Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity,ADCa.chA_fltr, ADCa.chB_fltr,alrm.TempHigh,alrm.TempLow,alrm.pH_High,alrm.pH_Low,Outputs.HeaterIsON,Outputs.FanIsON,Outputs.CO2IsON);		
	}
	else if(strstr(buffer, "gettable")) 
	{
		len = sprintf(obuf,	"HTTP/1.1 200 OK\r\n" 
							"Content-type:application/JSON"
							"\r\n\r\n"
							"{\"A0_0\":\"%d\",\"A0_1\":\"%d\","
							 "\"A1_0\":\"%d\",\"A1_1\":\"%d\","
							 "\"A2_0\":\"%d\",\"A2_1\":\"%d\","
							 "\"A3_0\":\"%d\",\"A3_1\":\"%d\","
							 "\"A4_0\":\"%d\",\"A4_1\":\"%d\","
							 "\"A5_0\":\"%d\",\"A5_1\":\"%d\","
							 "\"A6_0\":\"%d\",\"A6_1\":\"%d\","
							 "\"A7_0\":\"%d\",\"A7_1\":\"%d\","
							 "\"A8_0\":\"%d\",\"A8_1\":\"%d\","
							 "\"A9_0\":\"%d\",\"A9_1\":\"%d\","
							 "\"A10_0\":\"%d\",\"A10_1\":\"%d\","
							 "\"A11_0\":\"%d\",\"A11_1\":\"%d\","
							 "\"A12_0\":\"%d\",\"A12_1\":\"%d\","
							 "\"A13_0\":\"%d\",\"A13_1\":\"%d\","
							 "\"A14_0\":\"%d\",\"A14_1\":\"%d\","
							 "\"A15_0\":\"%d\",\"A15_1\":\"%d\","
							 "\"B0_0\":\"%d\",\"B0_1\":\"%d\","
							 "\"B1_0\":\"%d\",\"B1_1\":\"%d\","
							 "\"B2_0\":\"%d\",\"B2_1\":\"%d\","
							 "\"B3_0\":\"%d\",\"B3_1\":\"%d\","
							 "\"B4_0\":\"%d\",\"B4_1\":\"%d\","
							 "\"B5_0\":\"%d\",\"B5_1\":\"%d\","
							 "\"B6_0\":\"%d\",\"B6_1\":\"%d\","
							 "\"B7_0\":\"%d\",\"B7_1\":\"%d\","
							 "\"B8_0\":\"%d\",\"B8_1\":\"%d\","
							 "\"B9_0\":\"%d\",\"B9_1\":\"%d\","
							 "\"B10_0\":\"%d\",\"B10_1\":\"%d\","
							 "\"B11_0\":\"%d\",\"B11_1\":\"%d\","
							 "\"B12_0\":\"%d\",\"B12_1\":\"%d\","
							 "\"B13_0\":\"%d\",\"B13_1\":\"%d\","
							 "\"B14_0\":\"%d\",\"B14_1\":\"%d\","
							 "\"B15_0\":\"%d\",\"B15_1\":\"%d\","
							 "\"C0_0\":\"%d\",\"C0_1\":\"%d\","
							 "\"C1_0\":\"%d\",\"C1_1\":\"%d\","
							 "\"C2_0\":\"%d\",\"C2_1\":\"%d\","
							 "\"C3_0\":\"%d\",\"C3_1\":\"%d\","
							 "\"C4_0\":\"%d\",\"C4_1\":\"%d\","
							 "\"C5_0\":\"%d\",\"C5_1\":\"%d\","
							 "\"C6_0\":\"%d\",\"C6_1\":\"%d\","
							 "\"C7_0\":\"%d\",\"C7_1\":\"%d\","
							 "\"C8_0\":\"%d\",\"C8_1\":\"%d\","
							 "\"C9_0\":\"%d\",\"C9_1\":\"%d\","
							 "\"C10_0\":\"%d\",\"C10_1\":\"%d\","
							 "\"C11_0\":\"%d\",\"C11_1\":\"%d\","
							 "\"C12_0\":\"%d\",\"C12_1\":\"%d\","
							 "\"C13_0\":\"%d\",\"C13_1\":\"%d\","
							 "\"C14_0\":\"%d\",\"C14_1\":\"%d\","
							 "\"C15_0\":\"%d\",\"C15_1\":\"%d\"}",
							 A[0][0],A[0][1], 
							 A[1][0],A[1][1], 
							 A[2][0],A[2][1], 
							 A[3][0],A[3][1], 
							 A[4][0],A[4][1], 
							 A[5][0],A[5][1],
							 A[6][0],A[6][1],
							 A[7][0],A[7][1],
							 A[8][0],A[8][1],
							 A[9][0],A[9][1],
							 A[10][0],A[10][1],
							 A[11][0],A[11][1],
							 A[12][0],A[12][1],
							 A[13][0],A[13][1],
							 A[14][0],A[14][1],
							 A[15][0],A[15][1],							 
							 B[0][0],B[0][1], 
							 B[1][0],B[1][1], 
							 B[2][0],B[2][1], 
							 B[3][0],B[3][1], 
							 B[4][0],B[4][1], 
							 B[5][0],B[5][1],
							 B[6][0],B[6][1],
							 B[7][0],B[7][1],
							 B[8][0],B[8][1],
							 B[9][0],B[9][1],
							 B[10][0],B[10][1],
							 B[11][0],B[11][1],
							 B[12][0],B[12][1],
							 B[13][0],B[13][1],
							 B[14][0],B[14][1],
							 B[15][0],B[15][1],		
							 C[0][0],C[0][1], 
							 C[1][0],C[1][1], 
							 C[2][0],C[2][1], 
							 C[3][0],C[3][1], 
							 C[4][0],C[4][1], 
							 C[5][0],C[5][1],
							 C[6][0],C[6][1],
							 C[7][0],C[7][1],
							 C[8][0],C[8][1],
							 C[9][0],C[9][1],
							 C[10][0],C[10][1],
							 C[11][0],C[11][1],
							 C[12][0],C[12][1],
							 C[13][0],C[13][1],
							 C[14][0],C[14][1],
							 C[15][0],C[15][1]);
}

    //printf("Message from client: %s\n", buffer);

    //int len = sprintf(obuf,"%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%01d,%03d,%01d,%03d,%01d,%03d,%d,%d,%01d,%01d,%01d,%01d\0", Real.TankpH, Real.TankTemp, Real.EnvTemp1, Real.EnvHumidity1,Real.EnvTemp2, Real.EnvHumidity2,Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity,ADCa.chA_fltr, ADCa.chB_fltr,alrm.TempHigh,alrm.TempLow,alrm.pH_High,alrm.pH_Low);	
	
	//int len = sprintf(obuf,"{\"Realv\":{\"pH\":\"%0.2f\",\"temp\":\"%0.2f\",\"envtmp1\":\"%0.2f\",\"envhum1\":\"%0.2f\",\"envtmp2\":\"%0.2f\",\"envhum2\":\"%0.2f\",\"l0on\":\"%01d\",\"l0int\":\"%03d\",\"l1on\":\"%01d\",\"l1int\":\"%03d\",\"l2on\":\"%01d\",\"l2int\":\"%03d\",\"row1\":\"%d\",\"row2\":\"%d\",\"al_th\":\"%01d\",\"al_tl\":\"%01d\",\"al_ph\":\"%01d\",\"al_pl\":\"%01d\",\"ther_on\":\"%01d\",\"fan_on\":\"%01d\",\"co2_on\":\"%01d\"}}\0", Real.TankpH, Real.TankTemp, Real.EnvTemp1, Real.EnvHumidity1,Real.EnvTemp2, Real.EnvHumidity2,Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity,ADCa.chA_fltr, ADCa.chB_fltr,alrm.TempHigh,alrm.TempLow,alrm.pH_High,alrm.pH_Low,Outputs.HeaterIsON,Outputs.FanIsON,Outputs.CO2IsON);	
	//int len = sprintf(obuf,"{\"pH\":\"%0.2f\",\"temp\":\"%0.2f\",\"envtmp1\":\"%0.2f\",\"envhum1\":\"%0.2f\",\"envtmp2\":\"%0.2f\",\"envhum2\":\"%0.2f\",\"l0on\":\"%01d\",\"l0int\":\"%03d\",\"l1on\":\"%01d\"},{\"l1int\":\"%03d\",\"l2on\":\"%01d\",\"l2int\":\"%03d\",\"row1\":\"%d\",\"row2\":\"%d\",\"al_th\":\"%01d\",\"al_tl\":\"%01d\",\"al_ph\":\"%01d\",\"al_pl\":\"%01d\",\"ther_on\":\"%01d\",\"fan_on\":\"%01d\",\"co2_on\":\"%01d\"}\0", Real.TankpH, Real.TankTemp, Real.EnvTemp1, Real.EnvHumidity1,Real.EnvTemp2, Real.EnvHumidity2,Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity,ADCa.chA_fltr, ADCa.chB_fltr,alrm.TempHigh,alrm.TempLow,alrm.pH_High,alrm.pH_Low,Outputs.HeaterIsON,Outputs.FanIsON,Outputs.CO2IsON);	
	//int len = sprintf(obuf,"{ \"pH\":\"%0.2f\", \"temp\":\"%0.2f\", \"envtmp1\":\"%0.2f\", \"envhum1\":\"%0.2f\", \"envtmp2\":\"%0.2f\", \"envhum2\":\"%0.2f\", \"l0on\":\"%01d\", \"l0int\":\"%03d\", \"l1on\":\"%01d\", \"l1int\":\"%03d\", \"l2on\":\"%01d\", \"l2int\":\"%03d\", \"row1\":\"%d\", \"row2\":\"%d\", \"al_th\":\"%01d\", \"al_tl\":\"%01d\", \"al_ph\":\"%01d\", \"al_pl\":\"%01d\", \"ther_on\":\"%01d\", \"fan_on\":\"%01d\", \"co2_on\":\"%01d\" }\0", Real.TankpH, Real.TankTemp, Real.EnvTemp1, Real.EnvHumidity1,Real.EnvTemp2, Real.EnvHumidity2,Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity,ADCa.chA_fltr, ADCa.chB_fltr,alrm.TempHigh,alrm.TempLow,alrm.pH_High,alrm.pH_Low,Outputs.HeaterIsON,Outputs.FanIsON,Outputs.CO2IsON);	

	//int len = sprintf(obuf,"{ \"pH\":\"7\", \"temp\":\"20\", \"env1\":\"20\" }\0");	
	
	



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