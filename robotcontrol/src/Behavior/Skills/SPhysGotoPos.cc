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


#include "SPhysGotoPos.h"
#include "../../WorldModel/WorldModel.h"
#include <cmath>

using namespace Tribots;
using namespace std;


SPhysGotoPos::SPhysGotoPos () throw () : CmdGenerator ("SPhysGotoPos") {
  acc_delay=100;
  const RobotProperties& rp (MWM.get_robot_properties());
  set_dynamics (rp.max_velocity, rp.max_rotational_velocity, rp.max_acceleration, rp.max_rotational_acceleration);
  init (Vec::zero_vector, 0, true);
}

void SPhysGotoPos::init (Vec tp, Angle ta, bool st, bool tolp, bool tolh) throw () {
  max_target_velocity = (st ? 0.0 : max_tv);
  target_pos = tp;
  target_heading = ta;
  tolerance_pos = tolp;
  tolerance_heading = tolh;
}

void SPhysGotoPos::init (Vec tp, Angle ta, double mtv, bool tolp, bool tolh) throw () {
  max_target_velocity = (mtv>max_tv ? max_tv : mtv);
  target_pos = tp;
  target_heading = ta;
  tolerance_pos = tolp;
  tolerance_heading = tolh;
}

void SPhysGotoPos::set_dynamics (double vt, double vr, double at, double ar) throw () {
  max_tv = vt;
  max_rv = vr;
  max_ta = at;
  max_ra = ar;
  max_td = 0.75*max_ta;
  max_rd = 0.75*max_ra;
}

void SPhysGotoPos::get_dynamics (double& vt, double& vr, double& at, double& ar, double& bt, double& br) throw () {
  vt = max_tv;
  vr = max_rv;
  at = max_ta;
  ar = max_ra;
  bt = max_td;
  br = max_rd;
}

void SPhysGotoPos::getCmd(DriveVector* dest, const Time& texec) throw() {
  const RobotLocation& rloc_exec = MWM.get_robot_location(texec);
  Time tsmooth = texec;
  tsmooth.add_msec (-acc_delay);
  const RobotLocation& rloc_smooth = MWM.get_robot_location(tsmooth, false);

  LOUT << "\% red cross " << target_pos << " line " << target_pos << ' ' << target_pos+1000*Vec::unit_vector(target_heading+Angle::quarter) << '\n';

  // Rotation ausregeln
  double diff_heading = (target_heading-rloc_exec.heading).get_rad_pi();
  if (tolerance_heading) {
    if (abs(diff_heading)<0.06) diff_heading=0;  // minimale Regeldifferenzen (<3.6Grad) ignorieren
    if (abs(diff_heading)<0.12 && abs(rloc_exec.vrot)<1e-2) diff_heading=0;  // wegen kleiner Regeldifferenzen (<7.2Grad) den Roboter nicht in Bewegung setzen
  }
  if (diff_heading==0) 
    dest->vrot=0;
  else if (diff_heading>0)
    dest->vrot = max_rv;
  else
    dest->vrot = -max_rv;
  
  // Bremsweg fuer Rotation beruecksichtigen
  {
    double m = sqrt (2*max_rd*abs(diff_heading));  // Bremswegformel
    if (dest->vrot<-m) dest->vrot=-m;
    if (dest->vrot>m) dest->vrot=m;
  }
    
  // Maximalbeschleunigung beruecksichtigen
  if (1e3*(abs(dest->vrot)-rloc_smooth.vrot)>max_ra*acc_delay) {
    double delta = 1e-3*acc_delay*max_ra;
    if (dest->vrot<rloc_smooth.vrot-delta) dest->vrot = rloc_smooth.vrot-delta;
    if (dest->vrot>rloc_smooth.vrot+delta) dest->vrot = rloc_smooth.vrot+delta;
  }
  
  
  // Translation ausregeln
  Vec diff_pos = target_pos-rloc_exec.pos;
  if (tolerance_pos) {
    if (diff_pos.length()<50) diff_pos=Vec::zero_vector;  // minimale Regeldifferenzen (<5cm) ignorieren
    if (diff_pos.length()<100 && rloc_exec.vtrans.length()<1e-2) diff_pos=Vec::zero_vector;  // wegen kleiner Regeldifferenzen (<10cm) den Roboter nicht in Bewegung setzen
  }
  double desired_velocity = 0;
  if (diff_pos.length()>1)
    desired_velocity = max_tv;  // Achtung: hier noch in Weltkoordinaten!

  // Bremsweg beruecksichtigen
  if (desired_velocity>max_target_velocity) {
    double m = sqrt (max_target_velocity*max_target_velocity+2e-3*max_td*abs(diff_pos.length()));  // Bremswegformel
    if (desired_velocity<-m) desired_velocity=-m;
    if (desired_velocity>m) desired_velocity=m;
  }
  dest->vtrans = desired_velocity*diff_pos.normalize();
  
  // Maximalbeschleunigung beruecksichtigen
  double desired_acc = 1e3*(dest->vtrans-rloc_smooth.vtrans).length()/acc_delay;
  if (desired_acc>max_ta)
    dest->vtrans = rloc_smooth.vtrans+(max_ta/desired_acc)*(dest->vtrans-rloc_smooth.vtrans);
  
  // Zusammenhang zwischen Rotation und Translation beruecksichtigen
  if (abs(dest->vrot)>=1e-2) {
    Angle half_expected_rotation = Angle::rad_angle (0.5e-3*dest->vrot*MWM.get_game_state().actual_cycle_time);
    dest->vtrans /= (half_expected_rotation);
  }
  
  // Umrechnen von Welt- in Roboterkoordinaten
  dest->vtrans/=rloc_exec.heading;
}
