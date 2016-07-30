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


#include "ObstacleContainer.h"
#include "WorldModel.h"

using namespace Tribots;

ObstacleContainer::ObstacleContainer (const ConfigReader& cfg, const FieldGeometry& fg) throw () {
  stuck_obstacle_delay=2000;
  cfg.get ("ObstacleFilter::stuck_obstacle_delay", stuck_obstacle_delay);
  poles.pos.resize(4);
  poles.width.resize(4);
  double dx = 0.5*fg.field_width+fg.pole_position_offset_x;
  double dy = 0.5*fg.field_length+fg.pole_position_offset_y;
  double w = fg.pole_diameter;
  poles.pos[0] = Vec(dx,dy);
  poles.pos[1] = Vec(-dx,dy);
  poles.pos[2] = Vec(-dx,-dy);
  poles.pos[3] = Vec(dx,-dy);
  poles.width[0]=w;
  poles.width[1]=w;
  poles.width[2]=w;
  poles.width[3]=w;
}

ObstacleLocation ObstacleContainer::get () const throw () {
  return obstacles;
}

ObstacleLocation ObstacleContainer::get_with_poles () const throw () {
  ObstacleLocation dest = obstacles;
  dest.pos.insert (dest.pos.end(), poles.pos.begin(), poles.pos.end());
  dest.width.insert (dest.width.end(), poles.width.begin(), poles.width.end());
  return dest;
}

ObstacleLocation ObstacleContainer::get_with_poles_and_stuck () const throw () {
  ObstacleLocation dest = get ();
  dest.pos.insert (dest.pos.end(), poles.pos.begin(), poles.pos.end());  
  dest.width.insert (dest.width.end(), poles.width.begin(), poles.width.end());
  Time now;
  RobotLocation rloc = MWM.get_robot_location (now, false);
  if (rloc.stuck.msec_since_stuck<=stuck_obstacle_delay) {
    BallLocation bloc = MWM.get_ball_location (now, false);
    const RobotProperties& rp = MWM.get_robot_properties ();
    const FieldGeometry& fg = MWM.get_field_geometry ();
    double dist = rp.max_robot_radius;
    if ((bloc.pos-rloc.pos).length()< rp.max_robot_radius + fg.ball_diameter + 500)
      if (rloc.stuck.dir_of_stuck.angle (bloc.pos-rloc.pos).in_between (Angle::seven_eighth, Angle::eighth) && bloc.pos_known==BallLocation::known)
	dist+=fg.ball_diameter;
    dest.pos.push_back (rloc.stuck.pos_of_stuck+dist*rloc.stuck.dir_of_stuck.normalize());
    dest.width.push_back (rp.robot_width);
  }
  return dest;
}

void ObstacleContainer::update (const VisibleObjectList& vis, const RobotLocation& rpos) throw () {
  unsigned int n=vis.objectlist.size();
  obstacles.pos.resize(n);
  obstacles.width.resize(n);
  for (unsigned int i=0; i<n; i++) {
    obstacles.pos[i] = rpos.pos+vis.objectlist[i].pos.rotate(rpos.heading);
    obstacles.width[i] = vis.objectlist[i].width;
  }
}

void ObstacleContainer::update (const std::vector<Vec>& p, const std::vector<double>& w, Time) throw () {
  obstacles.pos.assign (p.begin(), p.end());
  obstacles.width.assign (w.begin(), w.end());
}


