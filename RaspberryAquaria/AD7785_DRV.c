//==============================================================================
//                    Analog Devices AD7785 Driver
//                            27/9/2013
//							Open piAquarium	
//                            27/9/2013
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

//    0 : AD 0  on CE0 - AIN1 pH sensor AIN2 RTD  
//    1 : AD 1  on CE1 - AIN1  - AIN2  AIN3
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "defines.h"
#include "config.h"
//==============================================================================
#define   AD7785_DUMMY        0xFF
//==============================================================================
static void AD7785_ComWrite(unsigned char  cs,unsigned char  val);
static void AD7785_SeqReset(int cs);
static void AD7785_ConfigWrite(int cs,short val);
void AD7785_ModeWrite(int cs,short val);
static unsigned short AD7785_ModeRead(int  cs);
static unsigned short AD7785_ConfigRead(int cs);
static void AD7785_IoWrite(int cs,unsigned char val);
static unsigned char AD7785_IoRead(int cs);
//==============================================================================
static unsigned char SpiBuf[16];
//==============================================================================
//					init AD7785 on CE0 
//==============================================================================
void  AD7785_CE0_init(void)
{
 //============================== 
 //==== init isolated AD on xCS ====
 //==============================   
  wiringPiSPISetup (0, 2000000) ;	
  
  AD7785_SeqReset(0);                                                           // reset AD    
  usleep(700);																	// 500uS minimum time  
  //***** Calib *******
  AD7785_ConfigWrite(0,0x0010);    // was 1010                                             // calib ch 1
  AD7785_ModeWrite(0,0x800A);                                                   // Calibrate zero
  usleep(500);
  AD7785_ModeWrite(0,0xA00A);                                                   // Full scale Calibration
  usleep(500);
  AD7785_ConfigWrite(0,0x1411);			                                        // calib ch 2
  AD7785_ModeWrite(0,0x800A);                                                   // Calibrate zero
  usleep(500);
  AD7785_ModeWrite(0,0xA00A);                                                   // Full scale Calibration
  usleep(500);
  //******************* 

  AD7785_ConfigWrite(0,0x0010);                                                 // 00 (bias off) -  0 BO - 1 (unipolar) - 0 (boost) - (000) gain x1 - 0 REFSEL - 00 - 1 BUF - 0 - 000 channel  -> 0001 0011 0001 0000  0x1310    
  //AD7785_ModeWrite(0,0x204A);													// 16.7 Hz update rate - Single conversion
  AD7785_ModeWrite(0,0x004A);													// 16.7 Hz update rate - auto conversion
  AD7785_IoWrite(0,0x02);                                                       // 0000  - 00 IEXCDIRn - 11 IEXCEN = 210uA
}  
//==============================================================================
//					init AD7785 on CE0 
//==============================================================================
void  AD7785_CE1_init(void)
{
  
  //wiringPiSPISetup (1, 2000000) ;	
  
  //========================
  //==== init AD on CE1 ====
  //========================
  AD7785_SeqReset(1);                                                           // reset AD      
  usleep(700);                                                                   // 500uS minimum time

    //***** Calib *******
  AD7785_ConfigWrite(1,0x1090);                                                 // 00 (bias off) -  0 BO - 1 (unipolar) - 0 (boost) - (000) gain x1 - 1 REFSEL - 00 - 1 BUF - 0 - 000 channel    
  AD7785_ModeWrite(1,0x800A);                                                   // Calibrate zero
  usleep(500);
  AD7785_ModeWrite(1,0xA00A);                                                   // Full scale Calibration
  usleep(500);
  AD7785_ConfigWrite(1,0x1091);                                                 // calib ch 2
  AD7785_ModeWrite(1,0x800A);                                                   // Calibrate zero
  usleep(500);
  AD7785_ModeWrite(1,0xA00A);                                                   // Full scale Calibration
  usleep(500);
  AD7785_ConfigWrite(1,0x1092);                                                 // calib ch 2
  AD7785_ModeWrite(1,0x800A);                                                   // Calibrate zero
  usleep(500);
  AD7785_ModeWrite(1,0xA00A);                                                   // Full scale Calibration
  usleep(500);
  //*******************          
  AD7785_ConfigWrite(1,0x1090);                                                 // 00 (bias off) -  0 BO - 1 (unipolar) - 0 (boost) - (000) gain x1 - 1 REFSEL - 00 - 1 BUF - 0 - 000 channel
  //AD7785_ModeWrite(1,0x204A);                                                   // 0010 01 00 1101    16.7 Hz
  AD7785_ModeWrite(1,0x004A);                                                   // 0010 01 00 1101    16.7 Hz
  AD7785_IoWrite(1,0x00);                                                       // 0000  - 00 IEXCDIRn - 11 IEXCEN = OFF
}  
//==============================================================================
//		Switch AD0 MUX to pH input
//==============================================================================
void AD_mux_to_pH_on_CE0(void)
{
    AD7785_ConfigWrite(0,0x0010);		// was 1010                                                 // 00 (bias off) -  0 BO - 1 (unipolar) - 0 (boost) - (011) gain x1 - 0 REFSEL - 00 - 1 BUF - 0 - 000 channel  -> 0001 0011 0001 0000  0x1310     
	//AD7785_ModeWrite(0,0x204A);
}  
//==============================================================================
//		Switch AD0 MUX to RTD input
//==============================================================================
void AD_mux_to_RTD_on_CE0(void)
{
	// Switch gain to x8 for PT100
    AD7785_ConfigWrite(0,0x1411);	                                               // 00 (bias off) -  0 BO - 1 (unipolar) - 0 (boost) - (011) gain x8 - 0 REFSEL - 00 - 1 BUF - 0 - 000 channel  -> 0001 0011 0001 0000  0x1310    
	//AD7785_ModeWrite(0,0x204A);
}  
//==============================================================================
//		Switch AD1 MUX to AIN1 input
//==============================================================================
void AD_mux_to_AIN1_on_CE1(void)
{
    AD7785_ConfigWrite(1,0x1090);                                                 // 00 (bias off) -  0 BO - 1 (unipolar) - 0 (boost) - (000) gain x1 - 1 REFSEL - 00 - 1 BUF - 0 - 000 channel  
	//AD7785_ModeWrite(1,0x204A);
}  
//==============================================================================
//		Switch AD1 MUX to AIN2 input
//==============================================================================
void AD_mux_to_AIN2_on_CE1(void)
{
    AD7785_ConfigWrite(1,0x1091);                                                 // 00 (bias off) -  0 BO - 1 (unipolar) - 0 (boost) - (000) gain x1 - 1 REFSEL - 00 - 1 BUF - 0 - 000 channel  
	//AD7785_ModeWrite(1,0x204A);
}  
//==============================================================================
//		Switch AD1 MUX to AIN3 input
//==============================================================================
void AD_mux_to_AIN3_on_CE1(void)
{
    AD7785_ConfigWrite(1,0x1092);                                                 // 00 (bias off) -  0 BO - 1 (unipolar) - 0 (boost) - (000) gain x1 - 1 REFSEL - 00 - 1 BUF - 0 - 000 channel  
	//AD7785_ModeWrite(1,0x204A);
}  

//==============================================================================
//      AD7785 Communication register write func
//==============================================================================
static void AD7785_ComWrite(unsigned char  cs,unsigned char  val)
{
	if(cs > 1)return; 
	SpiBuf[0] = val;
	wiringPiSPIDataRW (cs, SpiBuf, 1) ;
}  
//==============================================================================
//      Reset the internal 7785 state machine writing 32 high bits
//==============================================================================
static void AD7785_SeqReset(int cs)
{
    if(cs > 1)return; 
	SpiBuf[0] = 0xFF; SpiBuf[1] = 0xFF; SpiBuf[2] = 0xFF; SpiBuf[3] = 0xFF; 
	wiringPiSPIDataRW (cs, SpiBuf, 4) ;   	
}  
//==============================================================================
//      AD7785 Write Config register func (16B)
//==============================================================================
static void AD7785_ConfigWrite(int cs,short val)
{
  if(cs > 1)return; 
  SpiBuf[0] = 0x10; 
  SpiBuf[1] = val >> 8; 
  SpiBuf[2] = val; 
  wiringPiSPIDataRW (cs, SpiBuf, 3) ;   
} 
//==============================================================================
//      AD7785 Write Mode register func
//==============================================================================
void AD7785_ModeWrite(int cs,short val)
{
   
 if(cs > 1)return; 
  SpiBuf[0] = 0x08; 
  SpiBuf[1] = val >> 8; 
  SpiBuf[2] = val; 
  wiringPiSPIDataRW (cs, SpiBuf, 3) ;   
}
//==============================================================================
//      AD7785 Read Mode register func
//==============================================================================
static unsigned short AD7785_ModeRead(int  cs)
{
 unsigned short val;	

  if(cs > 1)return; 
  SpiBuf[0] = 0x48; 
  SpiBuf[1] = AD7785_DUMMY; 
  SpiBuf[2] = AD7785_DUMMY; 
  wiringPiSPIDataRW (cs, SpiBuf, 3) ;   
  
  val = (SpiBuf[1] << 8);							// High byte
  val |= SpiBuf[2] ;								// Low Byte	
  
  return(val);
}
//==============================================================================
//      AD7785 Read status register func
//==============================================================================
unsigned char AD7785_StatusRead(int  cs)
{

	if(cs > 1)return; 
	SpiBuf[0] = 0x40; 
	SpiBuf[1] = AD7785_DUMMY; 
	wiringPiSPIDataRW (cs, SpiBuf, 2) ;   
	return(SpiBuf[1]);
}
//==============================================================================
//      AD7785 Read Config register func (16B)
//==============================================================================
static unsigned short AD7785_ConfigRead(int cs)
{

unsigned short val;	

  if(cs > 1)return; 
  SpiBuf[0] = 0x50; 
  SpiBuf[1] = AD7785_DUMMY; 
  SpiBuf[2] = AD7785_DUMMY; 
  wiringPiSPIDataRW (cs, SpiBuf, 3) ;   
  
  val = (SpiBuf[1] << 8);							// High byte
  val |= SpiBuf[2] ;								// Low Byte	
  
  return(val);

}
//==============================================================================
//      AD7785 Read data register func (24B)
//==============================================================================
unsigned int AD7785_DataRead(int cs)
{
	unsigned int res;

	if(cs > 1)return; 
	SpiBuf[0] = 0x58; 
	SpiBuf[1] = AD7785_DUMMY; 
	SpiBuf[2] = AD7785_DUMMY; 
	SpiBuf[3] = AD7785_DUMMY; 
	wiringPiSPIDataRW (cs, SpiBuf, 4) ;   
  
	res = SpiBuf[1];
	res <<= 8;
	res |= SpiBuf[2];
	res <<= 8;	
	res |= SpiBuf[3];
	//res = (unsigned int *)(SpiBuf+1);							// check this out
	res >>= 4;
	return(res);
}
//==============================================================================
//      AD7785 Write IO register func (8Bit)
//==============================================================================
static void AD7785_IoWrite(int cs,unsigned char val)
{   
	if(cs > 1)return; 
	SpiBuf[0] = 0x28; 
	SpiBuf[1] = val; 
	wiringPiSPIDataRW (cs, SpiBuf, 2) ;    
}
//==============================================================================
//      AD7785 Read IO register func (8Bit)
//==============================================================================
static unsigned char AD7785_IoRead(int cs)
{   
  if(cs > 1)return; 
	SpiBuf[0] = 0x68; 
	SpiBuf[1] = AD7785_DUMMY; 
	wiringPiSPIDataRW (cs, SpiBuf, 2) ;    
	return(SpiBuf[1]);
}
//==============================================================================
//==============================================================================

