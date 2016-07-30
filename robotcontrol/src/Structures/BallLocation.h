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


#ifndef tribots_ball_location_h
#define tribots_ball_location_h

#include "../Fundamental/Vec.h"
#include "../Fundamental/Time.h"

namespace Tribots {

  /** Struktur, um die derzeitige Ballposition auf dem Spielfeld zu repraesentieren  
      alle Koordinaten (in WELTKOORDINATEN) sind relativ zur aktuellen Spielrichtung:
      (0,0) ist der Mittelpunkt des Feldes
      die y-Achse zeigt in Richtung des gegnerischen Tores
      die x-Achse zeigt rechtwinklig dazu (rechtshaendisches Koordinatensystem)
      alle Laengenangaben in mm, alle Geschwindigkeiten in m/s */
  struct BallLocation {
    Vec pos;              ///< Ballposition
    Vec velocity;         ///< Ballgeschwindigkeit in m/s

    double quality;       ///< Qualitaet der Positions- und Geschwindigkeitsschaetzung: 0=unzuverlaessig, 1=zuverlaessig
    Time lastly_seen;     ///< Zeitpunkt, zu dem der Ball zuletzt gesehen wurde
    int pos_known;        ///< ein Attribut, das angibt, ob die Ballposition bekannt ist oder nicht, Werte aus nachfolgenden statischen Konstanten

    static const int unknown;      ///< Ballposition vollkommen unbekannt
    static const int known;        ///< Ballposition hinreichend genau bekannt
    static const int raised;       ///< Ball mutmaslich hochgeschossen oder hochgenommen
    static const int communicated; ///< Ballposition von Mitspielern kommuniziert
  };

}

#endif

