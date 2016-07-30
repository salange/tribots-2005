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


#ifndef Tribots_SLStuckSensor_h
#define Tribots_SLStuckSensor_h

#include "../Fundamental/Time.h"
#include "../Fundamental/RingBuffer.h"
#include "../Fundamental/ConfigReader.h"
#include "../Structures/RobotLocation.h"


namespace Tribots {

  /** Klasse, um zu erkennen, wenn der Roboter blockiert wird;
      vergleicht Fahrtvektoren mit der Geschwindigkeit, die
      aus der Selbstlokalisierung geschaetzt wird */
  class SLStuckSensor {
  public:
    /** Konstruktor */
    SLStuckSensor (const ConfigReader&) throw (std::bad_alloc);
    /** Destruktor */
    ~SLStuckSensor () throw ();
    /** Stuckfilter aktualisieren; diese Methode 1 mal pro Zyklus aufrufen */
    void update () throw ();
    /** Blockadeinformation abfragen 
	Arg1: Position des Roboters
	Arg2: Augenblicklicher Fahrtvektor des Roboters in Weltkoordinaten */
    RobotStuckLocation get_stuck_location (Vec, Vec) const throw ();

  private:
    struct TPOS {
      Time timestamp;
      RobotLocation rloc;
      double deriv;                     // Laenge des Gradienten
    };

    const unsigned int n;               ///< groesse des Ringpuffers
    RingBuffer<TPOS> oldpos;            ///< Liste mit alten DriveVector/Positionen

    unsigned int count;                 ///< Zaehler, der die Iteration mit Stuck-Merkmal zaehlt; ab 12 Iterationen --> stuck
    Time timestamp_latest_stuck;        ///< Zeitpunkt, zu dem der Roboter zuletzt blockiert war
    Vec pos_latest_stuck;               ///< Position der letzten Blockade in Weltkoordinaten
    Vec dir_latest_stuck;               ///< Fahrtrichtung bei letzter Blockade in Weltkoordinaten

    unsigned int stuck_min;             ///< Schranke, ab der fuer count>=stuck_max eine Stucksituation erkannt wird
    bool was_stuck;                  ///< letzte Iteration stuck gewesen?
  };

}

#endif
