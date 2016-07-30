/*
 * Copyright (c) 2002-2005, Neuroinformatics research group, 
 * University of Osnabrueck <tribots@informatik.uni-osnabrueck.de>
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _OMNIROBOTDATA_H_
#define _OMNIROBOTDATA_H_

typedef struct ctrOmniRobotData_s{
  ctrOmniRobotData_s()
  {
    tmc_version = -1;
    for (int i=0; i<3; i++) {
      wheel_vel[i]=0; robot_vel[i]=0; current[i]=0;
      temp_switch[i]=0; temp[i]=0; output[i]=0;
    }
    timestamp_vel.tv_sec=-1; timestamp_vel.tv_usec=-1; 
    timestamp_current.tv_sec=-1; timestamp_current.tv_usec=-1;
    timestamp_temp_switch.tv_sec=-1; timestamp_temp_switch.tv_usec=-1;
    timestamp_temp.tv_sec=-1; timestamp_temp.tv_usec=-1;
    vcc = 0;
    timestamp_vcc.tv_sec=-1; timestamp_vcc.tv_usec=-1;
  };

  int      tmc_version;        // tmc_version (0 alt, 1 SW16, 2 SW116)
  
  float    wheel_vel[3];       // Radgeschwindigkeit in rad/s f�r Rad 1,2,3
  float    robot_vel[3];       // Robotergeschwindigkeit in x,y (m/s) und phi (rad/s) (y->vorne)
  timeval  timestamp_vel;

  float    current[3];         // Str�me f�r die 3 Motoren (A)
  timeval  timestamp_current;

  float    output[3];          // Ausgangsspannung an die Motoren [%der Betriebsspannung]

  // Folgende Information wird nur in tmc_version >= 2 gef�llt
  bool     temp_switch[3];     // Temperatur Schalter f�r die drei Motoren (1-> wird begrenzt)
  timeval  timestamp_temp_switch;

  float    temp[3];            // Temperatur der drei Motoren in Grad Cels
  timeval  timestamp_temp;

  float    vcc;                // Betriebsspannung in V
  timeval  timestamp_vcc;

} ctrOmniRobotData_t;

#endif
