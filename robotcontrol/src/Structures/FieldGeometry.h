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


#ifndef tribots_field_geometry_h
#define tribots_field_geometry_h

#include "../Fundamental/ConfigReader.h"
#include "../Structures/TribotsException.h"


namespace Tribots {

  /** Struktur zur Repraesentation der Spielfeldgeometrie
      Alle Laengenangaben in mm */
  struct FieldGeometry {
    /** Standard-Konstruktor */
    FieldGeometry () throw ();
    /** Konstruktor, der die Parameter aus einem ConfigReader liest */
    FieldGeometry (const ConfigReader&) throw (InvalidConfigurationException);

    double field_length;           ///< Spielfeld-Laenge
    double field_width;            ///< Spielfeld-Breite

    double side_band_width;        ///< Breite des Bereichs neben den Seitenlinien
    double goal_band_width;        ///< Breite des Bereichs neben den Torauslinien

    double goal_area_length;       ///< Laenge (Tiefe) des Torraums
    double goal_area_width;        ///< Breite des Torraums
    double penalty_area_length;    ///< Laenge (Tiefe) des Strafraums
    double penalty_area_width;     ///< Breite des Strafraums
    double center_circle_radius;   ///< Radius der Kreisboegen an den Spielfeld-Ecken
    double corner_arc_radius;      ///< Radius des Mittelkreises
    double penalty_marker_distance;///< Abstand des Penaltymarkers vom Tor

    double line_thickness;         ///< Breite der weissen Linien im Innern des Feldes
    double border_line_thickness;  ///< Breite der weissen Linien zur Spielfeldbegrenzung

    double goal_width;             ///< Breite des Tores
    double goal_length;            ///< Laenge (Tiefe) des Tores
    double goal_height;            ///< Hoehe des Tores

    double pole_height;            ///< Hoehe der Eckpfosten
    double pole_diameter;          ///< Durchmesser der Eckpfosten
    double pole_position_offset_x; ///< Position der Eckpfosten: Abstand zur Seitenlinie
    double pole_position_offset_y; ///< Position der Eckpfosten: Abstand zur Torauslinie

    double ball_diameter;          ///< Durchmesser des Balles

    unsigned int lineset_type;     ///< Anordnung der Feldlinien; 0(default)=normal gemaess Robocup-Regeln, 1=Mit Mitteldreieck statt Mittelkreis
    
    // Moeglichkeit, um weitere Parameter einzufuegen


    /** Vergleich zweier Feldgeometrien; Abweichungen bis zu 10mm werden toleriert */
    bool operator== (const FieldGeometry&) const throw ();
    bool operator!= (const FieldGeometry&) const throw ();
  };

}

#endif

