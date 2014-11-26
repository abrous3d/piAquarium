//==============================================================================
//                    Analog Devices AD7785 Driver
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

void  AD7785_CE0_init(void);
void  AD7785_CE1_init(void);
void AD_mux_to_pH_on_CE0(void);
void AD_mux_to_RTD_on_CE0(void);
void AD_mux_to_AIN1_on_CE1(void);
void AD_mux_to_AIN2_on_CE1(void);
void AD_mux_to_AIN3_on_CE1(void);
unsigned char AD7785_StatusRead(int  cs);
unsigned int AD7785_DataRead(int cs);
//==============================================================================			 