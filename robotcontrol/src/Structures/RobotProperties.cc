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


#include "RobotProperties.h"


void Tribots::RobotProperties::read (const Tribots::ConfigReader& vr) throw () {
  double u;
  int i;
  if (vr.get("RobotProperties::max_velocity", u)>0 && u>0)
    max_velocity=u;
  if (vr.get("RobotProperties::max_acceleration", u)>0 && u>0)
    max_acceleration=u;
  if (vr.get("RobotProperties::max_deceleration", u)>0 && u>0)
    max_deceleration=u;
  if (vr.get("RobotProperties::max_rotational_velocity", u)>0 && u>0)
    max_rotational_velocity=u;
  if (vr.get("RobotProperties::max_rotational_acceleration", u)>0 && u>0)
    max_rotational_acceleration=u;
  if (vr.get("RobotProperties::max_rotational_deceleration", u)>0 && u>0)
    max_rotational_deceleration=u;
  if (vr.get("RobotProperties::max_robot_radius", u)>0 && u>0)
    max_robot_radius=u;
  if (vr.get("RobotProperties::min_robot_radius", u)>0 && u>0)
    min_robot_radius=u;
  if (vr.get("RobotProperties::robot_width", u)>0 && u>0)
    robot_width=u;
  if (vr.get("RobotProperties::robot_length", u)>0 && u>0)
    robot_length=u;
  if (vr.get("RobotProperties::kicker_width", u)>0 && u>0)
    kicker_width=u;
  if (vr.get("RobotProperties::kicker_distance", u)>0 && u>0)
    kicker_distance=u;
  if (vr.get("RobotProperties::drive_vector_delay", i)>0)
    drive_vector_delay=i;
}
