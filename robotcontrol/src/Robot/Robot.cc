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


#include "Robot.h"
#include "RobotFactory.h"
#include "../Structures/Journal.h"
#include "../WorldModel/WorldModel.h"
#include <cstring>

using namespace Tribots;
using namespace std;

const char* Tribots::Robot::get_robot_type () const throw () { return robot_descriptor; }

void Tribots::Robot::emergency_stop () throw () {
  Tribots::DriveVector dv (Vec(0,0),0,false);
  set_drive_vector (dv);
}

Tribots::Robot::~Robot () throw () {
  delete the_robot;
  delete [] robot_descriptor;
}

Tribots::Robot::Robot (const ConfigReader& vread) throw (TribotsException, bad_alloc) {
  string confline;
  
  if (vread.get("robot_type", confline)<=0) {
    JERROR("no config line \"robot_type\" found");
    throw Tribots::InvalidConfigurationException ("robot_type");
  }

  try{
    the_robot = RobotFactory::get_robot_factory()->get_robot (confline, vread);
  }catch(invalid_argument) {
    JERROR("unknown robot_type");
    throw Tribots::InvalidConfigurationException ("robot_type");
  }
  robot_descriptor = new char [confline.length()+1];
  strcpy (robot_descriptor, confline.c_str());
  
  if (Tribots::WorldModel::is_main_world_model_available()) {  // wenn ein Weltmodell vorhanden ist
    Tribots::WorldModel::get_main_world_model().set_robot_properties(the_robot->get_robot_properties());
  }
  else {
    JWARNING("no worldmodel available");
  }
}

