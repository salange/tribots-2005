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


#include "RobotDummy.h"
#include "RobotFactory.h"
#include "../WorldModel/WorldModel.h"
#include <iostream>

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public RobotBuilder {
    static Builder the_builder;
  public:
    Builder () {
      RobotFactory::get_robot_factory ()->sign_up (string("Dummy"), this);
    }
    RobotType* get_robot (const std::string&, const ConfigReader& reader, RobotType*) throw (TribotsException,bad_alloc) {
      return new RobotDummy (reader);
    }
  };
  Builder the_builder;
}




RobotDummy::RobotDummy (const ConfigReader& vr) throw () {
  robot_properties.read (vr);
}

RobotDummy::~RobotDummy () throw () {;}

RobotProperties  RobotDummy::get_robot_properties () const throw () {
  return robot_properties;
}

void RobotDummy::set_drive_vector (DriveVector dv) throw () {
  Time tt;
  WorldModel::get_main_world_model().set_drive_vector (dv, tt);
  WorldModel::get_main_world_model().set_odometry (dv, tt);
  // ----- nachfolgende Zeile dient nur zu Testzwecken, spaeter bitte entfernen ------ 
  std::cout << dv.vtrans.x << ' ' << dv.vtrans.y << ' ' << dv.vrot << ' ' << (dv.kick ? "kick       " : "--           ") << '\n';
}
