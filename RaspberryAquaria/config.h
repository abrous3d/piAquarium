//================================================================================
//						Configuration header
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
//				
//================================================================================
#define		USER_TEMP_MIN_HYSTERESIS		0.01									// Min allowable hysteresis
#define		USER_FAN_MIN_HYSTERESIS			0.01									// minimum allowed fan hysteresis	
#define		USER_PH_MIN_HYSTERESIS			0.01									
#define		USER_PH_SET_MIN					5.0									// Do not allow user to set values lower than pH 5.0
#define		USER_PH_SET_MAX					9.0									// Do not allow user to set values Higher than pH 9.0
#define		USER_TEMP_SET_MIN				18.0								// 
#define		USER_TEMP_SET_MAX				32.0								// Do not let the control to boil your tank
#define 	MAX_EVENTS						16									// Maximum allowed events per channel

#define		PWM_USE_HARDWARE				1									// leave this uncomented to use hardware PWM for channel 1 - WARNING channel 1 is used for audio so it may interfere with audio apps
//================================================================================
//				Configuration definitions
//================================================================================
#define     MEDIAN_FILTER_MAX_SIZE            33									
#define     MAX_MAV_LENGTH                    64

#define     pH_MEDIAN_LEN					  11								// Median filter for pH - increase for more intense filtering 		
#define     pH_MAV_LEN					   	  16								// Moving average filter for pH electrode - 

#define     TEMP_MEDIAN_LEN					  11
#define     TEMP_MAV_LEN				   	  16

#define		ENV_TEMP1_MED_LEN				  5									// environmental sensor filtering
#define		ENV_TEMP1_MAV_LEN				  4

#define		ENV_HUM1_MED_LEN				  9
#define		ENV_HUM1_MAV_LEN				  8

#define		ENV_TEMP2_MED_LEN				  5									// environmental sensor filtering
#define		ENV_TEMP2_MAV_LEN				  4

#define		ENV_HUM2_MED_LEN				  9
#define		ENV_HUM2_MAV_LEN				  8

#define 	TERMINAL_OUT					  1									// comment this to switch off console output	

#define		MYSQL_USER						  "root"							// MYSQL database user
#define		MYSQL_PASS						  "piaquarium"						// MYSQL database password
#define		DATABASE_DEPTH_HOURS			  24
#define		DATABASE_OPTIMIZE_DAYS			  0									// Optimize database every N days (0 = Never)
//==============================================================================
//            Temperature sensor reference values
//==============================================================================
#define     R2_MINUS_R1						4126                                  // 41.26 Ohm  
#define     R1_REF							10009                                 // 100.09 Ohm
#define     R2_REF							14135                                 // 141.35 Ohm 

#define		RTD_DEF_OFFSET					1.0	
#define		RTD_DEF_GAIN					1.0	
//==============================================================================
//            pH sensor reference values
//==============================================================================
#define     pH4_MINUS_pH7   			    -300                                 // 3.00 pH  
#define     pH4_REF							400                                  // pH 7.00
#define     pH7_REF							700                                  // pH 4.00

#define		PH1_DEF_OFFSET					1.0	
#define		PH1_DEF_GAIN					1.0	
//================================================================================
//================================================================================
