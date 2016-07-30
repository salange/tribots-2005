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


#ifndef _Tribots_FieldLUT_h_
#define _Tribots_FieldLUT_h_

#include "../Structures/FieldGeometry.h"
#include "../Fundamental/Vec.h"

namespace Tribots {

  /** Klasse FieldLUT modelliert eine Look-up-table zur Speicherung von minimalen 
      Entfernungen zu weissen Linien 

      ACHTUNG: FieldLUT verwendet ein eigenes Koordinatensystem unabhaengig von der Spielrichtung.
               Ursprung ist der Spielfeldmittelpunkt
               die positive y-Achse weist in Richtung des blauen Tores */
  class FieldLUT {
  public:
    /** Konstruktor, uebergeben wird die Feldgeometrie sowie die Zellengroesse in mm */
    FieldLUT (const FieldGeometry&, unsigned int) throw (std::bad_alloc);
    /** Destruktor */
    ~FieldLUT () throw ();

    /** fuer den Punkt arg1 im FieldLUT-Koordinatensystem die minimale Distanz nachschlagen */
    double distance (const Vec&) const throw ();

    /** den Gradienten der Distanzfunktion an Punkt arg1 im FieldLUT-Koordinatensystem nachschlagen */
    Tribots::Vec gradient (const Vec&) const throw ();

  private:
    unsigned int x_res;                                // Aufloesung in x-Richtung (1/2 Anzahl Zellen)
    unsigned int y_res;                                // Aufloesung in y-Richtung (1/2 Anzahl Zellen)
    unsigned int cell_size;                            // Zellengroesse (Kantenlaenge) in mm

    double* array;                                     // Das Zellenarray mit Distanzwerten in mm (nur fuer positiven Quadranten)
    Tribots::Vec* grad;                                // der Gradient an jeder Position

    double error_outside;                              // Fehlerwert fuer Positionen auserhalb

    void draw_line_segment (Vec, Vec);                 // ein Liniensegment beruecksichtigen
    void draw_arc (Vec, double, Angle, Angle);         // einen Kreisbogen beruecksichtigen
    void draw_dot (Vec);                               // einen Punkt beruecksichtigen
    void update (unsigned int, unsigned int, double);  // setzt den Array-Eintrag (arg1,arg2) auf Wert min(arg3, bisheriger Eintrag)
  };

}


#endif
