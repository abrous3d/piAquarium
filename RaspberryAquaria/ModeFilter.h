//================================================================================
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

 struct pair
 {
   struct pair   *point;                              /* Pointers forming list linked in sorted order */
   uint32_t  value;                                   /* Values to sort */
 };
//--------------------------------------------------------
struct MED_FILTER{
  struct pair buffer[MEDIAN_FILTER_MAX_SIZE]; /* Buffer of nwidth pairs */
  struct pair *datpoint;               /* Pointer into circular buffer of data */
  struct pair small;         /* Chain stopper */
  struct pair big;               /* Pointer to head (largest) of linked list.*/  
};
//--------------------------------------------------------
void median_filter_init(struct MED_FILTER *a);
uint32_t median_filter(struct MED_FILTER *a,uint32_t datum,uint32_t fsize);
//==============================================================================
//                            Moving average filter stuff
//==============================================================================
struct MAV{
  uint8_t MaxTaps;
  uint8_t cnt;
  uint32_t filter[MAX_MAV_LENGTH]; 
  uint16_t OVS;                         // oversampling
  uint16_t id;
};
#define MAVINIT   0xDEAD
//==============================================================================
uint32_t MAVFilter(struct MAV *a,uint32_t in,uint8_t length,uint16_t OVS);
//==============================================================================