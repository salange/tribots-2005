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


#include <cmath>
#include "update_robot_location.h"

using namespace Tribots;

RobotLocation Tribots::update_robot_location (const RobotLocation& start, double dt) throw () {
  RobotLocation end;
  end.stuck=start.stuck;
  end.kick=start.kick;
  end.quality=start.quality;
  end.vrot=start.vrot;
  double turning (start.vrot*dt*1e-3);
  end.heading=start.heading+Angle::rad_angle (turning);
  end.vtrans=start.vtrans.rotate (Angle::rad_angle (turning));
  if (start.vrot==0) {
    // geradlinige Fahrt ohne Drehung
    end.pos=start.pos+start.vtrans*dt;
  } else {
    // gekruemmte Fahrt auf einem Kreisbogen oder Drehung auf der Stelle
    Vec vtrans_rob = start.vtrans.rotate (-start.heading);
    double sf = std::sin(turning)/start.vrot*1e3;
    double cf = (std::cos(turning)-1.0)/start.vrot*1e3;
    Vec trans_rob (sf*vtrans_rob.x+cf*vtrans_rob.y, -cf*vtrans_rob.x+sf*vtrans_rob.y);
    end.pos=start.pos+trans_rob.rotate (start.heading);
  }
  return end;
}

RobotLocation Tribots::flip_sides (const RobotLocation& src, int dir) throw () {
  if (dir>0) return src;
  RobotLocation dest = src;
  dest.pos*=-1;
  dest.heading+=Angle::half;
  dest.vtrans*=-1;
  dest.stuck.pos_of_stuck*=-1;
  return dest;
}

BallLocation Tribots::flip_sides (const BallLocation& src, int dir) throw () {
  if (dir>0) return src;
  BallLocation dest = src;
  dest.pos*=-1;
  dest.velocity*=-1;
  return dest;
}

ObstacleLocation Tribots::flip_sides (const ObstacleLocation& src, int dir) throw () {
  if (dir>0) return src;
  ObstacleLocation dest = src;
  std::vector<Vec>::iterator it = dest.pos.begin();
  std::vector<Vec>::iterator itend = dest.pos.end();
  while (it<itend)
    (*(it++)) *= -1;
  return dest;
}

