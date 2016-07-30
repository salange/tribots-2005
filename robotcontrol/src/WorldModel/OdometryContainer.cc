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


#include "OdometryContainer.h"
#include "update_robot_location.h"
#include <cmath>

using namespace std;
using namespace Tribots;

namespace {
  
  inline double rectified_velocity (const double& vrot) {
    return (abs(vrot)>1.2 ? 0.8 : (abs(vrot)>0.7 ? 0.9 : 1.0))*vrot;
  }

  inline Vec rectified_velocity (const Vec& vtrans) {
    return (vtrans.length()>1.2 ? 0.7 : (vtrans.length()>0.7 ? 0.8 : 1.0))*vtrans;
  }
  
}



OdometryContainer::OdometryContainer (unsigned int s, double mta, double mra) throw (std::bad_alloc) : odo(s), drv(s), n(s), max_acc (mta), max_racc (mra) {
  for (unsigned int i=0; i<n; i++) {
    odo[i].vtrans=drv[i].vtrans=Vec::zero_vector;
    odo[i].vrot=drv[i].vrot=0;
    odo[i].kick=drv[i].kick=false;
    odo[i].timestamp.set_msec(0);
    drv[i].timestamp.set_msec(0);
  }
}

OdometryContainer::~OdometryContainer () throw () {;}

void OdometryContainer::add_odometry (DriveVector dv, Time t) throw () {
  odo.get().timestamp=t;
  odo.get().vtrans=dv.vtrans;
  odo.get().vrot=dv.vrot;
  odo.get().kick=dv.kick;
  odo.step();
}

void OdometryContainer::add_drive_vector (DriveVector dv, Time t) throw () {
  const TDV& latest (drv[-1]);
  double dt = t.diff_msec (latest.timestamp);
  double dvtrans = (latest.vtrans-dv.vtrans).length();
  double dvrot = abs(latest.vrot-dv.vrot);

  // evtl. unmoegliche Veraenderungen der DriveVectoren korrigieren
  double mt = max_acc*dt*1e-3;
  if (mt<1e-3) mt=1e-3;
  if (dvtrans>mt)
    dv.vtrans = (1-mt/dvtrans)*latest.vtrans+(mt/dvtrans)*dv.vtrans;
  mt = max_racc*dt*1e-3;
  if (mt<1e-3) mt=1e-3;
  if (dvrot>mt)
    dv.vrot = (1-mt/dvrot)*latest.vrot+(mt/dvrot)*dv.vrot;
  
  drv.get().timestamp=t;
  drv.get().vtrans=dv.vtrans;
  drv.get().vrot=dv.vrot;
  drv.get().kick=dv.kick;
  drv.step();
}

RobotLocation OdometryContainer::movement (Time t1, Time t2) const throw () {
  RobotLocation start;
  start.pos=Vec::zero_vector;
  start.heading=Angle::zero;
  start.vtrans=Vec::zero_vector;
  start.vrot=0;
  return add_movement (start, t1, t2);
}

RobotLocation OdometryContainer::add_movement (RobotLocation start, Time t1, Time t2) const throw () {
  if (t1>=t2) {
    // nur Geschwindigkeiten zum Zeitpunkt t2 und kick setzen
    unsigned int odoindex = 0;
    while (odoindex+1<n)
      if (odo[odoindex+1].timestamp<=t2)
        odoindex++;
      else
        break;
    if (odoindex+1<n) {
      start.kick = odo[odoindex].kick;
      start.vrot = odo[odoindex].vrot;
      start.vtrans = odo[odoindex].vtrans.rotate (start.heading+start.vrot*t2.diff_msec(odo[odoindex].timestamp)*1e-3);
      return start;
    }
    unsigned int drvindex = 0;
    while (drvindex+1<n)
      if (drv[odoindex+1].timestamp<=t2)
        drvindex++;
      else
        break;
    start.kick = drv[drvindex].kick;
    start.vrot = drv[drvindex].vrot;
    start.vtrans = drv[drvindex].vtrans.rotate (start.heading+start.vrot*t2.diff_msec(drv[drvindex].timestamp)*1e-3);
    return start;
  }
  
  TDV cdv;
  Time tstart = t1;
  long int dt;

  // erste interessierende Odometrieinformation suchen
  unsigned int oindex = 1;
  while (oindex<n && odo[oindex].timestamp<=t1)
    oindex++;
  if (oindex<n)
    cdv = odo[oindex-1];
  else {
    // evtl. gibt es neuere DriveVectors
    unsigned int dindex = 1;
    while (dindex<n && drv[dindex].timestamp<=t1)
      dindex++;
    if (odo[n-1].timestamp<drv[dindex-1].timestamp)
      cdv = drv[dindex-1];
    else
      cdv = odo[n-1];
  }
  
  // jetzt schrittweise die Odometrieinformation ausnutzen
  while (oindex<n && odo[oindex].timestamp<t2) {
    dt = odo[oindex].timestamp.diff_msec (tstart);
    start.vtrans = rectified_velocity (cdv.vtrans.rotate (start.heading));
    start.vrot = rectified_velocity (cdv.vrot);
    start.kick = cdv.kick;
    start = update_robot_location (start, dt);
    cdv = odo[oindex];
    tstart = cdv.timestamp;
    oindex++;
  }
  
  if (oindex==n) {
    // Fall moeglicherweise gibt es noch DriveVectors, die juenger als die Odometrie sind

    // ersten interessierenden DriveVector suchen
    unsigned int dindex = 0;
    while (dindex<n && drv[dindex].timestamp<=tstart)
      dindex++;
    // jetzt schrittweise die DriveVectors ausnutzen
    while (dindex<n && drv[dindex].timestamp<t2) {
      dt = drv[dindex].timestamp.diff_msec (tstart);
      start.vtrans = rectified_velocity (cdv.vtrans.rotate (start.heading));
      start.vrot = rectified_velocity (cdv.vrot);
      start.kick = cdv.kick;
      start = update_robot_location (start, dt);
      cdv = drv[dindex];
      tstart = cdv.timestamp;
      dindex++;
    }
  }

  // den letzten Teilschritt bis zum Zeitpunkt t2 machen
  dt = t2.diff_msec (tstart);
  start.vtrans = rectified_velocity (cdv.vtrans.rotate (start.heading));
  start.vrot = rectified_velocity (cdv.vrot);
  start.kick = cdv.kick;
  start = update_robot_location (start, dt);

  start.vrot = cdv.vrot;
  start.vtrans = cdv.vtrans.rotate (start.heading);

  return start;
}

DriveVector OdometryContainer::get_odometry (Time t) const throw () {
  DriveVector dest (odo[0].vtrans, odo[0].vrot, odo[0].kick);
  unsigned int i=1;
  while (i<n && odo[i].timestamp<=t) {
    dest.vtrans = odo[i].vtrans;
    dest.vrot = odo[i].vrot;
    dest.kick = odo[i].kick;
    
    i++;
  }
  return dest;
}

DriveVector OdometryContainer::get_drive_vector (Time t) const throw () {
  DriveVector dest (drv[0].vtrans, drv[0].vrot, drv[0].kick);
  unsigned int i=1;
  while (i<n && drv[i].timestamp<=t) {
    dest.vtrans = drv[i].vtrans;
    dest.vrot = drv[i].vrot;
    dest.kick = drv[i].kick;
    i++;
  }
  return dest;
}

