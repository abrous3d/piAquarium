/*****************************************************************************
//					  Open Aquarium controller
//                          27/9/2013
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

* ****************************************************************************
*	  Build ->    gcc -Wall -o aqua miniIni.c th.c -lwiringPi -lpthread
* ****************************************************************************
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
//***************************************************************************
//***************************************************************************
#include <sys/select.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
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
#include "minIni.h"
#include "ModeFilter.h"
#include "AD7785_DRV.h"
#include "rtd.h"
#include "controller.h"
#include <wiringPi.h>
#include <maxdetect.h>
#include <softPwm.h>
#include "MoonPhase.h"
#include "database.h"
#include "Messaging.h"
#include <mysql/mysql.h>
#include <sys/inotify.h>
#include "aq_sockets.h"
//***************************************************************************
#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))
//***************************************************************************
#define		PWM0_PIN		1						// Light PWM channel 0
#define		PWM1_PIN		0						// Light PWM channel 1
#define		PWM2_PIN		2						// Light PWM channel 2

#define		SW_L0_PIN		3						// Light switch channel 0	
#define		SW_L1_PIN		7						// Light switch channel 1
#define		SW_L2_PIN		4						// Light switch channel 2

#define		SW_HEAT_PIN		5						// Heater	
#define		SW_FAN_PIN		6						// Cooling Fan
#define		SW_CO2_PIN		21						// CO2 Valve pin
#define		SW_BUZ_PIN		22						// Buzzer 

#define 	LED_ACT			27						// Alive led

#define 	ONE_WIRE_A		28						// One wire interface for the enviroment sensor AM2303 / DHT22 (1)
#define 	ONE_WIRE_B		29						// One wire interface for the enviroment sensor AM2303 / DHT22 (2)
	
//***************************************************************************
#define 	FDIVISOR			100000												// used to convert PWM to Fixed point		
//#define 	X_DEBUG				1
#define 	MAX_TIME_SLICES		10

//***************************************************************************
#define EVENT_SIZE  ( sizeof (struct inotify_event) )								//inotify related stuff
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
//***************************************************************************
struct  MAV pH_MavFltr,Temp_MavFltr, EnvTemp1_MavFltr,EnvHum1_MavFltr,EnvTemp2_MavFltr,EnvHum2_MavFltr ;			// moving average filters  
struct  MED_FILTER pH_MedFltr,Temp_MedFltr,EnvTemp1_MedFltr,EnvHum1_MedFltr,EnvTemp2_MedFltr,EnvHum2_MedFltr;       // Median filters

unsigned int RTD1_ADC_R1;														// ADC read value of RTD 1 @ REF 1   
unsigned int RTD1_ADC_R2;														// ADC read value of RTD 1 @ REF 2   

unsigned int PH1_ADC_pH4;														// ADC read value of pH 1 @ pH 4
unsigned int PH1_ADC_pH7;														// ADC read value of pH 1 @ pH 7   

struct H_COMPARATOR  pH_Controller;												// pH controller structure				
struct H_COMPARATOR  Temp_Controller;						
struct H_COMPARATOR  Fan_Controller;						

struct H_COMPARATOR  pH_Alarm;													// pH Alarm  structure				
struct H_COMPARATOR  Temp_Alarm;												// Temperature Alarm structure				
//***************************************************************************
void	InitSchedulerTables(void);
void	StrDecompose(char *str, int Table, int entry);
void	EventProcess(int ARR[][3],int CurTimeInSec);
void	TestLoop(void);
void	EventTake(int Chan,int Ctime,int val,int NextTime,int NextVal);
void	PWMinit(PWMx *ch);
void	PWMupdate(PWMx *ch, int target, int increment);
void	PWMservice(PWMx *ch);
void	BlinkActLed(void);
void	LowLevelInit(void);	
void	xTermMon(void);
void    ReadEnviromentalSensor(BYTE ch);
void 	ADcProcessInit(void);
void 	ADCa_Process(void);
void 	ADCb_Process(void);
void	InitRTDcalibrators(void);
void	InitpHcalibrators(void);
void	TestLoop(void);
void	ShutDownPi(void);
void	ExecProccessControl(void);
void	CheckForAlarms(void);
//***************************************************************************
//					scheduler.ini file 
//***************************************************************************
const char inipath[] = "/piAquarium/schedule/";								// Path of .ini file - used by Inotify
const char inifile[] = "/piAquarium/schedule/schedule.ini";
//**************************************************************************************************
//											Event tables
//	col0 = time in seconds since 00:00, col1 = target pwm, col2 = event state (0=pending, 1= taken)
//**************************************************************************************************
//========================================
int A[MAX_EVENTS][3]; 
int B[MAX_EVENTS][3];
int C[MAX_EVENTS][3];	

EVT Event_L0;
EVT Event_L1;
EVT Event_L2;

PWMx PWMa;
PWMx PWMb;
PWMx PWMc;

ENV_SENS  DHT22_a;							// Environmental sensor 1 
ENV_SENS  DHT22_b;							// Environmental sensor 2 

OUTP Outputs;								// 	Structure containing all the io states
REALMEAS Real;								// Structure containing all the real measurements
CNTRL Temperature;							// Temperature controller structure	
CNTRL pH;											
CNTRL Fan;
ALARMS alrm;

ADC	ADCa;
ADC	ADCb;

float Cal_RTD1_offs;						// RTD offset calibrator
float Cal_RTD1_gain;						// RTD gain calibrator

float Cal_pH1_offs;							// pH offset calibrator
float Cal_pH1_gain;							// pH gain calibrator

WORD	TermOnFlag;							// Used to turn on terminal output through software 
WORD	LightsOveride;						// if 1 lights settings will be overriden 
float	LightsScale;						// Scales the total light intensity 

BYTE  DataBaseUpdateDiv;					// Data base update divider
#define		DATABASEUPDATE			10		//	
//***************************************************************************
//							I/O MACROS
//***************************************************************************
void HEATER_ON(void)	{	digitalWrite(SW_HEAT_PIN, HIGH); Outputs.HeaterIsON = 1;}					
void HEATER_OFF(void)	{	digitalWrite(SW_HEAT_PIN, LOW); Outputs.HeaterIsON = 0;}					 

void FAN_ON(void)		{	digitalWrite(SW_FAN_PIN, HIGH); Outputs.FanIsON = 1; }					 
void FAN_OFF(void)		{   digitalWrite(SW_FAN_PIN, LOW); Outputs.FanIsON = 0;}					 

void CO2_ON(void)		{   digitalWrite(SW_CO2_PIN, HIGH); Outputs.CO2IsON = 1; }					 
void CO2_OFF(void)		{   digitalWrite(SW_CO2_PIN, LOW); Outputs.CO2IsON = 0; }	

void L0_ON(void)		{	digitalWrite (SW_L0_PIN, HIGH); Outputs.L0IsOn = 1; }
void L0_OFF(void)		{	digitalWrite (SW_L0_PIN, LOW); Outputs.L0IsOn = 0; }

void L1_ON(void)		{	digitalWrite (SW_L1_PIN, HIGH); Outputs.L1IsOn = 1; }
void L1_OFF(void)		{	digitalWrite (SW_L1_PIN, LOW); Outputs.L1IsOn = 0; }

void L2_ON(void)		{	digitalWrite (SW_L2_PIN, HIGH); Outputs.L2IsOn = 1; }
void L2_OFF(void)		{	digitalWrite (SW_L2_PIN, LOW); Outputs.L2IsOn = 0; }


#define 		IF_TERMINAL_ON()		if(TermOnFlag == 0xDEAD)

//**************************************************************************************************
//
//**************************************************************************************************
int main(void)
{
	int ClockTick,TimeSlice;
	time_t timer;
	struct tm * xtime;
	int fd,wd,i,ret;																// inotify 
	int npfd_Tsrv,npfd_Fsrv;		
	int newDayFlag = 0;

	struct pollfd ufds[1];   
	char buffer[BUF_LEN];															// used by inotify
	struct sigaction action;														// KILL and TERM signal handling
    	
	//***********************************************
IF_TERMINAL_ON()
	{
		printf("piAquarium controller \r\n"); 	
		printf("abrous3D 2014\r\n"); 	
	}

	//************************************************************
	//		Handle SIGTERM  signal 
	//************************************************************	
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = ShutDownPi;	
	sigaction(SIGTERM, &action, NULL);	
	
	//************************************************************	
//	DbCreate();																	// Create new database//
//	DbTableCreate();															// I have to handle this situation somehow
	
	DbConnect();
	DataBaseUpdateDiv = 0;
#ifdef 	TERMINAL_OUT
	TermOnFlag = 0xDEAD;														// Turn Terminal out on by default
#else
	TermOnFlag = 0;
#endif	
//=========== execute WiringPi spi library extension ==============
	int status = system("/root/wiringPi/gpio/gpio load spi");					// Call wiringPi spi 
//			FIX ME -> add some error checking here 
//=================================================================
	
	if( access( inifile, F_OK ) != -1 ) {
		IF_TERMINAL_ON()
		{
			printf("Config File ok..\r\n");		
		}
	} else {

	IF_TERMINAL_ON()
		{
			printf("Warning : No Init file\r\n");			
		}
		exit(1);
	}
	//=================== Init I/O ===========================
	LowLevelInit();		
	//===================  Filters init ======================
	median_filter_init(&pH_MedFltr);									
	median_filter_init(&Temp_MedFltr);
	median_filter_init(&EnvTemp1_MedFltr);
	median_filter_init(&EnvHum1_MedFltr);
	median_filter_init(&EnvTemp2_MedFltr);
	median_filter_init(&EnvHum2_MedFltr);
	//============ Inotify Init ==============================
	fd = inotify_init();												// inotify -> used to detect file changes
	if ( fd < 0 ) 	
	{
		IF_TERMINAL_ON() printf("inotify_init\r\n");
	}
	wd = inotify_add_watch( fd, inipath, IN_MODIFY);			///"piAquarium/schedule/" ..schedule.ini  FIX me - Why inotify cannot monitor only the .ini file ? 
	if (wd == -1)
    {
		IF_TERMINAL_ON() printf("Inotify :Couldn't add watch n");
    }
	ufds[0].fd = fd;
    ufds[0].events = POLLIN;

	//====== Initializes messaging FIFOs from/to server ======	

	Com_Fifo_init();

	//====== Read user config files and init tables ==========
ReadIniFile:

	InitSchedulerTables();												// Read user parameters from .ini file
	//======= Init RTD parameters ============================
	InitRTDcalibrators();												// Calculate gain & offset for RTD
	InitpHcalibrators();												// Calculate gain & offset for pH
	//======= Init controllers ===============================
	ComparatorInit(&pH_Controller,(pH.Set + pH.Hyst),(pH.Set - pH.Hyst));
	ComparatorInit(&Temp_Controller,(Temperature.Set + Temperature.Hyst),(Temperature.Set - Temperature.Hyst));
	ComparatorInit(&Fan_Controller,(Fan.Set + Fan.Hyst),(Fan.Set - Fan.Hyst));
	//======= Init Alarm comparators ========================
	ComparatorInit(&pH_Alarm,(pH.Set + pH.Alarm),(pH.Set - pH.Alarm));
	ComparatorInit(&Temp_Alarm,(Temperature.Set + Temperature.Alarm),(Temperature.Set - Temperature.Alarm));
	memset(&alrm,0,sizeof(alrm));									    // check this
   //=================  Peripheral init ======================
	if(ADCa.ADCact == 1) AD7785_CE0_init();
	if(ADCb.ADCact == 1) AD7785_CE1_init();
	ADcProcessInit();
   //==========================================================	
	TimeSlice = 0;
	IF_TERMINAL_ON()
	{
		printf("\033[2J\033[1;1H");												//Clear terminal screen
	}	

   // DataBaseCleanup();
   //================= Main controller loop =================== 			
	for(;;)
	{
		time(&timer);  													    // get current time; 
		xtime = localtime (&timer);									
		ClockTick = xtime->tm_hour * 3600;
		ClockTick += xtime->tm_min * 60;									// Calculate seconds since 00:00
		
		//============================================================================== 		
		//				 Daily housekeeping 
		//============================================================================== 		
		if(xtime->tm_hour == 23 && xtime->tm_min == 50)						// Every day at 23:59
		{
			newDayFlag = 0;
		}
		if(xtime->tm_hour == 23 && xtime->tm_min == 59 && newDayFlag == 0)	// Every day at 00:00	
		{
			newDayFlag = 1;													// Allow this to happen only once within this minute			
			DataBaseCleanup();												// cleanup database once every night
			usleep(500 * 1000);	
		}
		//============================================================================== 		
		if(ClockTick++ > 86400)												// This should never called 
		{
			ClockTick = 0;													// it is 00:00																
		}
		//============================================================================== 		
		//			
		//============================================================================== 		
		switch(TimeSlice)
		{
		//************************************
		case 0:				
			EventProcess(A,ClockTick);									// Process lights
			EventProcess(B,ClockTick);
			EventProcess(C,ClockTick);	
								
		break;
		//************************************
		case 1:
			PWMservice(&PWMa);
			PWMservice(&PWMb);
			PWMservice(&PWMc);			
		break;
		//************************************
		case 2:
			ReadEnviromentalSensor(0);
		break;
		//************************************
		case 3:
			//================================================================================
			//			Check if schedule.ini has been changed by the user or ui
			//================================================================================
				
			i = 0;
			BYTE iniModifiedFlag = 0;
			//************* Poll linux kernel for events **********	
			ret = poll(ufds, 1, 50);													// timeout of 50ms (is this necessary ?) FIX me
			if (ret < 0) 
			{
				IF_TERMINAL_ON() printf("poll failed: %s\n", strerror(errno));
			} else if (ret == 0) {														// Timeout with no events, move on.
				} else {																// Process the new event.
					struct inotify_event event;			
					int nbytes = read(fd, buffer, BUF_LEN);
					if ( nbytes < 0 ) 
					{
						IF_TERMINAL_ON() printf("inotify read fail" );
					}  
					while ( i < nbytes ) {
						struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
						if ( event->len ) 
						{
							if ( event->mask & IN_MODIFY)
							{
								iniModifiedFlag = 1;								
							}
						}
						i += EVENT_SIZE + event->len;
					}					
				}
				if(iniModifiedFlag)goto ReadIniFile;	
			break;
		//************************************	
			case 4:
				BlinkActLed();				
			break;
		//************************************	
			case 5:
				EventProcess(A,ClockTick);									// Process lights
				EventProcess(B,ClockTick);
				EventProcess(C,ClockTick);	
			break;
		//************************************
			case 6:
				PWMservice(&PWMa);
				PWMservice(&PWMb);
				PWMservice(&PWMc);						
			break;
		//************************************	
			case 7:
				ExecProccessControl();										// Execute pH,Temp & fan controllers
				CheckForAlarms();											// Compare measurements against alarm limits
			break;
		//************************************		
			case 8:	
				SockProcess();
				if(DataBaseUpdateDiv++ > DATABASEUPDATE)
				{
					DataBaseUpdateDiv = 0;
					DbWriteData();
				}
			break;
		//************************************	
			case 9:
				BlinkActLed();
				xTermMon();				
			break;
		//************************************	
			default:
				TimeSlice = 0;
		//************************************
		}		
		if(TimeSlice++ > MAX_TIME_SLICES - 1)TimeSlice = 0;
		// ============== End of TimeSlice Switch ===================		
		//===========================================================
		ADCa_Process();															// Process ADCa		
		ADCb_Process();															// Process ADCb
		// ======== Apply digital filtering on analog data ==========			// -------------------- FIX ME - ADC updates 1 channel / call (tot channels = 3)
		uint32_t	flt_tmp;		
		float Rfl;
		
		//===================== pH =====================================
		if(ADCa.SigA == 1)														// A valid pH measurement is ready - ADCa.SigA is set by ADCa_Process()
		{			
			flt_tmp = median_filter(&pH_MedFltr,ADCa.chA,pH_MEDIAN_LEN);		// Apply Median filter on pH input                	
			ADCa.chA_fltr = MAVFilter(&pH_MavFltr,flt_tmp,pH_MAV_LEN,1);        // Apply MAV filter on pH input                	          
			//============== Calculate pH value =========================	
			Rfl = (float)ADCa.chA_fltr;											// Get the filtered version of pH channel	
			Rfl = (Rfl - Cal_pH1_offs)*Cal_pH1_gain;							// apply offset & gain correction       
			Real.TankpH = Rfl/=100;  											// Rfl was x100 to keep resolution high	
			Real.TankpH = roundf(Real.TankpH * 100) / 100;						// Round to 2 decimal points

			ADCa.SigA = 0;
		}
		//===================== Temperature =============================
		if(ADCa.SigB == 1)														// A valid Temperature measurement is ready
		{
			flt_tmp = median_filter(&Temp_MedFltr,ADCa.chB,TEMP_MEDIAN_LEN);	// Apply Median filter on Temperature input                	
			ADCa.chB_fltr = MAVFilter(&Temp_MavFltr,flt_tmp,TEMP_MAV_LEN,1);    // Apply MAV filter on Temperature input                	          
			
			//============== Calculate RTD temperature ==================				
			Rfl = (float)ADCa.chB_fltr;											// Get the filtered version of temperature channel	
			Rfl = (Rfl - Cal_RTD1_offs)*Cal_RTD1_gain;							// apply offset & gain correction
			Rfl/=100;															// Rfl was x100 to keep resolution high
			Real.TankTemp = roundf( T_rtd(Rfl) * 100) / 100 ;						   				    // Linearize pt100									
			ADCa.SigB = 0;
		}		
		//====================================================
		Com_Fifo_update();														// Respond to commands from server
		//====================================================
		//delay(100);
		usleep(100 * 1000);														// Controller loop ~10/sec
	}	
	return 0;
}
//================================================================================
//						  Process controllers
//				  Simple bang-bang hysteretic control
//=================================================================================		
void ExecProccessControl(void)
{
    //============== Temperature Controller =====================
	if(ADCa.ADCact == 1 && Temperature.ControlActive == 1)
	{
		if(Comparator(&Temp_Controller,Real.TankTemp * 1000)) HEATER_OFF();				// Compare Real tank temperature with tSet High & tSet Low
		else												  HEATER_ON();
	}
	else
	{
		HEATER_OFF();
	}
	//============== pH Controller =====================
	if(ADCa.ADCact == 1 && pH.ControlActive == 1)
	{
		if(Comparator(&pH_Controller,Real.TankpH * 1000))	  CO2_ON();
		else												  CO2_OFF();	
	}
	else
	{
		CO2_OFF();
	}
	//============== Fan Controller =====================
	if(ADCa.ADCact == 1 && Fan.ControlActive == 1)
	{
		if(Comparator(&Fan_Controller,Real.TankTemp * 1000))  FAN_ON();
		else												  FAN_OFF();
	}	
	else
	{
		FAN_OFF();	
	}
}
//=================================================================================				
//						  Alarm controller
//				Compare measurements against alarm limits
//=================================================================================				
void CheckForAlarms(void)
{
	BYTE TmpComp;
	//============ Temperature alarm ==============
	if(ADCa.ADCact == 1 && Temperature.AlarmActive == 1)
	{
		TmpComp = Comparator3S(&Temp_Alarm,Real.TankTemp * 1000);
		if(TmpComp == 0)														// In range
		{
			alrm.TempHigh = 0;
			alrm.TempLow = 0;
		}
		else if(TmpComp == 1)													// Out of range Low
		{
			alrm.TempHigh = 0;
			alrm.TempLow = 1;
		}
		else if(TmpComp == 2)													// Out of range High
		{
			alrm.TempHigh = 1;
			alrm.TempLow = 0;
		}
	}
	//=================== pH alarm ==============
	if(ADCa.ADCact == 1 && pH.AlarmActive == 1)
	{
		TmpComp = Comparator3S(&pH_Alarm,Real.TankpH * 1000);
		if(TmpComp == 0)								
		{
			alrm.pH_High = 0;
			alrm.pH_Low = 0;
		}
		else if(TmpComp == 1)
		{
			alrm.pH_High = 0;
			alrm.pH_Low = 1;
		}
		else if(TmpComp == 2)
		{
			alrm.pH_High = 1;
			alrm.pH_Low = 0;
		}
	}
}
//**************************************************************************************************
//
//**************************************************************************************************
enum{ ST_ADA_0, ST_ADA_1};
enum{ ST_ADB_0, ST_ADB_1, ST_ADB_2};

static int ADRD_A_State;
static int ADRD_B_State;
//**************************************************************************************************
//							Initializes the ADC procces 
//	
//**************************************************************************************************
void 	ADcProcessInit(void)
{
	if(ADCa.ADCact == 1)
	{
		AD_mux_to_pH_on_CE0();
		ADRD_A_State = ST_ADA_0;
	}
	if(ADCb.ADCact == 1)
	{
		AD_mux_to_AIN1_on_CE1();
		ADRD_B_State = ST_ADB_0;
	}				
}
//**************************************************************************************************
//			Process the ADC0 (if it is enabled) in a task like round-robin loop  
//**************************************************************************************************
void 	ADCa_Process(void)
{
	if(ADCa.ADCact == 1){									 				// check if ADC0 exists												
		if(!(AD7785_StatusRead(0) & 0x80))										// Check busy state
		{	
			switch(ADRD_A_State)												// walk through ADC state machine	
			{
				case ST_ADA_0:
					ADCa.chA = AD7785_DataRead(0); 								// read pH value from AIN1
					AD_mux_to_pH_on_CE0();
					AD_mux_to_RTD_on_CE0();										// switch to RTD (AIN2) for next cycle
					ADRD_A_State = ST_ADA_1;	
					ADCa.SigA = 1;
				break;
		
				case ST_ADA_1:
					ADCa.chB = AD7785_DataRead(0);								// read RTD value from AIN2
					AD_mux_to_pH_on_CE0();										// switch to pH for the next cycle	
					ADRD_A_State = ST_ADA_0;
					ADCa.SigB = 1;					
				break;
					
				default:
					ADRD_A_State = ST_ADA_0;
			}		 			
		}
		return;		
	}
	else
	{
		ADCa.chA = 0xFFFFFFFF;													// Invalidate ADC variables if not enabled
		ADCa.chB = 0xFFFFFFFF;		
	}	
}
//**************************************************************************************************
//			Process the ADC1 (if it is enabled) in a task like round-robin loop  
//**************************************************************************************************
void 	ADCb_Process(void)
{
	if(ADCb.ADCact == 1){														// Skip if not active
		if(!(AD7785_StatusRead(1) & 0x80))   // ?????? FIX
		{	
			switch(ADRD_B_State)
			{
				case ST_ADB_0:
					ADCb.chA = AD7785_DataRead(1); 								// read pH value
					AD_mux_to_AIN1_on_CE1();
					ADRD_B_State = ST_ADB_1;	
				break;
		
				case ST_ADB_1:
					ADCb.chB = AD7785_DataRead(1);								// read RTD value
					AD_mux_to_AIN2_on_CE1();
					ADRD_B_State = ST_ADB_2;
				break;
					
				case ST_ADB_2:
					ADCb.chC = AD7785_DataRead(1);								// read RTD value
					AD_mux_to_AIN3_on_CE1();
					ADRD_B_State = ST_ADB_0;
				break;	
					
				default:
				ADRD_B_State = ST_ADA_0;
			}		 			
		}
		return;	
	}
	ADCb.chA = 0xFFFFFFFF;														// Invalidate ADC variables if not enabled
	ADCb.chB = 0xFFFFFFFF;	
	ADCb.chC = 0xFFFFFFFF;	
}
//**************************************************************************************************
//					Calculate RTD sensor offset and gain values
//**************************************************************************************************
void	InitRTDcalibrators(void)
{
	Cal_RTD1_offs = (float)((float)((float)(RTD1_ADC_R2 - RTD1_ADC_R1)/R2_MINUS_R1)*(-R1_REF)) + (float)(RTD1_ADC_R1);
	Cal_RTD1_gain = (float)((float)R2_REF /(RTD1_ADC_R2 - Cal_RTD1_offs));   
}
//**************************************************************************************************
//					Calculate pH sensor offset and gain values
//**************************************************************************************************
void	InitpHcalibrators(void)
{
	Cal_pH1_offs = (float)((float)((float)(PH1_ADC_pH4 - PH1_ADC_pH7)/pH4_MINUS_pH7)*(-pH7_REF)) + (float)(PH1_ADC_pH7);
	Cal_pH1_gain = (float)((float)pH4_REF /(PH1_ADC_pH4 - Cal_pH1_offs));   
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
 
  OutInc = ((NextVal - val))/TimeDiff;
  
  if(Chan ==0)
  {
	  
	  PWMa.TargetPWM_out = val;
	  PWMupdate(&PWMa, NextVal, OutInc);	
	  Event_L0.LastE_hour = hour;
	  Event_L0.LastE_min = minute;
	  Event_L0.LastE_val = val;	
	  Event_L0.NextEvent = TimeDiff;
	  Event_L0.NextE_val = NextVal;	
  }	
  else if(Chan == 1)
  {
	  PWMb.TargetPWM_out = val;
	  PWMupdate(&PWMb, NextVal, OutInc);	
	  Event_L1.LastE_hour = hour;
	  Event_L1.LastE_min = minute;
	  Event_L1.LastE_val = val;	
	  Event_L1.NextEvent = TimeDiff;
	  Event_L1.NextE_val = NextVal;	
  }
  else if(Chan == 2)
  {
	  PWMc.TargetPWM_out = val;
	  PWMupdate(&PWMc, NextVal, OutInc);	
	  Event_L2.LastE_hour = hour;
	  Event_L2.LastE_min = minute;
	  Event_L2.LastE_val = val;	
	  Event_L2.NextEvent = TimeDiff;
	  Event_L2.NextE_val = NextVal;	
  }
}
//**************************************************************************************************
//	 PWM service - must be called once every second				
//**************************************************************************************************
void PWMservice(PWMx *ch)
{
	int tmpPWM;
	float tmpf;
	
	if(LightsOveride == 1)																	// manual overide
	{
		tmpPWM = ch->ManualValue;															// Use the manual value		
		goto DirectWritetoPWM;		
	}

	//if(ch->CurrentPWM_out == ch->TargetPWM_out)return;	
	if(ch->CurrentPWM_out < ch->TargetPWM_out)												// less than target - go up
	{
	  ch->CurrentPWM_out += (ch->PWMincrement / 2);												
	  if(ch->CurrentPWM_out > ch->TargetPWM_out) ch->CurrentPWM_out = ch->TargetPWM_out;			
	}	
		
	if(ch->CurrentPWM_out > ch->TargetPWM_out)												
	{
	  ch->CurrentPWM_out += (ch->PWMincrement / 2);												// signed inc used - always add 
	  if(ch->CurrentPWM_out < ch->TargetPWM_out) ch->CurrentPWM_out = ch->TargetPWM_out;			
	}				
	
	tmpPWM = ch->CurrentPWM_out;
	if(tmpPWM > (100 * FDIVISOR))tmpPWM = (100 * FDIVISOR);
	if(tmpPWM < 0)tmpPWM = 0;
	
	//if(ch->LastPWMval == tmpPWM)return;													// same value - no need to update 
	ch->LastPWMval =tmpPWM ;	
																							// new value - update PWM
UpdateOutputs:			
	
	tmpf = (LightsScale / 100) * (float)tmpPWM;	
	tmpPWM = round(tmpf/FDIVISOR);
	//tmpPWM /= FDIVISOR;																		// scale to 0-100%

DirectWritetoPWM:

	if(ch == &PWMa)
	{
		Outputs.L0intensity = tmpPWM;														

#ifdef PWM_USE_HARDWARE
		tmpPWM = (tmpPWM * 1024)/100;														// convert to 0-1024 range

		if(Outputs.L0polarity == 1)pwmWrite (PWM0_PIN, tmpPWM) ;							// Update PWM L0
		else					   pwmWrite (PWM0_PIN, 1024 - tmpPWM) ;
#else
		if(Outputs.L0polarity == 1)	softPwmWrite (PWM0_PIN, tmpPWM); 					    // Update PWM L0
		else						softPwmWrite (PWM0_PIN, (100 - tmpPWM));
#endif
		
		if(tmpPWM == 0) L0_OFF();															// Switch Lights OFF if PWM is 0 
		else			L0_ON();															// Switch Lights ON if PWM is > 0 

	}
	else if(ch == &PWMb)
	{
		if(Outputs.L1polarity == 1)	softPwmWrite (PWM1_PIN, tmpPWM); 					    // Update PWM L0
		else						softPwmWrite (PWM1_PIN, (100 - tmpPWM));	 		
		Outputs.L1intensity = tmpPWM;
	
		if(tmpPWM == 0) L1_OFF();															// Switch Lights OFF if PWM is 0 
		else			L1_ON();															// Switch Lights ON if PWM is > 0 	

	}
	else if(ch == &PWMc)
	{
		if(Outputs.L2polarity == 1)	softPwmWrite (PWM2_PIN, tmpPWM); 					    // Update PWM L0
		else						softPwmWrite (PWM2_PIN, (100 - tmpPWM));
		Outputs.L2intensity = tmpPWM; 

		if(tmpPWM == 0) L2_OFF();															// Switch Lights OFF if PWM is 0 
		else			L2_ON();															// Switch Lights ON if PWM is > 0 
	}	
}
//**************************************************************************************************
//	 PWM update - must be called once every second				
//	 target range 0 - 100000 (translated to 0-100)
//	 increment - 0-100000 
//**************************************************************************************************
void PWMupdate(PWMx *ch, int target, int increment)
{
	
	ch->CurrentPWM_out = ch->TargetPWM_out;															// make sure that we are on target	
	if(target <= FDIVISOR * 100)ch->TargetPWM_out = target;
	else 				        ch->TargetPWM_out = (FDIVISOR * 100);
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
//**********************************************************************************
//		Read the .Ini file and initialize all the data arrays
//**********************************************************************************
//**********************************************************************************
void InitSchedulerTables(void)
{
	char str[100];
    long n;	
	char var[16];
	int i;
	//===================  Initialize time sceduler tables ============	
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
	
	//=================  End of time tables initialization ===========
	
	if( ini_getbool("lights", "Overide", 0, inifile)) LightsOveride = 1;	// Switch lights to manual
	else										  	  LightsOveride = 0;	// Set lights to AUTO
	
	PWMa.ManualValue = ini_getl("lights", "ManA",100, inifile);				// Get L0 manual value	
	if(PWMa.ManualValue < 0)PWMa.ManualValue = 0;
	if(PWMa.ManualValue > 100)PWMa.ManualValue = 100;

	PWMb.ManualValue = ini_getl("lights", "ManB",100, inifile);				// Get L1 manual value	
	if(PWMb.ManualValue < 0)PWMb.ManualValue = 0;
	if(PWMb.ManualValue > 100)PWMb.ManualValue = 100;


	PWMc.ManualValue = ini_getl("lights", "ManC",100, inifile);				// Get L2 manual value	
	if(PWMc.ManualValue < 0)PWMc.ManualValue = 0;
	if(PWMc.ManualValue > 100)PWMc.ManualValue = 100;


	LightsScale = ini_getf("lights", "Scale",100.0, inifile);
	if(LightsScale < 0)		LightsScale = 0;
	if(LightsScale > 100)	LightsScale = 100;


	//=================== Read PWM polarity
	if( ini_getbool("lights", "PolA", 1, inifile)) 
	{
		Outputs.L0polarity = 1;									// Normal PWM polarity
	}
	else 
	{
		Outputs.L0polarity = 0;									// Reverse PWM polarity
	}

	if( ini_getbool("lights", "PolB", 1, inifile)) 
	{
		Outputs.L1polarity = 1;									// Normal PWM polarity
	}
	else 
	{
		Outputs.L1polarity = 0;									// Reverse PWM polarity
	}

		if( ini_getbool("lights", "PolC", 1, inifile)) 
	{
		Outputs.L2polarity = 1;									// Normal PWM polarity
	}
	else 
	{
		Outputs.L2polarity = 0;									// Reverse PWM polarity
	}

	// =============	Initialize ADC 0 & 1 ====================
	if( ini_getbool("system", "ADConCE0", 0, inifile)) 
	{
		 ADCa.ADCact = 1;				// Enable ADC 0
	}
	else
	{
		ADCa.ADCact = 0;				// Disable ADC 0
	}

	if( ini_getbool("system", "ADConCE1", 0, inifile)) 
	{		 
		 ADCb.ADCact = 1;				// Disable ADC 1
	}
	else
	{
		ADCb.ADCact = 0;				// Disable ADC 1
	}
	//=========== init temperature related structures ========
	float tmp;

	if( ini_getbool("temperature", "On", 0, inifile)) 
	{	
		Temperature.ControlActive = 1;

		tmp = ini_getf("temperature", "Set",25.0, inifile);
		if(tmp < USER_TEMP_SET_MIN)tmp = USER_TEMP_SET_MIN;
		if(tmp > USER_TEMP_SET_MAX)tmp = USER_TEMP_SET_MAX;
		Temperature.Set = (int)(tmp * 1000);	
		
		tmp = ini_getf("temperature", "HeatHysteresis",2.0, inifile);		
		if(tmp < USER_TEMP_MIN_HYSTERESIS)Temperature.Hyst = USER_TEMP_MIN_HYSTERESIS * 1000;
		else							  Temperature.Hyst = (int)(tmp * 1000);	

		if( ini_getbool("temperature", "AlarmActive", 0, inifile)) 
		{		 
			Temperature.AlarmActive = 1;										// Enable Alarms for temperature
		}
		else
		{
			Temperature.AlarmActive = 0;
		}

		tmp = ini_getf("temperature", "Alarm",4.0, inifile);		
		Temperature.Alarm = (int)(tmp * 1000);	

		Fan.ControlActive = 1;
		tmp = ini_getf("temperature", "Fan",28.0, inifile);				
		Fan.Set = (int)(tmp * 1000);		


		tmp = ini_getf("temperature", "FanHysteresis",2.0, inifile);		
		if(tmp < USER_FAN_MIN_HYSTERESIS)Fan.Hyst = USER_FAN_MIN_HYSTERESIS * 1000;
		else							 Fan.Hyst = (int)(tmp * 1000);			
		
	}
	else
	{	
		Temperature.ControlActive = 0;
		Fan.ControlActive = 0;
	}

	//=============== init pH related structures ===============
	

	if( ini_getbool("pH", "On", 0, inifile)) 
	{	
		pH.ControlActive = 1;

		tmp = ini_getf("pH", "Set",7.0, inifile);
		if(tmp < USER_PH_SET_MIN)tmp = USER_PH_SET_MIN;
		if(tmp > USER_PH_SET_MAX)tmp = USER_PH_SET_MAX;

		pH.Set = (int)(tmp * 1000);	
		
		tmp = ini_getf("pH", "Hysteresis",0.1, inifile);		
		if(tmp < USER_PH_MIN_HYSTERESIS)pH.Hyst = USER_PH_MIN_HYSTERESIS * 1000;
		else							pH.Hyst = (int)(tmp * 1000);			

		if( ini_getbool("pH", "AlarmActive", 0, inifile)) 
		{		 
			pH.AlarmActive = 1;										// Enable Alarms for temperature
		}
		else
		{
			pH.AlarmActive = 0;
		}

		tmp = ini_getf("pH", "Alarm",0.5, inifile);		
		pH.Alarm = (int)(tmp * 1000);	
	}
	else
	{	
		pH.ControlActive = 0;
	}
	//============== Initialize System related variables =========

	RTD1_ADC_R1 = ini_getf("system", "RTD1_ADC_R1",1.0, inifile);					// FIX ME Default values
	RTD1_ADC_R2 = ini_getf("system", "RTD1_ADC_R2",1.0, inifile);	

	PH1_ADC_pH7 = ini_getf("system", "PH1_ADC_PH7",1.0, inifile);					// FIX ME Default values
	PH1_ADC_pH4 = ini_getf("system", "PH1_ADC_PH4",1.0, inifile);

	if( ini_getbool("system", "EnvSens0", 0, inifile))
	{
		DHT22_a.EnvSensActive = 1;
	}
	else
	{
		DHT22_a.EnvSensActive = 0;
	}

	if( ini_getbool("system", "EnvSens1", 0, inifile))
	{
		DHT22_b.EnvSensActive = 1;
	}
	else
	{
		DHT22_b.EnvSensActive = 0;
	}
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
	  A[entry][1] = (pwm * FDIVISOR);
	  A[entry][2] = 0;
   }
	
   else if(Table == 1)
   {   
	  B[entry][0] = c + 1;   
	  B[entry][1] = (pwm * FDIVISOR);
	  B[entry][2] = 0;
   }  
	
   else if(Table == 2)
   {   
	  C[entry][0] = c + 1;   
	  C[entry][1] = (pwm * FDIVISOR);
	  C[entry][2] = 0;
   }  
}	
//**********************************************************************************
//								Blink ACT LED
//**********************************************************************************
void BlinkActLed(void)
{
	static int blnk;

	if(blnk == 0)
	{
		blnk = 1;
		digitalWrite (LED_ACT, HIGH) ;	
	}
	else 
	{
		blnk = 0;
		digitalWrite (LED_ACT, LOW) ;	
	}
}
//**********************************************************************************
//								Low level Init
//**********************************************************************************
void LowLevelInit(void)	
{
//===================  Init WiringPi =====================
	wiringPiSetup() ;
    piHiPri(85) ;												// Set wiring to elevated priority (100 is the highest)

	//=================== Setup Pins =========================	
	pinMode (SW_BUZ_PIN, OUTPUT) ;								// Initialize buzzer pin
	pullUpDnControl (SW_BUZ_PIN, PUD_UP);
	digitalWrite (SW_BUZ_PIN, LOW) ;
	
	pinMode (LED_ACT, OUTPUT) ;									// Initialize ACT led	
	pullUpDnControl (LED_ACT, PUD_UP);
	digitalWrite (LED_ACT, HIGH) ;
	//*****************************************
#ifdef PWM_USE_HARDWARE
	pullUpDnControl (PWM0_PIN, PUD_UP);
	pinMode (PWM0_PIN, PWM_OUTPUT) ;
	pwmSetClock (32) ;
	pwmSetMode(PWM_MODE_MS);

#else
	softPwmCreate (PWM0_PIN, 0, 100) ;
#endif

	pullUpDnControl (PWM0_PIN, PUD_UP);
	pullUpDnControl (PWM1_PIN, PUD_UP);
	pullUpDnControl (PWM2_PIN, PUD_UP);

	softPwmCreate (PWM1_PIN, 0, 100) ;
    softPwmCreate (PWM2_PIN, 0, 100) ;
	pinMode (SW_L0_PIN, OUTPUT) ;
	pullUpDnControl (SW_L0_PIN, PUD_DOWN);
	digitalWrite (SW_L0_PIN, LOW) ;
	Outputs.L0IsOn = 0;
	Outputs.L0intensity = 0;

	pinMode (SW_L1_PIN, OUTPUT) ;
	pullUpDnControl (SW_L1_PIN, PUD_DOWN);
	digitalWrite (SW_L1_PIN, LOW) ;
	Outputs.L1IsOn = 0;
	Outputs.L1intensity = 0;

	pinMode (SW_L2_PIN, OUTPUT) ;
	pullUpDnControl (SW_L2_PIN, PUD_DOWN);
	digitalWrite (SW_L2_PIN, LOW) ;
	Outputs.L2IsOn = 0;
	Outputs.L2intensity = 0;

	pinMode (SW_HEAT_PIN, OUTPUT) ;
	pullUpDnControl (SW_HEAT_PIN, PUD_DOWN);
	digitalWrite (SW_HEAT_PIN, LOW) ;
	Outputs.HeaterIsON = 0;

	pinMode (SW_FAN_PIN, OUTPUT) ;
	pullUpDnControl (SW_FAN_PIN, PUD_DOWN);
	digitalWrite (SW_FAN_PIN, LOW) ;
	Outputs.FanIsON = 0;

	pinMode (SW_CO2_PIN, OUTPUT) ;
	pullUpDnControl (SW_CO2_PIN, PUD_DOWN);
	digitalWrite (SW_CO2_PIN, LOW) ;
	Outputs.CO2IsON = 0;	
#ifndef PWM_USE_HARDWARE
	PWMinit(&PWMa);
#endif
	PWMinit(&PWMb);
	PWMinit(&PWMc);
}
//**********************************************************************************
// 					End of aquarium controller
//**********************************************************************************
#define		DLYX	800*1000

void	TestLoop(void)
{
	softPwmWrite (PWM0_PIN, 40);
	softPwmWrite (PWM1_PIN, 50);
	softPwmWrite (PWM2_PIN, 60);

	for(;;)
	{
		BlinkActLed();
		L0_ON();
		usleep(DLYX);
		BlinkActLed();
		L1_ON();
		usleep(DLYX);
		BlinkActLed();
		L2_ON();
		usleep(DLYX);
		BlinkActLed();
		HEATER_ON();
		usleep(DLYX);
		BlinkActLed();
		FAN_ON();
		usleep(DLYX);
		BlinkActLed();
		CO2_ON();
		usleep(DLYX);
		BlinkActLed();
		CO2_OFF();
		usleep(DLYX);
		BlinkActLed();
		FAN_OFF();
		usleep(DLYX);
		BlinkActLed();
		HEATER_OFF();
		usleep(DLYX);
		BlinkActLed();
		L2_OFF();
		usleep(DLYX);
		BlinkActLed();
		L1_OFF();
		usleep(DLYX);
		BlinkActLed();
		L0_OFF();
		usleep(DLYX);
	}
}

//=================================================================================				
//
//=================================================================================				
void xTermMon(void)
{
	IF_TERMINAL_ON()
	{
		printf("\033[10;2H***************************************************\n");
	
		printf("\033[11;2H*");
		printf("\033[11;4H ADcha = %d  ",ADCa.chA);
		printf("\033[11;24H*");
		printf("\033[11;27H ADchaF = %d ", ADCa.chA_fltr);
		printf("\033[11;52H*\n");

		printf("\033[12;2H*");
		printf("\033[12;4H ADchb = %d  ",ADCa.chB);
		printf("\033[12;24H*");
		printf("\033[12;27H ADchbF = %d ", ADCa.chB_fltr);
		printf("\033[12;52H*\n");

		printf("\033[13;2H*");
		printf("\033[13;4H pH = %0.2f  ",Real.TankpH);
		printf("\033[13;24H*");
		printf("\033[13;27H Temp = %0.2fC", Real.TankTemp);
		printf("\033[13;52H*\n");

		printf("\033[14;2H***************************************************\n");


		printf("\033[15;2H*");
		if(Outputs.L0IsOn == 1){
			printf("\033[15;4H L0:ON @%d",Outputs.L0intensity);
		}else
		{
			printf("\033[15;4H L0:OFF ");
		}
		printf("\033[15;52H*\n");
		
		printf("\033[16;2H*");
		printf("\033[16;4H Last Event at:%02d:%02d with PWM = %03d ",Event_L0.LastE_hour,Event_L0.LastE_min,Event_L0.LastE_val/ FDIVISOR); 
		printf("\033[16;52H*\n");

		printf("\033[17;2H*");
		printf("\033[17;4H Next in %dSec with PWM = %03d ",Event_L0.NextEvent,Event_L0.NextE_val/ FDIVISOR);
		printf("\033[17;52H*\n");

		printf("\033[18;2H***************************************************\n");

		if(Outputs.L1IsOn == 1){
			printf("\033[19;4H L1:ON @%d",Outputs.L1intensity);
		}else
		{
			printf("\033[19;4H L1:OFF ");
		}
		printf("\033[19;52H*\n");

		printf("\033[20;2H*");
		printf("\033[20;4H Last Event at:%02d:%02d with PWM = %03d ",Event_L1.LastE_hour,Event_L1.LastE_min,Event_L1.LastE_val/ FDIVISOR); 
		printf("\033[20;52H*\n");

		printf("\033[21;2H*");
		printf("\033[21;4H Next in %dSec with PWM = %03d ",Event_L1.NextEvent,Event_L1.NextE_val/ FDIVISOR);
		printf("\033[21;52H*\n");

		printf("\033[22;2H***************************************************\n");


		if(Outputs.L2IsOn == 1){
			printf("\033[23;4H L2:ON @%d",Outputs.L2intensity);
		}else
		{
			printf("\033[23;4H L2:OFF ");
		}
		printf("\033[23;52H*\n");

		printf("\033[24;2H*");
		printf("\033[24;4H Last Event at:%02d:%02d with PWM = %03d ",Event_L2.LastE_hour,Event_L2.LastE_min,Event_L2.LastE_val/ FDIVISOR); 
		printf("\033[24;52H*\n");

		printf("\033[25;2H*");
		printf("\033[25;4H Next in %dSec with PWM = %03d ",Event_L2.NextEvent,Event_L2.NextE_val/ FDIVISOR);
		printf("\033[25;52H*\n");

		printf("\033[26;2H***************************************************\n");


		printf("\033[27;2H*");
		if(Outputs.HeaterIsON == 1)
		{
			printf("\033[27;4H HEATER:ON ");	
		}
		else
		{
			printf("\033[27;4H HEATER:OFF");	
		}
		printf("\033[27;18H*");

		if(Outputs.FanIsON == 1)
		{
			printf("\033[27;20H FAN:ON ");	
		}
		else
		{
			printf("\033[27;20H FAN:OFF");		
		}
		printf("\033[27;35H*");

		if(Outputs.CO2IsON == 1)
		{
			printf("\033[27;37H CO2:ON ");	
		}
		else
		{
			printf("\033[27;37H CO2:OFF");	
		}
		printf("\033[27;52H*\n");

		printf("\033[28;2H***************************************************\n");

		printf("\033[29;2H*");
		printf("\033[29;4H Ext Temp = %0.1f  ",(float)((float)DHT22_a.RowTemperature / 10));
		printf("\033[29;24H*");
		printf("\033[29;27H Ext Humidity = %0.1f ", (float)((float)DHT22_a.RowHumidity / 10));
		printf("\033[29;52H*\n");
		printf("\033[30;2H***************************************************\n");
	}
}

//=================================================================================				
//		Scan the installed DHT22 devices one at a time,once every 2 seconds
//=================================================================================				
void ReadEnviromentalSensor(BYTE ch)
{
	static int tog,softDiv;
	int status,temp,rh;   
	float flt_tmp;

	if(softDiv++ < 3)return;
	softDiv = 0;
	if(tog == 1)
	{
		tog = 0;
		ch = 0;
	}
	else
	{
		tog = 1;
		ch = 1;
	}

	if(ch == 0 && DHT22_a.EnvSensActive)
	{
		status = readRHT03(ONE_WIRE_A, &temp, &rh);
		if(status)
		{
			DHT22_a.RowTemperature = temp;
			DHT22_a.RowHumidity = rh;

			flt_tmp = median_filter(&EnvTemp1_MedFltr,((float)((float)temp)),ENV_TEMP1_MED_LEN);		// Apply Median filter on Env sensor1 input                	
			Real.EnvTemp1 = MAVFilter(&EnvTemp1_MavFltr,flt_tmp,ENV_TEMP1_MAV_LEN,1);					// Apply MAV filter on Env sensor 1 input                	          
			Real.EnvTemp1 /= 10;

			flt_tmp = median_filter(&EnvHum1_MedFltr,((float)((float)rh)) ,ENV_HUM1_MED_LEN);			// Apply Median filter on Env sensor1 input                	
			Real.EnvHumidity1 = MAVFilter(&EnvHum1_MavFltr,flt_tmp,ENV_HUM1_MAV_LEN,1);					// Apply MAV filter on Env sensor 1 input                	          			
			Real.EnvHumidity1 /= 10;

		}
	}
	if(ch == 1 && DHT22_b.EnvSensActive)
	{
		status = readRHT03 (ONE_WIRE_B, &temp, &rh);
		if(status)
		{
			DHT22_b.RowTemperature = temp;
			DHT22_b.RowHumidity = rh;
			
			flt_tmp = median_filter(&EnvTemp2_MedFltr,((float)((float)temp)),ENV_TEMP2_MED_LEN);		// Apply Median filter on Env sensor1 input                	
			Real.EnvTemp2 = MAVFilter(&EnvTemp2_MavFltr,flt_tmp,ENV_TEMP2_MAV_LEN,1);					// Apply MAV filter on Env sensor 1 input                	          
			Real.EnvTemp2 /= 10;

			flt_tmp = median_filter(&EnvHum2_MedFltr,((float)((float)rh)) ,ENV_HUM1_MED_LEN);			// Apply Median filter on Env sensor1 input                	
			Real.EnvHumidity2 = MAVFilter(&EnvHum2_MavFltr,flt_tmp,ENV_HUM2_MAV_LEN,1);					// Apply MAV filter on Env sensor 1 input                	          			
			Real.EnvHumidity2 /= 10;

		}
	}
}
//==================================================================================================
//			SIGTERM handle - close all open files and exit gracefully
//==================================================================================================
void ShutDownPi(void)
{
	DbDisconnect();																	// DataBase Close
	Com_Fifo_kill();
	
}
//=================================================================================				