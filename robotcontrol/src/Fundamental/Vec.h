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


#ifndef Vec_h
#define Vec_h

#include "Angle.h"
#include <iostream>

namespace Tribots {

  /** 2-dimensionaler Vektor */
  class Vec {
  public:
    double x;    ///< Wert in 1. Dimension
    double y;    ///< Wert in 2. Dimension
    
    /** Default-Konstruktor */
    Vec () throw () { x=y=0.;}
    /** Konstruktor zum direkten Setzen der Werte */
    Vec (double x1, double y1) throw () : x(x1), y(y1) {;}
    /** Copy-Konstruktor */
    Vec (const Vec& v) throw () : x(v.x), y(v.y) {;}
    /** Zuweisungsoperator */
    const Vec& operator= (const Vec& v) { x=v.x; y=v.y; return *this; }

    /** Vergleichsoperator */
    bool operator== (const Vec) const throw ();
    bool operator!= (const Vec) const throw ();
    
    /** Addition */
    Vec operator+ (const Vec) const throw ();
    const Vec& operator+= (const Vec) throw ();
    /** Subtraktion */
    Vec operator- (const Vec) const throw ();
    const Vec& operator-= (const Vec) throw ();
    /** Negation */
    Vec operator- () const throw ();
    /** Skalarmultiplikation */
    const Vec& operator*= (double) throw ();
    const Vec& operator/= (double) throw ();       // Division durch Null wird nicht abgefangen, ergibt nan
    /** Skalarprodukt */
    double operator* (const Vec) const throw ();

    /** Rotation um Winkel arg1, identisch mit Methode rotate (.) */
    Vec operator* (const Angle) const throw ();
    /** Rotation von (*this) um Winkel arg, identisch mit Methode s_rotate (.) */
    const Vec& operator*= (const Angle) throw ();
    /** Rotation um Winkel -arg1 */
    Vec operator/ (const Angle) const throw ();
    /** Rotation von (*this) um Winkel -arg1 */
    const Vec& operator/= (const Angle) throw ();

    // Rotationen:
    Vec rotate (const Angle) const throw ();       ///< Rotation um beliebigen Winkel
    Vec rotate_twelvth () const throw ();          ///< Rotation um 30 Grad
    Vec rotate_eleven_twelvth () const throw ();   ///< Rotation um -30=330 Grad
    Vec rotate_eighth () const throw ();           ///< Rotation um 45 Grad
    Vec rotate_seven_eighth () const throw ();     ///< Rotation um -45=315 Grad
    Vec rotate_sixth () const throw ();            ///< Rotation um 60 Grad
    Vec rotate_five_sixth () const throw ();       ///< Rotation um -60=300 Grad
    Vec rotate_quarter () const throw ();          ///< Rotation um 90 Grad
    Vec rotate_three_quarters () const throw ();   ///< Rotation um -90 Grad
    Vec rotate_half () const throw ();             ///< Rotation um 180 Grad=Punktspiegelung am Ursprung

    Vec s_rotate (const Angle) throw ();           ///< Rotation von this
    
    // Spiegelungen:
    Vec mirror (const Vec) const throw ();         ///< Spiegelung an einer Achse mit gegebener Richtung wenn ||Arg1|| > 0, ansonsten Punktspiegelung 
    Vec mirror_x () const throw ();                ///< Spiegelung an x-Achse
    Vec mirror_y () const throw ();                ///< Spiegelung an y-Achse
    Vec mirror_eighth () const throw ();           ///< Spiegelung an 1. Winkelhalbierender
    Vec mirror_three_eighth () const throw ();     ///< Spiegelung an 2. Winkelhalbierender
                 
    /** Quadrierte Laenge des Vektors */
    double squared_length () const throw ();
    /** Laenge des Vektors */
    double length () const throw ();
    /** Winkel des Vektors (0 fuer den Nullvektor) */
    Angle angle () const throw ();
    /** Normalisieren ((0,0) beim Nullvektor) */
    Vec normalize () const throw ();
    
    /** Winkel zwischen zwei Vektoren (nan bei Nullvektor) */
    Angle angle (const Vec) const throw ();
    
    /** liefert einen Vektor der Laenge 1 in gegebener Richtung */
    static Vec unit_vector (Angle) throw ();

    static const Vec unit_vector_x;           ///< Einheitsvektor in x-Richtung
    static const Vec unit_vector_y;           ///< Einheitsvektor in y-Richtung
    static const Vec zero_vector;             ///< Nullvektor
    
  };

  /** Skalarmultiplikation */
  Vec operator* (Vec, double) throw ();
  Vec operator* (double, Vec) throw ();
  Vec operator/ (Vec, double) throw ();    // Division durch 0 wird nicht abgefangen, ergibt nan

  /** Lineare Unabhaengigkeit */
  bool linearly_independent (const Vec, const Vec);

  
}    

std::ostream& operator<< (std::ostream& os, const Tribots::Vec& v);

#endif
  

