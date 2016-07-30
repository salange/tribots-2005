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


#ifndef Tribots_ErrorMinimiserWorldModel_h_
#define Tribots_ErrorMinimiserWorldModel_h_

#include "WorldModelTypeBase.h"
#include "ErrorMinimiserSL.h"
#include "OdometryContainer.h"
#include "VisualContainer.h"
#include "DynamicSlidingWindowBallFilter.h"
#include "EMAObstacleFilter.h"
#include "SLVelocitySensor.h"
#include "SLStuckSensor.h"


namespace Tribots {

  /** Weltmodell, das auf einer Fehlerminimierung und Kalmanfilter beruht */
  class ErrorMinimiserWorldModel : public WorldModelTypeBase {
  private:
    OdometryContainer odobox;                    // Aufsammeln der Odometrie und Fahrtvektoren
    VisualContainer visbox;                      // Aufsammeln der Bildobjekte
    ErrorMinimiserSL sl;                         // Selbstlokalisierung
    DynamicSlidingWindowBallFilter ball_filter;  // Ballfilter (Position, Geschwindigkeit, Attribute)
    EMAObstacleFilter obstacle_filter;           // Hindernisfilter (Position, Attribute)
    SLVelocitySensor velocity_filter;            // Geschwindigkeitsschaetzung aus Selbstlokalisierung
    SLStuckSensor stuck_sensor;                  // Erkennung von Blockadesituationen
    Time latest_vis_timestamp;                   // Zeitstempel des letzten Bildes

  public:
    /** Konstruktor */
    ErrorMinimiserWorldModel (const ConfigReader&) throw (std::bad_alloc);
    /** Destruktor */
    ~ErrorMinimiserWorldModel () throw ();

    RobotLocation get_robot (Time) const throw ();
    BallLocation get_ball (Time) const throw ();
    ObstacleLocation get_obstacles (Time) const throw ();
    Time get_timestamp_latest_update () const throw ();
    const RobotLocation& get_slfilter_robot_location (Time&) const throw ();
    void set_drive_vector (DriveVector, Time) throw ();
    void set_odometry (DriveVector, Time) throw ();
    void set_visual_information (const VisibleObjectList&) throw ();
    void set_visual_information (const VisibleObject&, Time) throw ();
    void update_localisation () throw ();
    void reset () throw ();
    void reset (const Vec) throw ();
    void reset (const Vec, const Angle) throw ();
    const VisibleObjectList& get_visible_objects () throw ();
  };
}

#endif
