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


#ifndef Tribots_SLVelocitySensor_h
#define Tribots_SLVelocitySensor_h

#include "../Structures/RobotLocation.h"
#include "../Fundamental/RingBuffer.h"
#include "../Fundamental/Time.h"


namespace Tribots {

  /** Klasse, um aus Selbstlokalisationspositionen die Robotergeschwindigkeit zu berechnen;
      Berechnung erfolgt mit Hilfe eines kleinsten Quadrateansatzes unter Annahme konstanter
      Rotations- und Translationsgeschwindigkeiten */
  class SLVelocitySensor {
  public:
    /** Konstruktor, arg1=Pufferlaenge=Laenge der Glaettung */
    SLVelocitySensor (unsigned int =10) throw (std::bad_alloc);
    /** eine neue Positionsschaetzung arg1 zum Zeitpunkt arg2 einbinden */
    void update (const RobotLocation&, Time) throw ();
    /** die zuletzt berechnete Geschindigkeit bekommen;
	Arg2: Rueckgabewert Zeitpunkt, fuer den die Berechnung erfolgt
	Ret: Position und Geschwindigkeiten in WELTKOORDINATEN des internen Bewegungsmodells */
    const RobotLocation& get (Time&) const throw ();
  private:
    /** Struktur um Tripel aus Zeit, Position und Orientierung zu speichern */
    struct TPH {
      Time timestamp;
      Vec pos;
      double heading;  // in rad
    };

    const unsigned int n;     ///< Groesse des Ringpuffers, Anzahl Beobachtungen
    unsigned int burn_in;     ///< ist >0, wenn der Ringpuffer noch teilweise uninitialisiert ist
    RobotLocation model;    ///< zuletzt berechnetes Bewegungsmodell
    Time timestamp;          ///< Zeitpunkt der letzten Berechnung
    RingBuffer<TPH> buffer;   ///< Ringpuffer mit alten Positionen, Anker zeigt auf aeltestes Element
  };

}

#endif
