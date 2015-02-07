//================================================================================
//					   MySQL interface functions
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "defines.h"
#include "config.h"
#include <wiringPi.h>
#include "database.h"
#include <mysql/mysql.h>
//**************************************************************************************************
extern ENV_SENS  DHT22_a;	
extern ENV_SENS  DHT22_b;		
extern OUTP Outputs;
extern REALMEAS Real;	

static void finish_with_error(MYSQL *con);

char SQLstring[360];		         // string to send to SQL engine
char TimeString[64];			     // formatted time
MYSQL *xcon;
//**************************************************************************************************
//
//**************************************************************************************************
void DbCreate(void)															// Create new database
{	

	MYSQL *con = mysql_init(NULL);
  
	if (con == NULL)
	{
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);	
	}

	if (mysql_real_connect(con, "localhost", MYSQL_USER, MYSQL_PASS, 
          NULL, 0, NULL, 0) == NULL) 
	{
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
	}  
  //if (mysql_query(con, "CREATE DATABASE Aquadb")) 
	if (mysql_query(con, "CREATE DATABASE IF NOT EXISTS Aquadb")) 
	{
      fprintf(stderr, "%s\n", mysql_error(con));
     // mysql_close(con);
     // exit(1);
	}
  	mysql_close(con);

//	if (mysql_query(con, "DROP TABLE IF EXISTS AquaPr"))							//	Create table with parameters 
//	{
  //    finish_with_error(con);
	//}
  
}
//**************************************************************************************************
//
//**************************************************************************************************
void DbTableCreate(void)
{
MYSQL *con = mysql_init(NULL);
  
  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }  

  if (mysql_real_connect(con, "localhost", MYSQL_USER, MYSQL_PASS, 
          "Aquadb", 0, NULL, 0) == NULL) 
  {
      finish_with_error(con);
  }    
#if 0
  if (mysql_query(con, "DROP TABLE IF EXISTS AquaTab")) {
      finish_with_error(con);
  }
#endif  
  //if (mysql_query(con, "CREATE TABLE AquaTab(id INT AUTO_INCREMENT PRIMARY KEY,Time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,cTime INTEGER UNSIGNED,pH DECIMAL(5,2),Temp DECIMAL(5,2),L0_state BIT(1),L0_intensity INTEGER UNSIGNED,L1_state BIT(1),L1_intensity INTEGER UNSIGNED,L2_state BIT(1),L2_intensity INTEGER UNSIGNED,Heat_State BIT(1),CO2_State BIT(1),Fan_State BIT(1),Env_Temp1 DECIMAL(5,2),Env_Hum1 DECIMAL(5,2),Env_Temp2 DECIMAL(5,2),Env_Hum2 DECIMAL(5,2))" ))  	 
  //if (mysql_query(con, "CREATE TABLE IF NOT EXISTS AquaTab(id INT AUTO_INCREMENT PRIMARY KEY,Time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,cTime INTEGER UNSIGNED,pH DECIMAL(5,2),Temp DECIMAL(5,2),L0_state BIT(1),L0_intensity INTEGER UNSIGNED,L1_state BIT(1),L1_intensity INTEGER UNSIGNED,L2_state BIT(1),L2_intensity INTEGER UNSIGNED,Heat_State BIT(1),CO2_State BIT(1),Fan_State BIT(1),Env_Temp1 DECIMAL(5,2),Env_Hum1 DECIMAL(5,2),Env_Temp2 DECIMAL(5,2),Env_Hum2 DECIMAL(5,2))" ))  
  if (mysql_query(con, "CREATE TABLE IF NOT EXISTS AquaTab(id INT AUTO_INCREMENT PRIMARY KEY,Time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,cTime INTEGER UNSIGNED,pH DECIMAL(5,2),Temp DECIMAL(5,2),L0_state INTEGER UNSIGNED,L0_intensity INTEGER UNSIGNED,L1_state INTEGER UNSIGNED,L1_intensity INTEGER UNSIGNED,L2_state INTEGER UNSIGNED,L2_intensity INTEGER UNSIGNED,Heat_State INTEGER UNSIGNED,CO2_State INTEGER UNSIGNED,Fan_State INTEGER UNSIGNED,Env_Temp1 DECIMAL(5,2),Env_Hum1 DECIMAL(5,2),Env_Temp2 DECIMAL(5,2),Env_Hum2 DECIMAL(5,2))" ))  
  {      
      finish_with_error(con);
  }

  mysql_close(con);
}
//**************************************************************************************************
//
//**************************************************************************************************
//**************************************************************************************************
//
//**************************************************************************************************
void DbConnect(void)
{
	xcon = mysql_init(NULL);
	if (xcon == NULL) 
	{
      fprintf(stderr, "%s\n", mysql_error(xcon));
      exit(1);
	}  

	if (mysql_real_connect(xcon, "localhost", MYSQL_USER, MYSQL_PASS, "Aquadb", 0, NULL, 0) == NULL) 
	{
      finish_with_error(xcon);
	}    
}
//=============================================================================
//=============================================================================
void DbDisconnect(void)
{
	mysql_close(xcon);
}
//=============================================================================
//		
//=============================================================================
static DB_LAST dbLast;
//=============================================================================
void DbWriteData(void)
{
	time_t rawtime;
	struct tm * timeinfo;

	
	if((dbLast.old_TankpH != ((int)(Real.TankpH * 100))) || (dbLast.old_TankTemp != ((int)(Real.TankTemp * 100))) || (Outputs.L0IsOn != dbLast.old_L0IsOn) || (Outputs.L1IsOn != dbLast.old_L1IsOn) || (Outputs.L2IsOn != dbLast.old_L2IsOn) || \
   	   (dbLast.old_HeaterIsON != Outputs.HeaterIsON) || (dbLast.old_CO2IsON != Outputs.CO2IsON) || (dbLast.old_FanIsON != Outputs.FanIsON)  || (dbLast.old_EnvTemp1 != ((int)(Real.EnvTemp1 * 100))) || (dbLast.old_EnvHumidity1 != ((int)(Real.EnvHumidity1 * 100))) ) 
	{
		dbLast.old_TankpH = (int)(Real.TankpH * 100);
		dbLast.old_TankTemp = (int)(Real.TankTemp * 100);
		dbLast.old_L0IsOn = Outputs.L0IsOn;
		dbLast.old_L1IsOn = Outputs.L1IsOn;
		dbLast.old_L2IsOn = Outputs.L2IsOn;
   	    dbLast.old_HeaterIsON = Outputs.HeaterIsON;
		dbLast.old_CO2IsON = Outputs.CO2IsON;
		dbLast.old_FanIsON = Outputs.FanIsON;
		dbLast.old_EnvTemp1 = (int)(Real.EnvTemp1 * 100);
		dbLast.old_EnvHumidity1 = (int)(Real.EnvHumidity1 * 100);

		goto AddNewEntry; 
	}
	return;

AddNewEntry:
	time(&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (TimeString,64,"%F %T",timeinfo);
	
    sprintf(SQLstring,"INSERT INTO AquaTab (cTime, pH, Temp,L0_state, L0_intensity, L1_state, L1_intensity, L2_state,L2_intensity, Heat_State,CO2_State,Fan_State,Env_Temp1,Env_Hum1,Env_Temp2,Env_Hum2) \
					   VALUES(unix_timestamp(now()),%5.2f,%5.2f,%d,%03d,%d,%03d,%d,%03d,%d,%d,%d,%5.2f,%5.2f,%5.2f,%5.2f)",Real.TankpH,Real.TankTemp,Outputs.L0IsOn,Outputs.L0intensity,Outputs.L1IsOn,Outputs.L1intensity,Outputs.L2IsOn,Outputs.L2intensity,Outputs.HeaterIsON,Outputs.CO2IsON,Outputs.FanIsON,Real.EnvTemp1,Real.EnvHumidity1,Real.EnvTemp2,Real.EnvHumidity2);
    
    if (mysql_query(xcon, SQLstring)) finish_with_error(xcon);

	UpdateCSVfromMySQL();


}
//**************************************************************************************************
//
//**************************************************************************************************

const char *tankcsv = "/piAquarium/web/tank.csv";
const char *iocsv = "/piAquarium/web/io.csv";
const char *envcsv = "/piAquarium/web/env.csv";


void UpdateCSVfromMySQL(void)
{	
	FILE *fp,*tmpfp;
	
	
	if(remove(tankcsv) == 0){
		if (mysql_query(xcon, "(SELECT 'Time','Temperature','pH' ) UNION (SELECT Time,pH,Temp FROM AquaTab INTO OUTFILE '/piAquarium/web/tank.csv' FIELDS TERMINATED BY ',' LINES TERMINATED BY '\n');")) 
		{      
			finish_with_error(xcon);
		}
	}
	if(remove(iocsv) == 0){		
		if (mysql_query(xcon, "(SELECT 'Time','Heater','CO2State','FanState') UNION  (SELECT Time,Heat_State,CO2_State,Fan_State  FROM AquaTab INTO OUTFILE '/piAquarium/web/io.csv' FIELDS TERMINATED BY ',' LINES TERMINATED BY '\n');"))
		{      
			finish_with_error(xcon);
		}
	}
	
	if(remove(envcsv) == 0){			// Delete file - Mysql cannot overwrite	
		if (mysql_query(xcon, "(SELECT 'Time','EnvTemp1','EnvHum1','EnvTemp2','EnvHum2') UNION (SELECT Time,Env_Temp1,Env_Hum1,Env_Temp2,Env_Hum2  FROM AquaTab INTO OUTFILE '/piAquarium/web/env.csv' FIELDS TERMINATED BY ',' LINES TERMINATED BY '\n');"))
		{      
			finish_with_error(xcon);
		}
	}
}
//**************************************************************************************************
//
//**************************************************************************************************
static void finish_with_error(MYSQL *con)
{

	fprintf(stderr, "%s\n", mysql_error(con));
	//mysql_close(con);		
	//exit(1);															// we dont want controller to exit
}
//**************************************************************************************************
//
//**************************************************************************************************
void DataBaseCleanup(void)
{
	static int DaysSinceLastOptimization;
	int interval = DATABASE_DEPTH_HOURS;
	/**************************************************************************/
	/*     DELETE from AquaTab WHERE time < DATE_SUB(NOW(), INTERVAL 8 HOUR); */
	/**************************************************************************/
	//sprintf(SQLstring,"DELETE from AquaTab WHERE time < DATE_SUB(NOW(), INTERVAL 24 HOUR)");
	sprintf(SQLstring,"DELETE from AquaTab WHERE time < DATE_SUB(NOW(), INTERVAL %d HOUR)", interval);
	if (mysql_query(xcon, SQLstring)) finish_with_error(xcon);
	
	if(DATABASE_OPTIMIZE_DAYS == 0)return;																	
	
	if(DaysSinceLastOptimization++ > DATABASE_OPTIMIZE_DAYS)									// Every n days try to optimize the database
	{
		DaysSinceLastOptimization = 0;
		sprintf(SQLstring,"OPTIMIZE TABLE AquaTab");
		if (mysql_query(xcon, SQLstring)) finish_with_error(xcon);
		//sprintf(SQLstring,"ALTER TABLE AquaTab ENGINE='InnoDB'");
		//if (mysql_query(xcon, SQLstring)) finish_with_error(xcon);
	}
}

//**************************************************************************************************