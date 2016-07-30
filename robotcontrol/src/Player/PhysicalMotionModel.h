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


#ifndef _Tribots_PhysicalMotionModel_
#define _Tribots_PhysicalMotionModel_

#include "../Structures/DriveVector.h"
#include "../Structures/RobotProperties.h"
#include "../Structures/RobotLocation.h"
#include "../Structures/FieldGeometry.h"
#include "../Fundamental/Time.h"

namespace Tribots {

  /** Klasse, um das physikalische Bewegungsmodell zu implementieren;
      Speichert intern zuletzt berechnete DriveVectors zwischen, um abrupte 
      Bewegungsaenderungen zu vermeiden; */
  class PhysicalMotionModel {
  private:
    bool first_cycle;                   ///< erster Zyklus?
    Time cycle_timer;                   ///< Timer, um Zykluszeit zu ermitteln
    double average_cycle_time;          ///< mittlere Zykluszeit in ms
    DriveVector latest_drive_vector;    ///< im letzten Zyklus gesetzter Fahrtvektor in Roboterkoordinaten
    const RobotProperties properties;   ///< Robotereigenschaften

    Vec target_vtrans;                  ///< Zielgeschwindigkeit in Weltkoordinaten
    double target_vrot;                 ///< Ziel-Rotationsgeschwindigkeit
    bool target_kick;                   ///< Zielsituation: Kicker aktivieren?
    Vec current_position;               ///< gegenwaertige Position
    Angle current_heading;              ///< gegenwaertige Orientierung

    double max_dec_vel (double) throw ();      ///< berechne Maximalgeschwindigkeit, um in einem Bremsweg von arg1 zum Stehen zu kommen
    double max_rot_dec_vel (double) throw ();  ///< berechne maximale Rotationsgeschwindigkeit, um in einem Bremsweg von arg1 (rad) zum Stehen zu kommen

  public:
    /** Konstruktor */
    PhysicalMotionModel (const RobotProperties&) throw ();
    /** Destruktor */
    ~PhysicalMotionModel () throw ();
    

    /** Berechnen eines Fahrtvektors, 
	ohne Beruecksichtigung von Hindernissen und Zielanfahrtsrichtung,
	stoppt am Zielpunkt;
	legt Ergebnis intern ab (kann mit anderen Methoden weiterverarbeitet und mit get_drive_vector gelsen werden)
	arg1: Zielposition in Weltkoordinaten
	arg2: Zielausrichtung
	arg3: Ausgangssituation (Position & Geschwindigkeit)
	(arg4: Maximalgeschwindigkeit) 
	(arg5: Max. Rotationsgeschwindigkeit)
        arg4(6): kleine Veraenderungen der Position ignorieren? 
	arg5(7): kleine Veraenderungen der Orientierung ignorieren? */
    void direct_approach_stop (const Vec&, const Angle&, const RobotLocation&, bool =true, bool =true) throw ();
    void direct_approach_stop (const Vec&, const Angle&, const RobotLocation&, double, double, bool =true, bool =true) throw ();
    /** Berechnen eines Fahrtvektors,
	ohne Beruecksichtigung von Hindernissen und Zielanfahrtsrichtung,
	Geschwindigkeit am Zielpunkt beliebig;
	legt Ergebnis intern ab
	arg1: Zielposition Weltkoordinaten
	arg2: Zielausrichtung
	arg3: Ausgangssituation (Position & Geschwindigkeit) 
	(arg4: Maximalgeschwindigkeit)
	(arg5; Max. Rotationsgeschwindigkeit)
	arg4(6): kleine Veraenderungen der Position ignorieren? 
	arg5(7): kleine Veraenderungen der Orientierung ignorieren? */
    void direct_approach_nostop (const Vec&, const Angle&, const RobotLocation&, bool =true, bool =true) throw ();
    void direct_approach_nostop (const Vec&, const Angle&, const RobotLocation&, double, double, bool =true, bool =true) throw ();
    /** Berechnen einer Fahrtvektors fuer eine Hundekurve,
	ohne Beruecksichtigung von Hindernissen und Zielanfahrtsrichtung,
	stoppt am Zielpunkt
	legt Ergebnis intern ab
	arg1: Zielposition Weltkoordinaten
	arg2: Zielausrichtung
	arg3: Radius des zu umfahrenden Objektes (Mittelpunkt an Zielposition)
	arg4: Umfahrungsrichtung: +1=im Gegenuhrzeigersinn, -1=im Uhrzeigersinn
	arg5: Ausgangssituation (Position & Geschwindigkeit) */
    void dogs_approach_stop (const Vec&, const Angle&, double, int, const RobotLocation&) throw ();
    /** "Stehen bleiben" setzen */
    void set_no_motion () throw ();
    /** DriveVector direkt vorgeben 
	arg1: ZielDriveVector
	arg2: Ausgangssituation */
    void set_drive_vector (const DriveVector&, const RobotLocation&) throw ();
    /** Kicker setzen (defaultmaessig nicht gesetzt) */
    void set_kick (bool =true) throw ();

    /** Fahrtvektor anpassen, um Kolliesion mit dem Punkt (arg1) zu vermeiden 
	(beruecksichtigt die Roboterausmasse) 
	Rueckgabewert: war Anpassung notwendig? */
    bool avoid_point_collision (const Vec&) throw ();
    bool avoid_into_goal (const FieldGeometry&, double) throw ();  // nicht ins eigene Tor fahren
    /** Fahrtvektor anpassen, um abrupte Aenderungen der Geschwindigkeit zu vermeiden 
	Rueckgabewert: war Anpassung notwendig? */
    bool avoid_abrupt_changes () throw ();
    /** Bewegungen, um das eigene Tor moeglichst nicht zu beruehren und aus Torinnenraum herauszufahren 
	arg1: Feldgeometrie
	arg2: Mindestabstand von Torlinie in mm */
    bool avoid_own_goal (const FieldGeometry&, double) throw ();

    /** berechneten DriveVector zurueckliefern 
	arg1: korrigiere die Veraenderung der Fahrtrichtung durch Eigendrehung/Bogenfahrt? */
    const DriveVector& get_drive_vector (bool =false) throw ();
  };

}

#endif
