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


#ifndef Tribots_ObstacleContainer_h
#define Tribots_ObstacleContainer_h

#include "../Structures/VisibleObject.h"
#include "../Structures/ObstacleLocation.h"
#include "../Structures/RobotLocation.h"
#include "../Structures/FieldGeometry.h"
#include "../Fundamental/ConfigReader.h"


namespace Tribots {

  /** Hindernis-Container ohne weitere Funktionalitaet
      liefert genau die Hindernisse, die im Bild gesehen wurden */
  class ObstacleContainer {
  private:
    ObstacleLocation obstacles;
    ObstacleLocation poles;
    unsigned int stuck_obstacle_delay;
  public:
    /** Konstruktor */
    ObstacleContainer (const ConfigReader&, const FieldGeometry&) throw ();

    /** liefere Position der Hindernisse*/
    ObstacleLocation get () const throw ();
    /** liefere Position der Hindernisse einschliesslich der Eckpfosten*/
    ObstacleLocation get_with_poles () const throw ();
    /** analysiere die Hindernisse anhand der visuellen Information;
	uebergeben wird die Liste erkannter Hindernisse und die Roboterposition zum Zeitpunkt
	der Bildinformation */
    /** liefere Position der Hindernisse einschliesslich der Eckpfosten 
	und der Positionen, an denen der Roboter blockiert war */
    ObstacleLocation get_with_poles_and_stuck () const throw ();
    void update (const VisibleObjectList&, const RobotLocation&) throw ();
    /** setze die Hindernisse neu;
	arg1: Positionen der Hindernisse
	arg2: zugehoerige Breiten
	arg3: Zeitpunkt der Informationen */
    void update (const std::vector<Vec>&, const std::vector<double>&, Time) throw ();
  };

}

#endif
