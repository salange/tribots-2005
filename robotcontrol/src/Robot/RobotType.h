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


#ifndef tribots_robot_type_h
#define tribots_robot_type_h

#include "../Structures/DriveVector.h"
#include "../Structures/RobotProperties.h"
#include "../Structures/TribotsException.h"


namespace Tribots {
  
  /** abstrakte Klasse zur Modellierung eines Roboters 
      Schnittstelle der Roboterkomponente nach innen */
  class RobotType {
  public:
    virtual ~RobotType () throw () {;}

    /** liefere physikalische Eigenschaften des Roboters (Geschwindigkeit, Beschleunigung) */
    virtual RobotProperties get_robot_properties () const throw () =0;

    /** setze einen Fahr- und Kickbefehl
	Arg1: gewuenschter Fahrbefehl
	Ret: aktuelle Odometrie vor Ausfuehrung des Fahrbefehls (Geschwindigkeiten) 
	Seiteneffekte: Roboter gibt gesetzten Fahrbefehl sowie Odometrie an Weltmodell weiter */
    virtual void set_drive_vector (DriveVector) throw (BadHardwareException) =0;

    // evtl. weitere Funktionalitaet hier einbauen, falls benoetigt
    // z.B. Statusinformation ueber den Roboter
  };

}

#endif

