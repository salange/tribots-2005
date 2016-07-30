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


#ifndef _Tribots_SimulatorWorldModel_h_
#define _Tribots_SimulatorWorldModel_h_

#include "../WorldModel/WorldModelTypeBase.h"
#include "../WorldModel/DynamicSlidingWindowBallFilter.h"
#include "../WorldModel/ObstacleContainer.h"
#include "../WorldModel/OdometryContainer.h"
#include "../WorldModel/SLVelocitySensor.h"
#include "../WorldModel/SLStuckSensor.h"
#include "SimClient.h"

namespace Tribots {

  /** Klasse implementiert die Weltmodell-Seite der Simulatoranbindung */
  class SimulatorWorldModel : public WorldModelTypeBase {
  private:
    SimClient* the_sim_client;         ///< pointer auf den SimClient
    DynamicSlidingWindowBallFilter ball_filter;
    ObstacleContainer obstacle_filter;
    OdometryContainer odobox;
    SLVelocitySensor velocity_filter;
    SLStuckSensor stuck_sensor;
    RobotLocation cpos;
    Time cpos_time;

  public:
    SimulatorWorldModel (const ConfigReader&) throw (std::bad_alloc, Tribots::InvalidConfigurationException);
    ~SimulatorWorldModel () throw ();

    void update_localisation () throw ();
    RobotLocation get_robot (Time) const throw ();
    BallLocation get_ball (Time) const throw ();
    ObstacleLocation get_obstacles (Time) const throw ();
    Time get_timestamp_latest_update () const throw ();
    const RobotLocation& get_slfilter_robot_location (Time&) const throw ();
    void set_drive_vector (DriveVector, Time) throw ();

    // folgende Funktionen tun nichts:
    void set_odometry (DriveVector, Time) throw ();
    void set_visual_information (const VisibleObjectList&) throw ();
    void set_visual_information (const VisibleObject&, Time) throw ();
    void reset () throw ();
    void reset (const Vec) throw ();
  };

}

#endif

