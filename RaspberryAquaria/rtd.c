//**************************************************************
//                  			RTD.c
//        				Linearization of RTD signals
//  			using piecewise linear approximation method.
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

#define TMIN (-50)  // = minimum temperature in degC
#define TMAX (800)  // = maximum temperature in degC
#define RMIN (80.3063)  // = input resistance in ohms at -50 degC
#define RMAX (375.704)  // = input resistance in ohms at 800 degC
#define NSEG 255  // = number of sections in table
#define RSEG 1.15842  // = (RMAX-RMIN)/NSEG = resistance RSEG in ohms of each segment
//**************************************************************
// lookup tables

// pt1000 standard table
// pt100 standard table
const float C_rtd[] = {-50.0002,-47.0817,-44.1602,-41.2358,-38.3085,-35.3783,-32.4452,-29.5094,-26.5707,-23.6293,-20.6852,-17.7384,-14.7889,-11.8367,-8.88182,-5.92434,-2.96424,-0.00153641,2.96377,5.93167,8.90219,11.8753,14.8511,17.8295,20.8105,23.7942,26.7805,29.7695,32.7611,35.7554,38.7524,41.7521,44.7545,47.7596,50.7674,53.7779,56.7911,59.8071,62.8258,65.8473,68.8715,71.8984,74.9282,77.9607,80.996,84.0341,87.075,90.1187,93.1652,96.2145,99.2667,102.322,105.38,108.44,111.504,114.57,117.64,120.712,123.787,126.865,129.946,133.03,136.116,139.206,142.299,145.394,148.493,151.594,154.699,157.806,160.917,164.03,167.147,170.267,173.389,176.515,179.643,182.775,185.91,189.048,192.189,195.333,198.48,201.63,204.783,207.94,211.1,214.262,217.428,220.597,223.77,226.945,230.124,233.306,236.491,239.679,242.871,246.066,249.264,252.465,255.67,258.878,262.089,265.303,268.521,271.742,274.967,278.195,281.426,284.66,287.898,291.14,294.384,297.633,300.884,304.139,307.398,310.659,313.925,317.194,320.466,323.742,327.021,330.304,333.59,336.88,340.174,343.471,346.772,350.076,353.384,356.695,360.01,363.329,366.651,369.977,373.307,376.64,379.978,383.318,386.663,390.011,393.363,396.719,400.078,403.442,406.809,410.18,413.555,416.933,420.316,423.702,427.092,430.486,433.884,437.286,440.692,444.102,447.516,450.933,454.355,457.781,461.211,464.644,468.082,471.524,474.97,478.42,481.874,485.332,488.795,492.261,495.732,499.207,502.686,506.169,509.657,513.148,516.644,520.145,523.649,527.158,530.671,534.188,537.71,541.236,544.767,548.302,551.841,555.385,558.933,562.486,566.043,569.605,573.171,576.742,580.317,583.897,587.481,591.07,594.664,598.262,601.865,605.473,609.085,612.702,616.324,619.95,623.581,627.218,630.858,634.504,638.154,641.81,645.47,649.135,652.805,656.48,660.16,663.845,667.535,671.23,674.93,678.635,682.345,686.06,689.781,693.506,697.237,700.973,704.714,708.46,712.211,715.968,719.73,723.498,727.271,731.049,734.832,738.621,742.415,746.215,750.021,753.831,757.648,761.47,765.297,769.13,772.969,776.813,780.663,784.519,788.381,792.248,796.121,800};
// lookup table size:
//   = 255 linear sections
//   = 256 coefficients
//   = 1024 bytes (4 bytes per floating point coefficient)
//**************************************************************
// linearization routine error band:  
//   = -0.000366375degC .. 0.000374213degC
// specified over measurement range -50degC .. 800degC

//**************************************************************
// Temperature of RTD Function                             T_rtd
// input: r = resistance of RTD
// output: T_rtd() = corresponding temperature of RTD
// Calculates temperature of RTD as a function of resistance via
// a piecewise linear approximation method.
//**************************************************************
float T_rtd (float r) {
  float t;
  int i;
  i=(int)((r-RMIN)/RSEG);       // determine which coefficients to use
  if (i<0)               // if input is under-range..
    i=0;                 // ..then use lowest coefficients
  else if (i>NSEG-1)     // if input is over-range..
    i=NSEG-1;            // ..then use highest coefficients
  t = C_rtd[i]+(r-(RMIN+RSEG*i))*(C_rtd[i+1]-C_rtd[i])/RSEG;
  return (t);
}

// _____________________________________________________________
// Resistance of RTD Function                              R_rtd
// input: t = temperature of RTD
// output: R_rtd() = corresponding resistance of RTD
// Calculates resistance of RTD as a function of temperature via
// a piecewise linear approximation method.

float R_rtd (float t) {
  float r;
  int i, adder;

  // set up initial values
  i = NSEG/2;           // starting value for 'i' index
  adder = (i+1)/2;      // adder value used in do loop

  // determine if input t is within range
  if (t<C_rtd[0])           // if input is under-range..
    i=0;                    // ..then use lowest coefficients
  else if (t>C_rtd[NSEG])   // if input is over-range..
    i=NSEG-1;               // ..then use highest coefficients

  // if input within range, determine which coefficients to use
  else do {
    if (C_rtd[i]>t)   i-=adder; // either decrease i by adder..
    if (C_rtd[i+1]<t) i+=adder; // ..or increase i by adder
    if (i<0)       i=0;         // make sure i is >=0..
    if (i>NSEG-1)  i=NSEG-1;    // ..and <=NSEG-1
    adder = (adder+1)/2;        // divide adder by two (rounded)
  } while ((C_rtd[i]>t)||(C_rtd[i+1]<t));   // repeat 'til done

  // compute final result
  r = RMIN+RSEG*i + (t-C_rtd[i])*RSEG/(C_rtd[i+1]-C_rtd[i]);

  return (r);
}

// _____________________________________________________________
// Minimum Temperature Function                         Tmin_rtd
// Returns minimum temperature specified by lookup table.
float Tmin_rtd () {
  return (TMIN);
}

// _____________________________________________________________
// Maximum Temperature Function                         Tmax_rtd
// Returns maximum temperature specified by lookup table.
float Tmax_rtd () {
  return (TMAX);
}

// _____________________________________________________________
// Minimum Resistance Function                          Rmin_rtd
// Returns minimum RTD resistance specified by lookup table.
float Rmin_rtd () {
  return (RMIN);
}

// _____________________________________________________________
// Maximum Resistance Function                          Rmax_rtd
// Returns maximum RTD resistance specified by lookup table.
float Rmax_rtd () {
  return (RMAX);
}
