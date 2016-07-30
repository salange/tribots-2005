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


#ifndef Tribots_LocationShortTimeMemory
#define Tribots_LocationShortTimeMemory

#include "../Fundamental/RingBuffer.h"
#include "../Fundamental/Time.h"
#include "../Structures/RobotLocation.h"
#include "../Structures/BallLocation.h"
#include "../Structures/ObstacleLocation.h"
#include "WorldModelTypeBase.h"
#include "ObjectInteractionManager.h"

namespace Tribots {

  class LocationShortTimeMemory {
  public:
    /** Konstruktor, uebergeben wird eine Referent auf das eigentliche Weltmodell,
	aus dem ggf. Positionen geholt werden koennen, falls nicht bereits vorhanden */
    LocationShortTimeMemory (const WorldModelTypeBase&) throw (std::bad_alloc);
    
    /** hole Roboterposition zum Zeitpunkt arg1, arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    const RobotLocation& get_robot_location (Time, bool) throw ();
    /** hole Ballposition zum Zeitpunkt arg1, arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    const BallLocation& get_ball_location (Time, bool) throw ();
    /** hole Hindernisposition zum Zeitpunkt arg1, arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    const ObstacleLocation& get_obstacle_location (Time, bool) throw ();

  private:
    const WorldModelTypeBase& wm;
    ObjectInteractionManager interaction_manager;

    template<class Loc>
    struct CycleTimeLocationTriple {
      unsigned long int cycle;
      Time timestamp;
      Loc value;
    };

    RingBuffer<CycleTimeLocationTriple<RobotLocation> > robots_pure;  // die gespeicherten Roboterpositionen, ohne Interaktion
    RingBuffer<CycleTimeLocationTriple<RobotLocation> > robots_interacted;  // die gespeicherten Roboterpositionen, mit Interaktion
    RingBuffer<CycleTimeLocationTriple<BallLocation> > balls_pure;  // die gespeicherten Ballpositionen, ohne Interaktion
    RingBuffer<CycleTimeLocationTriple<BallLocation> > balls_interacted;  // die gespeicherten Ballpositionen, ohne Interaktion
    CycleTimeLocationTriple<ObstacleLocation> obstacles;  // da z.Z. noch keine Hindernispraediktion oder -interaktion

    void get_interacted (Time);  // interagierte Locations berechnen und einspeichern
  };

}

#endif
