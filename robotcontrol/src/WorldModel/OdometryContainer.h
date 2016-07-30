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


#ifndef Tribots_OdometryContainer_h_
#define Tribots_OdometryContainer_h_

#include "../Structures/DriveVector.h"
#include "../Structures/RobotLocation.h"
#include "../Fundamental/RingBuffer.h"
#include "../Fundamental/Time.h"

namespace Tribots {

  /** Klasse, um Odometrie und Fahrtvektoren einige Zyklen lang aufzubewahren und
      daraus zurueckgelegte zu berechnen */
  class OdometryContainer {
  private:
    /** Interne Struktur, um Fahrtvektoren mit Zeitstempel abspeichern zu koennen */
    struct TDV {
      Time timestamp;
      Vec vtrans;
      double vrot;
      bool kick;
    };
    RingBuffer<TDV> odo;   ///< Puffer fuer Odometriewerte; Konvention: zeigt immer auf aelteste Information
    RingBuffer<TDV> drv;   ///< Puffer fuer Fahrtvektoren; Konvention: zeigt immer auf aelteste Information
    unsigned int n;        ///< Groesse von odo und drv
    const double max_acc;    ///< maximale translat. Beschleunigung  
    const double max_racc;    ///< maximale rotat. Beschleunigung

  public:
    /** Konstruktor
	Arg1: Groesse der Puffer
	Arg2: maximale translatorische Beschleunigung
	Arg3: maximale rotatorische Beschleunigung */
    OdometryContainer (unsigned int =20, double =1e6, double =1e6) throw (std::bad_alloc);
    /** Destruktor */
    ~OdometryContainer () throw ();

    /** neue Odometriewerte einarbeiten; Annahme: Information ist neuer als bisherige */
    void add_odometry (DriveVector, Time) throw ();
    /** neuen Fahrtvektor einarbeiten; Annahme: Information ist neuer als bisherige */
    void add_drive_vector (DriveVector, Time) throw ();

    /** Berechne die Bewegung zwischen den Zeitpunkten arg1 und arg2;
	relativ zur Ausgangsposition zum Zeitpunkt arg1;
	arg1<arg2 wird vorausgesetzt;
	wenn vorhanden, wird Odometrieinformation verwendet, ansonsten DriveVektoren */
    RobotLocation movement (Time, Time) const throw ();
    /** Berechne die Bewegung zwischen den Zeitpunkten arg2 und arg3;
	und addiere sie zur Ausgangspoition arg1 hinzu;
	arg2<arg3 wird vorausgesetzt;
	wenn vorhanden, wird Odometrieinformation verwendet, ansonsten DriveVektoren */
    RobotLocation add_movement (RobotLocation, Time, Time) const throw ();
    /** Liefere Odometrie in ROBOTERKOORDINATEN zum Zeitpunkt Arg1 */
    DriveVector get_odometry (Time) const throw ();
    /** Liefere Fahrtvektor in ROBOTERKOORDINATEN zum Zeitpunkt Arg1 */
    DriveVector get_drive_vector (Time) const throw ();

  };

}

#endif

