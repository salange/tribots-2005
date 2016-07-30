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

#ifndef TRIBOTS_OMNIROBOT_TRIBOT_H
#define TRIBOTS_OMNIROBOT_TRIBOT_H

#include "RobotType.h"
#include "../WorldModel/WorldModel.h"
#include "../Fundamental/ConfigReader.h"
#include <iostream>
#include "ctrOmniRobot.h"

namespace Tribots {
  class OmniRobot_Tribot:public RobotType {
    
  protected:
    bool blocking_communication;
    ctrOmniRobot* ctrRobot;
    ctrOmniRobotData_t data[10];

    std::ostream * hardware_errout_stream;
    std::ostream * hardware_infoout_stream; 

    bool  amIKicking;
    Time  lastKickStart;
    Time  lastKickEnd;

    DriveVector lastDriveState;
    Time        lastDriveStateTime;

    RobotProperties robot_properties;

    void fill_robot_data( const ctrOmniRobotData_t &src, const DriveVector &dv_set, RobotData &target, Time &target_time);

  public:
    OmniRobot_Tribot(const ConfigReader & vr) throw(TribotsException, std::bad_alloc);
    ~OmniRobot_Tribot() throw();

    RobotProperties get_robot_properties () const throw ();
    
    void set_drive_vector (DriveVector dv) throw ();
    
  };
}

#endif

