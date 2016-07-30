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

#ifndef tribots_robot_data_h
#define tribots_robot_data_h

#define MAX_IDSTR_LEN 200

#include "DriveVector.h"

namespace Tribots {
  /** Struktur, um Informationen des Roboters darstellen und kommunizieren zu können;
      alle Angaben in ROBOTERKOORDINATEN;
      Diese Daten stellen die realen Informationen des Roboters dar um ein erweitertes
      Verhalten und Reaktion auf Fehlerzustände (sowie Aufspüren von Fehlern) zu ermöglichen. Sie sind NICHT dazu gedacht
      bestehende Strukturen wie gesetzte Odometrie o.ä. zu ersetzen.*/

  struct RobotData {
    /** Default-Konstruktor */
    RobotData() throw ();
    /** Copy-Konstruktor */
    RobotData (const RobotData&) throw ();
     /** Zuweisungsoperator */
    const RobotData& operator= (const RobotData&) throw ();

    /** Name, aus dem der verwendete Motor-Controller und die verwendeten Motoren
	sowie der Robotertyp hervor gehen sollten */
    char robotIdString[MAX_IDSTR_LEN];
    
    /** Eindeutige ID des verwendeten MotorControllers. Dies hat direkten Einfluss auf die
	verwendeten Daten die hier ausgelesen werden können.
	Werte: [0] altes TMC (keine HW REV) mit alter SW (1.06) (Stand GO04)
	       [1] TMC (HW REV 1.2) mit SW (1.06)
	       [2] TMC (HW REV 1.2) mit SW (1.16)
	       [-1] kein realer Roboter (evtl. Simulation)
    */
    int      BoardID; 

    /** Flag ob die Motoren gerade aktiv sind oder ob sie auf Leerlauf geschaltet wurden */
    bool     motors_on;

    /** Die aktuellen Radgeschwindigkeiten der drei Räder 
        in [rad/s] */
    float    wheel_vel[3];

    /** Die aktuelle relative Geschwdigkeit die sich direkt aus den Radgeschwindigkeiten ergibt.
	(Diese Information steht hier nur zur Vollständigkeit da ohne Wissen über die Roboterkinematik
	mit den Radgeschwindigkeiten evtl. wenig anzufangen ist.) 
        0: x Richtung [m/s], 1: y Richtung (vorne)[m/s], 2: Drehgeschwindigkeit [rad/s] */
    float    robot_vel[3];
    
    /** Die aktuellen Ströme die in den Motoren fliessen. [A] */
    float    motor_current[3];
    
    /** Aktuelle Spannung, die an die Motoren angelegt wird [%der Betriebsspannung] (-100 ... 100)*/
    float    motor_output[3];
    
    /** gesetzter DriveVector in aktuellem Zyklus, wird erst später aktiv!! */
    DriveVector dv_set;

    /* --------------------------------------------------------------
     * Folgende Information ist nur in TMC SW Ver > 1.06 vorhanden
     * --------------------------------------------------------------*/

    /** Temperaturflag für die drei Motoren, dieser springt an, wenn der Motor die Grenztemperatur
	überschreitet. */
    bool motor_temp_switch[3];
    
    /** Aktuelle Motortemperatur der drei Motoren in [Grad Cels] */
    float motor_temp[3];

    /** Aktuelle Betriebsspannung des Roboters (Motoren und Motor-Controller) in [v] */
    float motor_vcc;
    
  };

}

#endif
