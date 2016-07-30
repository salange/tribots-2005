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


#ifndef tribots_visible_object_h
#define tribots_visible_object_h

#include "../Fundamental/Time.h"
#include "../Fundamental/Vec.h"
#include <vector>

namespace Tribots {

  /** Struktur, um ein einzelnes erkanntes Object zu beschreiben 
      alle Angaben in ROBOTERKOORDINATEN */
  struct VisibleObject {
    /** Default-Konstruktor */
    VisibleObject () throw ();
    /** Konstruktor
	Arg1: pos
	Arg2: object_type 
        Arg3: Breite des Objektes (nur fuer Hindernisse) */
    VisibleObject (Vec, int =0, double =0) throw ();
    /** Copy-Konstruktor */
    VisibleObject (const VisibleObject&) throw ();
    /** Zuweisungs-Operator */
    const VisibleObject& operator= (const VisibleObject&) throw ();
    
    Vec pos;            ///< Position des Referenzpunktes des Objektes im Roboter-Koordinatensystem
    int object_type;    ///< Objektart, Typ (z.B. Ball, Hindernis, blaues Tor, gelbes Tor, usw.)
    double width;       ///< Breite des Objektes quer zur Blickrichtung in mm; ACHTUNG: wird nur fuer Hindernisse verwendet!
    
    // Erweiterungen moeglich, z.B.:
    //  - Sicherheitswert, mit dem Objekt erkannt wurde
    //  - genauere geometrische Objektbeschreibung des Objektumfangs

    // Werte fuer das object_type-Attribut (statt haesslicher #define-Deklarativen):
    static const int unknown;      ///< object_type = unbekannt
    static const int ball;         ///< object_type = Ball
    static const int blue_goal;    ///< object_type = blaues Tor
    static const int yellow_goal;  ///< object_type = gelbes Tor
    static const int blue_goal_post_left;     ///< object_type = blaues Tor, linker Pfosten
    static const int yellow_goal_post_left;   ///< object_type = gelbes Tor, linker Pfosten
    static const int blue_goal_post_right;    ///< object_type = blaues Tor, rechter Pfosten
    static const int yellow_goal_post_right;  ///< object_type = gelbes Tor, rechter Pfosten
    static const int blue_pole;    ///< object_type = blau-gelb-blauer Eckpfosten
    static const int yellow_pole;  ///< object_type = gelb-blau-gelber Eckpfosten
    static const int teammate;     ///< object_type = Roboter der eigenen Mannschaft
    static const int opponent;     ///< object_type = Roboter der gegnerischen Mannschaft
    static const int obstacle;     ///< object_type = Hindernis (allgemein)
    static const int white_line;   ///< object_type = weisse Linie
    // weitere Deskriptoren koennen ergaenzt werden

    // Erweiterung der Klasse zum Lesen/Schreiben aus/in Stream moeglich
  };


  /** Listenstruktur fuer erkannte Objekte mit Zeitstempel */
  struct VisibleObjectList {
    Time timestamp;                         ///< Zeitstempel: angenommener Zeitpunkt, zu der Bild aufgenommen wurde
    std::vector<VisibleObject> objectlist;  ///< Objektliste

    VisibleObjectList (unsigned int len =0) throw () : objectlist(len) {;}
    ~VisibleObjectList () throw () {;}
    VisibleObjectList (const VisibleObjectList&) throw (std::bad_alloc);
    const VisibleObjectList& operator= (const VisibleObjectList&) throw (std::bad_alloc);

    /** Writes a ascii serialization of the object to a stream. 
	Arg1: stream to write on. 
    **/
    void writeAt(std::ostream &stream) const;

    /** reads a ascii serialization of the object from a stream.
     Arg1: stream to read from.
     Returns number of correct read obstacles.
    **/
    int  readFrom(std::istream &stream);
  };

}

#endif

