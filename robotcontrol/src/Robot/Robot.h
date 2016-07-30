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


#ifndef tribots_robot_h
#define tribots_robot_h

#include <stdexcept>
#include "RobotType.h"
#include "../Structures/TribotsException.h"
#include "../Fundamental/ConfigReader.h"


namespace Tribots {

  /** Schnittstelle der Roboterkomponente nach aussen
      Aufgaben: Ansteuerung des Roboters, Berechnung der Odometrie, Verwaltung verschiedenartiger Roboter */
  class Robot {
  private:
    RobotType* the_robot;
    char* robot_descriptor;

  public:
    /** Konstruktor
	Arg1: ConfigReader, aus dem alle relevanten Roboterparameter (Typ, technische Daten) ausgelesen werden 
        Seiteneffekte: die Robotereigenschaften werden dem Weltmodell mitgeteilt */
    Robot (const ConfigReader&) throw (TribotsException, std::bad_alloc);
    ~Robot () throw ();


    /** liefere eine Beschreibung des aktuellen Robotertyps, z.B. eine Bauartbezeichnung */
    const char* get_robot_type () const throw ();

    
    /** setze einen Fahr- und Kickbefehl
	Arg1: gewuenschter Fahrbefehl (Geschwindigkeiten)
	Ret: aktuelle Odometrie vor Ausfuehrung des Fahrbefehls (Geschwindigkeiten)
	Seiteneffekte: Roboter gibt gesetzten Fahrbefehl sowie Odometrie an Weltmodell weiter 
        Falls der Fahrbefehl so nicht realisierbar ist, wandelt das Robotermodul den Fahrbefehl ab */
    inline void set_drive_vector (DriveVector dv) throw (BadHardwareException) { return the_robot->set_drive_vector(dv); }

    /** Notstopp: setze Fahrvektor auf 0, deaktiviere Kicker 
        Seiteneffekte: keine */
    void emergency_stop () throw ();
  };

}

#endif

