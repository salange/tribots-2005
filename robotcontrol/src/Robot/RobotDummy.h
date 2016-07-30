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


#ifndef tribots_robot_dummy_h
#define tribots_robot_dummy_h

#include "RobotType.h"
#include "../WorldModel/WorldModel.h"
#include "../Fundamental/ConfigReader.h"
#include <iostream>

namespace Tribots {

  /** Eine Dummy-Implementierung des Robotertyps; tut nichts anderes als
      die erhaltenen Fahrtbefehle an das Weltmodell weiterzureichen */
  class RobotDummy:public RobotType {
  private:
    RobotProperties robot_properties;
  public:
    RobotDummy (const ConfigReader& vr) throw ();

    ~RobotDummy () throw ();

    RobotProperties get_robot_properties () const throw ();

    void set_drive_vector (DriveVector) throw ();
  };

}

#endif

