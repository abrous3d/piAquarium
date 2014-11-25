//================================================================================
//							  defines.h
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

//							 Types				
//================================================================================
//#define uint32_t	unsigned int
//#define uint16_t	unsigned short
//#define uint8_t		unsigned char
#include <stdint.h>
#define DWORD		unsigned int
#define WORD		unsigned short
#define BYTE		unsigned char
#define FLAG		unsigned char
//================================================================================
typedef struct pwmchannel
{	
	int CurrentPWM_out;
	int TargetPWM_out;
	int PWMincrement;	
	int LastPWMval;
	int ManualValue;
}	PWMx;
//****************************************************************************
typedef struct Control
{
	int ControlActive;															// If it is zero control is deactivated
	int MonitorActive;															// If it is zero there is no monitoring 	
	int Set;																	// Set value of the controller multiplied by 1000
	int Hyst;																	// hysteresis (* 1000)
	int AlarmActive;
	int Alarm;																	// Max allowed deviation before alarm
	
}  CNTRL;
// ***************************************************************************
// Enviromental sensor DHT22/AM2302 interface structure
// ***************************************************************************
typedef struct DHT22
{
	int EnvSensActive;
	int	RowTemperature;
	int	RowHumidity;
	int	FltrTemperature;
	int	FltrHumidity;
}  ENV_SENS;
// ***************************************************************************
//		contains all the real measurements of the tank and enviroment
//		to be monitored in WEB app
// ***************************************************************************
typedef struct RealMeas
{
	float TankpH;
	float TankTemp;
	float EnvTemp1;
	float EnvHumidity1;
	float EnvTemp2;
	float EnvHumidity2;
}  REALMEAS;
//****************************************************************************
typedef struct Alarms
{
	FLAG TempHigh;
	FLAG TempLow;
	FLAG pH_High;
	FLAG pH_Low;
	FLAG EnvTempLow;
	FLAG EnvTempHigh;	
}  ALARMS;
//****************************************************************************
typedef struct Outputs
{
	BYTE HeaterIsON;
	BYTE FanIsON;
	BYTE CO2IsON;
	BYTE L0IsOn;
	int L0intensity;
	BYTE L0polarity;
	BYTE L1IsOn;
	int L1intensity;
	BYTE L1polarity;
	BYTE L2IsOn;
	int L2intensity;
	BYTE L2polarity;
} OUTP;
//****************************************************************************
typedef struct Events
{
	int LastE_hour;				// Last event hour:minute
	int LastE_min;
	int LastE_val;				// pwm out at last event
	int	NextEvent;				// nest event in seconds
	int NextE_val;				// pwm out of next event		
} EVT;
//****************************************************************************
typedef struct ADCconv
{
	int ADCact;																// If it is zero do not access the AD converter
	int chA;																// ADC channel A Row data
	int SigA;																// Flag to indicate a new value. Must be cleared by read funct.
	int chB;																// ADC channel B Row data	
	int SigB;																// Flag to indicate a new value. Must be cleared by read funct.
	int chC;																// ADC channel C Row data
	int SigC;																// Flag to indicate a new value. Must be cleared by read funct.
	int chA_fltr;															// Filtered channel A
	int chB_fltr;															// Filtered channel B
	int chC_fltr;	
}
ADC;
//****************************************************************************
typedef struct dbLastEntries
{
	int old_TankpH;					// TankpH is float but it is stored for comparison as int * 100 to avoid random noise
	int old_TankTemp;				// This is float but it is stored for comparison as int * 100 to avoid random noise
	BYTE old_L0IsOn;
	int old_L0intensity;
	BYTE old_L1IsOn;
	int old_L1intensity;
	BYTE old_L2IsOn;
	int old_L2intensity;
	BYTE old_HeaterIsON;
	BYTE old_FanIsON;
	BYTE old_CO2IsON;
	int old_EnvTemp1;				// This is float but it is stored for comparison as int * 100 to avoid random noise
	int old_EnvHumidity1;			// This is float but it is stored for comparison as int * 100 to avoid random noise	
}
DB_LAST;
//****************************************************************************
//
//****************************************************************************
