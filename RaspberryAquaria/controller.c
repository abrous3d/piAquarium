//============================================================================
//				Simple Bang-Bang controller with hysteresis
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

//============================================================================
#include "defines.h"
#include "config.h"
#include "controller.h"
//============================================================================
void ComparatorInit(struct H_COMPARATOR *a,uint32_t Hthres,uint32_t Lthres)
{
    a->LowThreshold = Lthres;
    a->HighThreshold = Hthres;
    a->y_old = 0;
}  
//============================================================================
//    Comparator with hysteresis
//============================================================================
uint8_t Comparator(struct H_COMPARATOR *a,uint32_t Vin)
{
  if(a->y_old){
    if(Vin <= a->LowThreshold)a->y_old=0;    
  }else{
    if(Vin >= a->HighThreshold)a->y_old=1;    
  }  
  return(a->y_old);
}
//============================================================================

//============================================================================
//    3 state Comparator 
//============================================================================
uint8_t Comparator3S(struct H_COMPARATOR *a,uint32_t Vin)
{
  
    if(Vin < a->LowThreshold)return(1);    
	else if(Vin > a->HighThreshold)return(2);

    return(0);
}
//============================================================================