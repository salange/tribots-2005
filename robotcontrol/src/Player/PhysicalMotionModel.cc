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


#include "PhysicalMotionModel.h"
#include "../Fundamental/geometry.h"
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace Tribots;
using namespace std;

#define DEBUG_MOTION_MODEL 0

PhysicalMotionModel::PhysicalMotionModel (const RobotProperties& rp) throw () : properties (rp) {
  average_cycle_time=33;
  first_cycle=true;
}

PhysicalMotionModel::~PhysicalMotionModel () throw () {;}


const DriveVector& PhysicalMotionModel::get_drive_vector (bool corr) throw () {
  // Zykluszeit anpassen
  Time now;
  if (!first_cycle)
    average_cycle_time = 0.7*average_cycle_time+0.3*now.diff_msec(cycle_timer);
  cycle_timer=now;

#if DEBUG_MOTION_MODEL
  cerr << "CYCLE-TIME= " << average_cycle_time << "\n\r";
#endif

  // neuen Fahrtvektor berechnen
  latest_drive_vector.kick=target_kick;
  latest_drive_vector.vrot=target_vrot;
  if (corr)
    latest_drive_vector.vtrans=target_vtrans.rotate (-current_heading-Angle::rad_angle(0.001*average_cycle_time*target_vrot)); // eigentlich muesste 0.0005 stehen; 0.001 steht hier unter Annahme, dass Rotation schneller umgesetzt wird als Translation
  else
    latest_drive_vector.vtrans=target_vtrans.rotate (-current_heading);

  return latest_drive_vector;
}


double PhysicalMotionModel::max_dec_vel (double s) throw () {
  double v = sqrt(0.002*properties.max_deceleration*s);
  v-=0.001*properties.max_deceleration*average_cycle_time;
  if (v<0)
    v=0;
  return v;
}

double PhysicalMotionModel::max_rot_dec_vel (double s) throw () {
  double v = sqrt(2.0*properties.max_rotational_deceleration*s);
  v-=0.001*properties.max_rotational_deceleration*average_cycle_time;
  if (v<0)
    v=0;
  return v;
}


bool PhysicalMotionModel::avoid_abrupt_changes () throw () {
  bool values_changed=false;

  if (!first_cycle) {
    // translative Bewegung:
    Vec vt_ist = latest_drive_vector.vtrans.rotate(current_heading);
    double delta = (target_vtrans-vt_ist).length();
    double max_diff = 0.001*average_cycle_time*properties.max_acceleration;
    double fmax = max_diff/delta;
    if (delta>0 && fmax<1.0) {
      values_changed=true;
      target_vtrans=(1-fmax)*vt_ist+fmax*target_vtrans;
    }
    // rotative Bewegung:
    delta = fabs(target_vrot-latest_drive_vector.vrot);
    max_diff =  0.001*average_cycle_time*properties.max_rotational_acceleration;
    fmax = max_diff/delta;
    if (fmax<1.0) {
      values_changed=true;
      target_vrot=latest_drive_vector.vrot+fmax*(target_vrot-latest_drive_vector.vrot);
    }
  } else
    first_cycle=false;

  return values_changed;
}


bool PhysicalMotionModel::avoid_point_collision (const Vec& cp) throw () {
  try{
    // moeglichen Kollisionspunkt berechnen
    Line drive_direction (current_position, current_position+target_vtrans);
    Vec pp = drive_direction.perpendicular_point (cp);
    // Minimalabstand bei gegenwaertiger Roboterorientierung abschaetzen,
    // einfach mal linear interpolieren
    double main_radius = std::sqrt(0.25*properties.robot_width*properties.robot_width+properties.min_robot_radius*properties.min_robot_radius);
    Angle ref_angle = current_heading-target_vtrans.angle();
    Angle tau = (ref_angle.in_between (Angle::zero, Angle::half) ? (ref_angle.in_between (Angle::zero, Angle::quarter) ? ref_angle : Angle::half-ref_angle) : (ref_angle.in_between (Angle::half, Angle::three_quarters) ? ref_angle-Angle::half : -ref_angle));
    double rho = 2.0*tau.get_rad()/M_PI;
    double min_durchfahrtsradius = rho*main_radius+(1-rho)*properties.min_robot_radius;

    // Kollision ueberpruefen
    if (((pp-cp).length()<min_durchfahrtsradius) && (target_vtrans*(cp-current_position)>0)) {
      double dist = (pp-current_position).length()-min_durchfahrtsradius;
      if (dist<0)
        dist=0;
      double max_v = max_dec_vel (dist);
      double cv = target_vtrans.length();
      if (cv>max_v) {
        target_vtrans*=max_v/cv;
        return true;
      }
    }
  }catch(exception&) {
    // Roboter bewegt sich gar nicht
  }
  return false;
}


void PhysicalMotionModel::set_kick (bool k) throw () {
  target_kick=k;
}


void PhysicalMotionModel::set_drive_vector (const DriveVector& dv, const RobotLocation& rl) throw () {
  current_position=rl.pos;
  current_heading=rl.heading;
  target_kick=dv.kick;
  target_vrot=dv.vrot;
  target_vtrans=dv.vtrans.rotate (current_heading);
}


void PhysicalMotionModel::set_no_motion () throw () {
  target_kick=false;
  target_vrot=0;
  target_vtrans.x=target_vtrans.y=0;
}


void PhysicalMotionModel::direct_approach_nostop (const Vec& target_position, const Angle& target_heading, const RobotLocation& rl, bool ignore_pos, bool ignore_rot) throw () {
  direct_approach_nostop (target_position, target_heading, rl, properties.max_velocity, properties.max_rotational_velocity, ignore_pos, ignore_rot);
}

void PhysicalMotionModel::direct_approach_nostop (const Vec& target_position, const Angle& target_heading, const RobotLocation& rl, double max_vtrans, double max_vrot, bool ignore_pos, bool ignore_rot) throw () {
  current_position=rl.pos;
  current_heading=rl.heading;
  target_kick=false;
  
  // ganz triviale Annahmen: fahre auf direktem Weg zum Ziel
  Vec target_vector = (target_position-current_position);
  target_vtrans = max_vtrans*target_vector.normalize();

  // gedaempfte Rotation
  Angle d_angle = target_heading-current_heading;
  int turn_direction = ( d_angle.in_between (Angle::zero, Angle::three_eighth) ? +1 : ( d_angle.in_between (Angle::five_eighth, Angle::zero) ? -1 : (rl.vrot>=0 ? +1 : -1 )));   // +1=links herum drehen, -1=rechts herum drehen
  double d_angle_d = ( turn_direction==1 ? d_angle.get_rad() : 2*M_PI-d_angle.get_rad());
  target_vrot = turn_direction*min(max_rot_dec_vel (d_angle_d), max_vrot);

  if (ignore_pos && target_vector.length()<180)
    target_vtrans = Vec(0,0);
  if (ignore_rot && d_angle.in_between (Angle::rad_angle (-0.1), Angle::rad_angle (0.1)))
    target_vrot = 0;

#if DEBUG_MOTION_MODEL
  cerr << "NOSTOP-VEL: " << target_vtrans.length() << ' ' << target_vtrans.angle().get_deg() << ' ' << target_vrot << "\n\r";
#endif
}

void PhysicalMotionModel::direct_approach_stop (const Vec& target_position, const Angle& target_heading, const RobotLocation& rl, bool ignore_pos, bool ignore_rot) throw () {
  direct_approach_stop (target_position, target_heading, rl, properties.max_velocity, properties.max_rotational_velocity, ignore_pos, ignore_rot);
}

void PhysicalMotionModel::direct_approach_stop (const Vec& target_position, const Angle& target_heading, const RobotLocation& rl, double max_vtrans, double max_vrot, bool ignore_pos, bool ignore_rot) throw () {
  direct_approach_nostop (target_position, target_heading, rl, max_vtrans, max_vrot, ignore_pos, ignore_rot);
  double max_v = max_dec_vel ((target_position-current_position).length());
  double ist_v = target_vtrans.length();
  if (ist_v>max_v)
    target_vtrans*=max_v/ist_v;

#if DEBUG_MOTION_MODEL
  cerr << "STOP-VEL: " << target_vtrans.length() << ' ' << target_vtrans.angle().get_deg() << ' ' << target_vrot << "\n\r";
#endif
}


void PhysicalMotionModel::dogs_approach_stop (const Vec& target_position, const Angle& target_heading, double ball_radius, int way_around, const RobotLocation& rl) throw () {
  current_position=rl.pos;
  current_heading=rl.heading;
  target_kick=false;
  
  Vec tg;                         // Zwischenziel
  Angle tgh = current_heading;    // Zwischenausrichtung
  double md = ball_radius+properties.max_robot_radius+50;  // Mindestabstand beim Vorbeifahren
  
  Vec h=Vec::unit_vector_x.rotate (target_heading+Angle::quarter);
  Vec hortho=way_around*h.rotate_quarter();
  Vec target_robot=current_position-target_position;
  
  double quer=(hortho*target_robot);
  double laengs=(h*target_robot);
  
  if (laengs>0 && quer<0)
    tg=target_position+1.2*md*h;
  else if (laengs<0 && quer<0) {
    tg=target_position;
    tgh=target_heading;    
  } else if (laengs>1.2*md) {
    tg=target_position+md*h+1.2*md*hortho;
    tgh=target_heading;
  } else if (laengs>0.5*md)
    tg=target_position+0.3*md*h+1.2*md*hortho;    
  else if (laengs>=-0.1*md)
    tg=target_position-0.5*md*h+1.2*md*hortho;    
  else if (laengs>=-md)
    tg=target_position-1.2*md*h+md*hortho;
  else if (std::fabs (quer)>2*ball_radius)
    tg=target_position-1.2*md*h;
  else {
    tg=target_position;
    tgh=target_heading;
  }

  direct_approach_nostop (tg, tgh, rl, 0.5, 2.0, false, false);
}


bool PhysicalMotionModel::avoid_own_goal (const FieldGeometry& fg, double distance_y) throw () {
  Vec left_post (-0.5*fg.goal_width, -0.5*fg.field_length);
  Vec right_post (0.5*fg.goal_width, -0.5*fg.field_length);
  Vec goal_center (0, -0.5*fg.field_length);

  bool values_changed=false;

  // Kollisionen mit Torpfosten verhindern
  values_changed |= avoid_point_collision (left_post);
  values_changed |= avoid_point_collision (right_post);

  // Korrekturen, um neben den Torpfosten im Torinneren nach vorne zu fahren (ohne Drehung)
  if ((current_position.y<goal_center.y+distance_y) && (current_position.x>left_post.x-properties.max_robot_radius-50) && (current_position.x<left_post.x+properties.max_robot_radius+50)) {
    if (target_vtrans.x<0) 
      target_vtrans.x=0;
    target_vtrans.y=0.3;
    target_vrot=0;
    values_changed=true;
  }
  if ((current_position.y<goal_center.y+distance_y) && (current_position.x<right_post.x+50+properties.max_robot_radius) && (current_position.x>right_post.x-properties.max_robot_radius-50)) {
    if (target_vtrans.x>0) 
      target_vtrans.x=0;
    target_vtrans.y=0.3;
    target_vrot=0;
    values_changed=true;
  }

  // Korrekturen, um nicht gegen die Torrueckwand zu fahren
  if (current_position.y-properties.min_robot_radius<goal_center.y-fg.goal_length)
    if (target_vtrans.y<0) {
      target_vtrans.y=0;
      target_vrot=0;
      values_changed=true;
    }
  
  return values_changed;
}

bool PhysicalMotionModel::avoid_into_goal (const FieldGeometry& fg, double distance_y) throw () {
  double dist = current_position.y+0.5*fg.field_length-distance_y;
  if (dist<0 && target_vtrans.y<0.3) {
    target_vtrans.y=0.3;
    return  true;
  }
  if (dist>0 && target_vtrans.y<0) {
    double max_v = max_dec_vel (dist);
    if (target_vtrans.y>max_v) {
      target_vtrans.y=max_v;
      return true;
    }
  }
  return false;
}
    
