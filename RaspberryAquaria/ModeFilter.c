//=============================================================================
//                Fast Median filter based on linked list
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
#include "ModeFilter.h"
//==============================================================================
#define STOPPER 0                                      /* Smaller than any datum */
//==============================================================================
#define uint32_t	unsigned int
#define uint16_t	unsigned short
#define uint8_t		unsigned char
//==============================================================================
//
//==============================================================================
void median_filter_init(struct MED_FILTER *a)
{
    memset(a->buffer,0,sizeof(a->buffer));                                      /* Buffer of nwidth pairs */
    a->datpoint = a->buffer;                                                    /* Pointer into circular buffer of data */
    a->small.point = NULL;                                                      /* Chain stopper */
    a->small.value = STOPPER;
    a->big.point = &a->small;
    a->big.value = 0;                                                             /* Pointer to head (largest) of linked list.*/  
}  
//==============================================================================
//
//
//==============================================================================
uint32_t median_filter(struct MED_FILTER *a,uint32_t datum,uint32_t fsize)
{

 struct pair *successor;                              /* Pointer to successor of replaced data item */
 struct pair *scan;                                   /* Pointer used to scan down the sorted list */
 struct pair *scanold;                                /* Previous value of scan */
 struct pair *median;                                 /* Pointer to median */
 uint16_t i;

 if(fsize > MEDIAN_FILTER_MAX_SIZE ) fsize = MEDIAN_FILTER_MAX_SIZE;
 if(fsize < 3)fsize = 3;
    
 if (datum == STOPPER)
 {
   datum = STOPPER + 1;                               /* No stoppers allowed. */
 }

 if ( (++a->datpoint - a->buffer) >= fsize)
 {
   a->datpoint = a->buffer;                               /* Increment and wrap data in pointer.*/
 }

 a->datpoint->value = datum;                           /* Copy in new datum */
 successor = a->datpoint->point;                       /* Save pointer to old value's successor */
 median = &a->big;                                     /* Median initially to first in chain */
 scanold = NULL;                                       /* Scanold initially null. */
 scan = &a->big;                                       /* Points to pointer to first (largest) datum in chain */

 /* Handle chain-out of first item in chain as special case */
 if (scan->point == a->datpoint)
 {
   scan->point = successor;
 }
 scanold = scan;                                     /* Save this pointer and   */
 scan = scan->point ;                                /* step down chain */

 /* Loop through the chain, normal loop exit via break. */
 for (i = 0 ; i < fsize; ++i)
 {
   /* Handle odd-numbered item in chain  */
   if (scan->point == a->datpoint)
   {
     scan->point = successor;                      /* Chain out the old datum.*/
   }

   if (scan->value < datum)                        /* If datum is larger than scanned value,*/
   {
     a->datpoint->point = scanold->point;             /* Chain it in here.  */
     scanold->point = a->datpoint;                    /* Mark it chained in. */
     datum = STOPPER;
   };

   /* Step median pointer down chain after doing odd-numbered element */
   median = median->point;                       /* Step median pointer.  */
   if (scan == &a->small)
   {
     break;                                      /* Break at end of chain  */
   }
   scanold = scan;                               /* Save this pointer and   */
   scan = scan->point;                           /* step down chain */

   /* Handle even-numbered item in chain.  */
   if (scan->point == a->datpoint)
   {
     scan->point = successor;
   }

   if (scan->value < datum)
   {
     a->datpoint->point = scanold->point;
     scanold->point = a->datpoint;
     datum = STOPPER;
   }

   if (scan == &a->small)
   {
     break;
   }

   scanold = scan;
   scan = scan->point;
 }
 return median->value;
}

//==============================================================================
// Variable length MAV filter with oversampling Capability 
//==============================================================================
uint32_t MAVFilter(struct MAV *a,uint32_t in,uint8_t length,uint16_t OVS)
{
  long long accu;
  uint8_t i;
  
  if(a->id != 0xDEAD){                                                          // MAV is used for first time -> init filter 
    for(i=0;i < MAX_MAV_LENGTH;i++){
     a->filter[i] = in;
    } 
    a->id = 0xDEAD;
    a->MaxTaps = length;
    a->OVS = OVS;                                                              // initialize oversampling (always >= 1)  
    return(in);
  }  
  
  if(a->MaxTaps != length)a->MaxTaps = length;
  if(a->MaxTaps > MAX_MAV_LENGTH)a->MaxTaps =  MAX_MAV_LENGTH;
  
  accu=0;
  if(a->cnt >= a->MaxTaps)a->cnt = 0;
  a->filter[a->cnt++] = in;
  for(i=0;i < a->MaxTaps;i++){
    accu = accu + a->filter[i];
  }
  accu/=(uint32_t)(a->MaxTaps/a->OVS);                                          // x4 oversampling
  return((uint32_t)accu);                                                                 // FIX ME
}  
//==============================================================================
//==============================================================================