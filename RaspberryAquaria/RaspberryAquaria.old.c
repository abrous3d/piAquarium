/*****************************************************************************
*				Aquarium control simulation
* 				   Angelo Brousalis 2014	
* ****************************************************************************
*	  Build ->    gcc -Wall -o aqua miniIni.c th.c -lwiringPi -lpthread
* ****************************************************************************
*	    		  Control File Format 
* ****************************************************************************
*	[Lights]	
*   A1=10:30,0
*	A2=11:00,80
*	A3=12:30,80	
*	A2=11:00,80
*	A3=12:30,80
*	A4=14:00,100
*	A5=21:00,100
*	A6=21:30,0
*	B2= ...
*	B3= ...
* 	C1= ...
* 
*	[temperature]
*   on_off = 1				
* 	Set = 26.5
*   Hyst = 0.3
* 	Alarm = 3	
*   Fan = 27.5   	
* 
*	[ph]
* 	phSet = 6.7
*   Hyst = 0.3
*   phAlarm = 0.5	
*****************************************************************************
+-----+-----+---------+------+---+--B Plus--+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 |   IN | 1 |  3 || 4  |   |      | 5V      |     |     |
 |   3 |   9 |   SCL.1 |   IN | 1 |  5 || 6  |   |      | 0v      |     |     |
 |   4 |   7 | GPIO. 7 |   IN | 0 |  7 || 8  | 1 | ALT0 | TxD     | 15  | 14  |
 |     |     |      0v |      |   |  9 || 10 | 1 | ALT0 | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |
 |  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
 |     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI |   IN | 0 | 19 || 20 |   |      | 0v      |     |     |
 |   9 |  13 |    MISO |   IN | 0 | 21 || 22 | 0 | IN   | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK |   IN | 0 | 23 || 24 | 0 | IN   | CE0     | 10  | 8   |
 |     |     |      0v |      |   | 25 || 26 | 0 | IN   | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 |   IN | 0 | 27 || 28 | 0 | IN   | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 |   IN | 0 | 29 || 30 |   |      | 0v      |     |     |
 |   6 |  22 | GPIO.22 |   IN | 0 | 31 || 32 | 0 | IN   | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 |   IN | 0 | 33 || 34 |   |      | 0v      |     |     |
 |  19 |  24 | GPIO.24 |   IN | 0 | 35 || 36 | 1 | OUT  | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.28 | 28  | 20  |
 |     |     |      0v |      |   | 39 || 40 | 0 | IN   | GPIO.29 | 29  | 21  |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+--B Plus--+---+------+---------+-----+-----+
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "minIni.h"
#include <wiringPi.h>
#include <softPwm.h>
//***************************************************************************
#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))
//***************************************************************************
#define		PWM0_PIN		7
#define		PWM1_PIN		0
#define		PWM2_PIN		2

#define		SW_L0_PIN		3
#define		SW_L1_PIN		1
#define		SW_L2_PIN		4

#define		SW_HEAT_PIN		5
#define		SW_FAN_PIN		6

#define		SW_AUX0_PIN		19
#define		SW_AUX1_PIN		20
//***************************************************************************
#define 	FDIVISOR		100000												// used to convert PWM to Fixed point		
#define 	X_DEBUG			1
//***************************************************************************
typedef struct pwmchannel
{	
	int CurrentPWM_out;
	int TargetPWM_out;
	int PWMincrement;	
	int LastPWMval;
}	PWMx;
//***************************************************************************
void InitSchedulerTables(void);
void StrDecompose(char *str, int Table, int entry);
void EventProcess(int ARR[][3],int CurTimeInSec);
void TestLoop(void);
void EventTake(int Chan,int Ctime,int val,int NextTime,int NextVal);
void PWMinit(PWMx *ch);
void PWMupdate(PWMx *ch, int target, int increment);
void PWMservice(PWMx *ch);
//***************************************************************************
const char inifile[] = "schedule.ini";
//**************************************************************************************************
//											Event tables
//	col0 = time in seconds since 00:00, col1 = target pwm, col2 = event state (0=pending, 1= done)
//**************************************************************************************************
#define 	MAX_EVENTS		16				
//========================================
int A[MAX_EVENTS][3]; 
int B[MAX_EVENTS][3];
int C[MAX_EVENTS][3];	

PWMx PWMa;
PWMx PWMb;
PWMx PWMc;
//**************************************************************************************************
//
//**************************************************************************************************
int main(void)
{
	int ClockTick;
	time_t timer;
	struct tm * xtime;
	
#ifdef X_DEBUG  
	printf("Raspi Aquarium controler \r\n"); 
	fflush(stdout);
	printf("abrous3D 2014\r\n"); 
	fflush(stdout);
#endif  

	if( access( inifile, F_OK ) != -1 ) {
		printf("Config File ok..\r\n");		
		fflush(stdout);
	} else {

#ifdef X_DEBUG  
		printf("Warning : No Init file\r\n");
		fflush(stdout);
#endif			
		exit(1);
	}
		
	//===================  Init WiringPi =====================
	wiringPiSetup() ;
    piHiPri(55) ;
	//=================== Setup Pins =========================
	pullUpDnControl (PWM0_PIN, PUD_UP);
	pullUpDnControl (PWM1_PIN, PUD_UP);
	pullUpDnControl (PWM2_PIN, PUD_UP);
	softPwmCreate (PWM0_PIN, 0, 100) ;
	softPwmCreate (PWM1_PIN, 0, 100) ;
    softPwmCreate (PWM2_PIN, 0, 100) ;
	pinMode (SW_L0_PIN, OUTPUT) ;
	pullUpDnControl (SW_L0_PIN, PUD_DOWN);
	digitalWrite (SW_L0_PIN, LOW) ;
	
	pinMode (SW_L1_PIN, OUTPUT) ;
	pullUpDnControl (SW_L1_PIN, PUD_DOWN);
	digitalWrite (SW_L1_PIN, LOW) ;
	
	pinMode (SW_L2_PIN, OUTPUT) ;
	pullUpDnControl (SW_L2_PIN, PUD_DOWN);
	digitalWrite (SW_L2_PIN, LOW) ;

	pinMode (SW_HEAT_PIN, OUTPUT) ;
	pullUpDnControl (SW_HEAT_PIN, PUD_DOWN);
	digitalWrite (SW_HEAT_PIN, LOW) ;
#if 0			
	pinMode (SW_FAN_PIN, OUTPUT) ;
	pullUpDnControl (SW_FAN_PIN, PUD_DOWN);
	digitalWrite (SW_FAN_PIN, LOW) ;

	pinMode (SW_AUX1_PIN, OUTPUT) ;
	pullUpDnControl (SW_AUX1_PIN, PUD_DOWN);
	digitalWrite (SW_AUX1_PIN, LOW) ;
#endif
	
	PWMinit(&PWMa);
	PWMinit(&PWMb);
	PWMinit(&PWMc);

	InitSchedulerTables();

	{	
	//static int flag;	
	//if(flag !=1234567){flag =1234567; ClockTick = 86350;}
	}
	
	for(;;)
	{
		time(&timer);  													    // get current time; 
		xtime = localtime (&timer);										
		ClockTick = xtime->tm_hour * 3600;
		ClockTick += xtime->tm_min * 60;									// Calculate seconds since 00:00
		
		if(ClockTick++ > 86400)ClockTick = 0;					

		EventProcess(A,ClockTick);
		EventProcess(B,ClockTick);
		EventProcess(C,ClockTick);	
		
		PWMservice(&PWMa);
		PWMservice(&PWMb);
		PWMservice(&PWMc);
						
		delay(50);
		//usleep(500 * 1000);				// 100 * 1000
	}	
	return 0;
}
//**************************************************************************************************
//		Process the event table and execute any pending events   				
//
//**************************************************************************************************
void EventProcess(int ARR[][3],int CurTimeInSec)
{			
	int i;
	int ValidEvents = 0;
	int chan; 

	if(ARR == A)	 chan = 0;
	else if(ARR == B)chan = 1;
	else if(ARR == C)chan = 2;	
//=====================================================================	
//	
//=====================================================================	
	//=========================================
	for(i=0;i<MAX_EVENTS;i++)
	{
	   if(ARR[i][0] != 0)ValidEvents++;												// Count the valid events in table 
	   else 		   break;	
	}
	if(ValidEvents > MAX_EVENTS)ValidEvents = MAX_EVENTS;
	ValidEvents--;																	// fix me
	//=========================================				
	for(i=0;i<=ValidEvents;i++)														// parse all valid events
	{
	  
	  if( (i < ValidEvents ) && (CurTimeInSec >= ARR[i][0]) && (CurTimeInSec < ARR[i+1][0]) )
	  {
		 if( ARR[i][2] == 0)														// event is not taken yet
		 {
			 ARR[i][2] = 1;															// mark as taken	
			 if(i>0) ARR[i-1][2] = 0;												// clear previous
			 else 	 ARR[ValidEvents][2] = 0;			 			 				
			 EventTake(chan,CurTimeInSec,ARR[i][1],ARR[i+1][0],ARR[i+1][1] );		// execute event	 			 
		 }		
	  }	
	  //======================= Handle last element in table ================================
	  if(i == ValidEvents)  														// 
	  {
		  if((CurTimeInSec >= ARR[ValidEvents][0]) )								
		  {
			  if( ARR[ValidEvents][2] == 0)											// if not executed already
			  {
				  ARR[ValidEvents][2] = 1;											// mark as taken
				  ARR[ValidEvents-1][2] = 0;
				  EventTake(chan,CurTimeInSec,ARR[ValidEvents][1],ARR[0][0],ARR[0][1] );  
			  }
		  }		  
	  }	
	}	
	//=========================================	
}	
//**************************************************************************************************
//		
//
//**************************************************************************************************
void EventTake(int Chan, int Ctime,int val,int NextTime,int NextVal)
{
  int hour,minute;		
  int TimeDiff; 
  int OutInc;
  
  hour = Ctime / 3600; 
  minute = (Ctime - (hour * 3600))/60;
  
  if(Ctime <= NextTime) TimeDiff = NextTime - Ctime;
  else TimeDiff = (86400 - Ctime) + NextTime;
 
  OutInc = ((NextVal - val) * FDIVISOR)/TimeDiff;
  
  if(Chan ==0)		PWMupdate(&PWMa, NextVal, OutInc);	
  else if(Chan == 1)PWMupdate(&PWMb, NextVal, OutInc);	
  else if(Chan == 2)PWMupdate(&PWMc, NextVal, OutInc);	

#ifdef X_DEBUG  
  printf("Channnel %d ->Event taken at %.2d:%.2d with pwm = %d -> Next in %d with pwm = %d \r\n",Chan, hour,minute,val,TimeDiff,NextVal); 		
  fflush(stdout);
#endif  
}
//**************************************************************************************************
//	 PWM service - must be called once every second				
//**************************************************************************************************
void PWMservice(PWMx *ch)
{
	int tmpPWM;
	
	if(ch->CurrentPWM_out == ch->TargetPWM_out)return;												// On target - do nothing	
	if(ch->CurrentPWM_out < ch->TargetPWM_out)														// less than target - go up
	{
	  ch->CurrentPWM_out += ch->PWMincrement;
	  if(ch->CurrentPWM_out > ch->TargetPWM_out) ch->CurrentPWM_out = ch->TargetPWM_out;			
	}	
		
	if(ch->CurrentPWM_out > ch->TargetPWM_out)
	{
	  ch->CurrentPWM_out += ch->PWMincrement;														// signed inc used - always add 
	  if(ch->CurrentPWM_out < ch->TargetPWM_out) ch->CurrentPWM_out = ch->TargetPWM_out;			
	}				
	
	tmpPWM = (ch->CurrentPWM_out / FDIVISOR);
	if(tmpPWM > 100)tmpPWM = 100;
	if(tmpPWM < 0)tmpPWM = 0;
	
	if(ch->LastPWMval == tmpPWM)return;														// same value - no need to update
	ch->LastPWMval = tmpPWM;																// new value - update PWM
			
	if(ch == &PWMa)		softPwmWrite (PWM0_PIN, tmpPWM); 	
	else if(ch == &PWMb)softPwmWrite (PWM1_PIN, tmpPWM); 
	else if(ch == &PWMc)softPwmWrite (PWM2_PIN, tmpPWM);
	
}
//**************************************************************************************************
//	 PWM update - must be called once every second				
//	 target range 0 - 100000 (translated to 0-100)
//	 increment - 0-100000 
//**************************************************************************************************
void PWMupdate(PWMx *ch, int target, int increment)
{
	
	ch->CurrentPWM_out = ch->TargetPWM_out;															// make sure that we are on target
	target *= FDIVISOR;
	if(target <= FDIVISOR * 100)ch->TargetPWM_out = target;
	else 				 ch->TargetPWM_out = FDIVISOR * 100;
	ch->PWMincrement = increment;	
}
//**************************************************************************************************
//
//**************************************************************************************************
void PWMinit(PWMx *ch)
{
	ch->PWMincrement = 0;
	ch->TargetPWM_out = 0;
	ch->CurrentPWM_out = 0;	
	ch->LastPWMval = 0;
}

//**********************************************************************************
//		Reads the init file and initializes all the data arrays
//**********************************************************************************
void InitSchedulerTables(void)
{
	char str[100];
    long n;	
	char var[16];
	int i;
		
	for(i=0;i<MAX_EVENTS;i++)
	{
	  sprintf(var,"A%d",i);			
	  n = ini_gets("lights", var, "----", str, sizearray(str), inifile);
	  if(strcmp(str,"----")==0)break;	
	  StrDecompose(str, 0, i);			
	}
	for(;i<MAX_EVENTS;i++){ A[i][0] = 0; A[i][1] = 0;  A[i][2] = 0;}
	
	for(i=0;i<MAX_EVENTS;i++)
	{
	  sprintf(var,"B%d",i);			
	  n = ini_gets("lights", var, "----", str, sizearray(str), inifile);
	  if(strcmp(str,"----")==0)break;	
	  StrDecompose(str, 1, i);			
	}
	for(;i<MAX_EVENTS;i++){ B[i][0] = 0; B[i][1] = 0;  B[i][2] = 0;}
	
	for(i=0;i<MAX_EVENTS;i++)
	{
	  sprintf(var,"C%d",i);			
	  n = ini_gets("lights", var, "----", str, sizearray(str), inifile);
	  if(strcmp(str,"----")==0)break;	
	  StrDecompose(str, 2, i);			
	}	
	for(;i<MAX_EVENTS;i++){ C[i][0] = 0; C[i][1] = 0;  C[i][2] = 0;}	
}
//**********************************************************************************
//					parse a time,pwm string
//**********************************************************************************
void StrDecompose(char *str, int Table, int entry)
{
  int c,i,pwm;
  char tmp1[10];	
  char tmp2[10];
  char tmp3[10];	
  		
   c = 0;
   i = 0;
   while(str[c] != ':' && str[c] != 0 )
   {   
		tmp1[i++] = str[c];		
		c++;
   }
   c++;
   tmp1[i] = 0;
   
   
   i = 0;
   while(str[c] !=',' && str[c] != 0 )
	{
		tmp2[i++] = str[c];		   
		c++;
	}
   c++;
   tmp2[i] = 0;
   
   i = 0;
   while(str[c] != 0
   )
	{		   
		tmp3[i++] = str[c];
		c++;	
	}
   c++;
   tmp3[i] = 0;
   //************************** convert to seconds ************** 
   c = (int)atol(tmp1);
   c = (c * 3600);
   i = (int)atol(tmp2);
   c += (i * 60);
   pwm = (int)atol(tmp3);
   
   if(Table == 0)
   {   
	  A[entry][0] = c + 1;   			// Timestamps must be non-zero - always add an offset of 1
	  A[entry][1] = pwm;
	  A[entry][2] = 0;
   }
	
   else if(Table == 1)
   {   
	  B[entry][0] = c + 1;   
	  B[entry][1] = pwm;
	  B[entry][2] = 0;
   }  
	
   else if(Table == 2)
   {   
	  C[entry][0] = c + 1;   
	  C[entry][1] = pwm;
	  C[entry][2] = 0;
   }  
}	
//**********************************************************************************
// 					End of aquarium controller
//**********************************************************************************

