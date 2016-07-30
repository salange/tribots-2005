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


#ifndef tribots_robot_properties
#define tribots_robot_properties

#include "../Fundamental/ConfigReader.h"

namespace Tribots {

  /** Struktur, um fuer die Bewegungsplanung relevante Robotereigenschaften angeben zu koennen, 
      z. B. Maximalgeschwindigkeit, Beschleunigung, Bremsverzoegerung, Groesse des Roboters */
  struct RobotProperties {
    /** aus ConfigReader Werte lesen */
    void read (const ConfigReader&) throw ();

    double max_velocity;                 ///< Maximale translative Geschwindigkeit in m/s (>0)
    double max_acceleration;             ///< Maximale translative Beschleunigung in m/s2 (>0)
    double max_deceleration;             ///< Maximale translative Bremsverzoegerung in m/s2 (>0)

    double max_rotational_velocity;      ///< Maximale rotative Geschwindigkeit in rad/s (>0)
    double max_rotational_acceleration;  ///< Maximale rotative Beschleunigung in rad/s2 (>0)
    double max_rotational_deceleration;  ///< Maximale rotative Bremsverzoegerung in rad/s2 (>0)

    double max_robot_radius;             ///< Radius des kleinsten Kreises, in den der Roboter hineinpasst
    double min_robot_radius;             ///< Radius des groessten Kreises, der vollstaendig vom Roboter ueberdeckt wird
    double robot_width;                  ///< maximale Roboterbreite in mm
    double robot_length;                 ///< maximale Roboterlaenge in mm
    double kicker_width;                 ///< Breite der Kickeinheit in mm
    double kicker_distance;              ///< Abstand der Kickeinheit vom Robotermittelpunkt in mm

    bool omnidirectional_drive;          ///< true, wenn Roboter Omniantrieb besitzt, false bei Differentialantrieb

    int drive_vector_delay;              ///< Anzahl ms, die Ausfuehrung eines DriveVectors benoetigt
    
    // hier sind weitere relevante Groessen als Ergaenzung moeglich
  };

}

#endif

