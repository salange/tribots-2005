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


#include "FileRobot.h"
#include "RobotFactory.h"
#include "../Structures/Journal.h"
#include "../WorldModel/WorldModel.h"
#include "../Fundamental/stringconvert.h"

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public RobotBuilder {
    static Builder the_builder;
  public:
    Builder () {
      RobotFactory::get_robot_factory ()->sign_up (string("File"), this);
    }
    RobotType* get_robot (const std::string&, const ConfigReader& reader, RobotType*) throw (TribotsException,bad_alloc) {
      return new FileRobot (reader);
    }
  };
  Builder the_builder;
}





FileRobot::FileRobot (const ConfigReader& reader) throw (std::bad_alloc) : odo_stream (NULL), drv_stream(NULL), odo_reader(NULL), drv_reader(NULL) {
  robot_properties.read (reader);
  string fname;
  if (reader.get ("odometry_file", fname)>0) {
    odo_stream = new ifstream (fname.c_str());
    if (!(*odo_stream)) {
      JERROR ("Odometry file: file error");
      delete odo_stream;
      odo_stream=NULL;
    } else {
      odo_reader = new DriveVectorReader (*odo_stream);
    }
  }
  if (reader.get ("drive_vector_file", fname)>0) {
    drv_stream = new ifstream (fname.c_str());
    if (!(*drv_stream)) {
      JERROR ("Drive vector file: file error");
      delete drv_stream;
      drv_stream=NULL;
    } else {
      drv_reader = new DriveVectorReader (*drv_stream);
    }
  }
}

FileRobot::~FileRobot () throw () {
  if (odo_reader) delete odo_reader;
  if (drv_reader) delete drv_reader;
  if (odo_stream) delete odo_stream;
  if (drv_stream) delete drv_stream;
}

RobotProperties FileRobot::get_robot_properties () const throw () {
  return robot_properties;
}

void FileRobot::set_drive_vector (DriveVector) throw () {
  Time now;
  if (odo_reader) {
    unsigned long int t1, t2;
    DriveVector obj;
    bool success = odo_reader->read_until (t1, t2, obj, now.get_msec());
    Time t;
    t.set_msec (t2);
    if (success)
      MWM.set_odometry (obj, t);
  }
  if (drv_reader) {
    unsigned long int t1, t2;
    DriveVector obj;
    bool success = drv_reader->read_until (t1, t2, obj, now.get_msec());
    Time t;
    t.set_msec (t2);
    if (success)
      MWM.set_drive_vector (obj, t);
  }
}


