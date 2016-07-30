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


#include "RobotPositionPredictor.h"
#include "update_robot_location.h"

using namespace Tribots;


RobotPositionPredictor::RobotPositionPredictor () throw () {
  current_rloc.pos=Vec::zero_vector;
  current_rloc.vtrans=Vec::zero_vector;
  current_rloc.vrot=0;
}

void RobotPositionPredictor::set (const RobotLocation& r, Time t1, Time t2) throw () {
  current_rloc=r;
  timestamp_current_rloc=t1;
  timestamp_calculation=t2;
}

RobotLocation RobotPositionPredictor::get (Time t) const throw () {
  double dt1 = t.diff_msec(timestamp_current_rloc);
  double dt2 = t.diff_msec(timestamp_calculation);
  RobotLocation rloc = update_robot_location (current_rloc, dt1);
  rloc.quality *= 2000.0/(2000.0+std::fabs(dt2));  // eine plausible, abklingende Zuverlaessigkeitskurve

  return rloc;
}

