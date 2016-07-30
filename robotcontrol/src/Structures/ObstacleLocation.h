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


#ifndef tribots_obstacle_location_h
#define tribots_obstacle_location_h

#include "../Fundamental/Vec.h"
#include <vector>
#include <iostream>

namespace Tribots {

  /** Struktur, um die Hindernisse auf dem Spielfeld zu repraesentieren 
      alle Koordinaten (in WELTKOORDINATEN) sind relativ zur aktuellen Spielrichtung:
      (0,0) ist der Mittelpunkt des Feldes
      die y-Achse zeigt in Richtung des gegnerischen Tores
      die x-Achse zeigt rechtwinklig dazu (rechtshaendisches Koordinatensystem)
      ein Winkel von 0 bezeichnet die Parallele zur x-Achse
      alle Laengenangaben in mm, alle Winkelangaben in rad 
      alle Geschwindigkeiten in m/s bzw. rad/s */
  struct ObstacleLocation {
    /** Standardkonstruktor, uebergeben wird die Anzahl Hindernisse */
    ObstacleLocation (unsigned int =0) throw (std::bad_alloc);
    /** Copy-Konstruktor */
    ObstacleLocation (const ObstacleLocation&) throw (std::bad_alloc);

    /** Writes a ascii serialization of the object to a stream. 
	Arg1: stream to write on. 
    **/
    void writeAt(std::ostream &stream) const;

    /** reads a ascii serialization of the object from a stream.
     Arg1: stream to read from.
     Returns number of correct read obstacles.
    **/
    int  readFrom(std::istream &stream);

    std::vector<Vec> pos;             ///< Position der Hindernisse
    std::vector<double> width;        ///< Breite der Hindernisse in mm

    // ggf. detailliertere Beschreibung kann noch ergaenzt werden

  };

}

#endif

