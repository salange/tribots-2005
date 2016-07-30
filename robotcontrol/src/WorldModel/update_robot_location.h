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


#ifndef _Tribots_update_robot_location_h
#define _Tribots_update_robot_location_h

#include "../Structures/RobotLocation.h"
#include "../Structures/BallLocation.h"
#include "../Structures/ObstacleLocation.h"

namespace Tribots {
 
  /** Funktion zum Fortberechnen von Roboterposition und -Geschwindigkeit durch eine Fahrt
      Angaben in WELTKOORDINATEN 
      Arg1: Ausgangsposition und -Geschwindigkeit
      Arg2: Zeitdauer der Bewegung in msec
      Ret: Position und Geschwindigkeit nach Ausfuehren der Bewegung */
  RobotLocation update_robot_location (const RobotLocation&, double) throw ();

  /** Funktion zum Spiegeln eines RobotLocation Objektes in arg1;
      arg2 gibt an, ob gespiegelt werden soll (-1) oder nicht (+1). In letzterem
      Fall ist return = arg1 */      
  RobotLocation flip_sides (const RobotLocation&, int =-1) throw ();

  /** Funktion zum Spiegeln eines BallLocation Objektes in arg1;
      arg2 gibt an, ob gespiegelt werden soll (-1) oder nicht (+1). In letzterem
      Fall ist return = arg1 */      
  BallLocation flip_sides (const BallLocation&, int =-1) throw ();

  /** Funktion zum Spiegeln eines ObstacleLocation Objektes in arg1;
      arg2 gibt an, ob gespiegelt werden soll (-1) oder nicht (+1). In letzterem
      Fall ist return = arg1 */      
  ObstacleLocation flip_sides (const ObstacleLocation&, int =-1) throw ();

}

#endif

