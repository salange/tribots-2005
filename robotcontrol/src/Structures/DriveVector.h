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


#ifndef tribots_drive_vector_h
#define tribots_drive_vector_h

#include "../Fundamental/Vec.h"

namespace Tribots {

  enum DriveVectorMode {ROBOTVELOCITY, WHEELVELOCITY, MOTORVOLTAGE};

  /** Struktur, um einen Fahr- und Kickbefehl darstellen und kommunizieren zu koennen; 
      alle Angaben in ROBOTERKOORDINATEN */
  struct DriveVector {
    /** Default-Konstruktor */
    DriveVector () throw ();
    /** Konstrukor
	Arg1: vtrans
	Arg2: vrot
	Arg3: Schusserlaubnis */
    DriveVector (Vec, double =0, bool =false) throw();
    /** Konstrukor
	Arg1: vtrans.x (ROBOTVELOCITY), wheel vel 1 (WHEELVELOCITY), motor voltage 1 (MOTORVOLTAGE)
	Arg2: vtrans.y (ROBOTVELOCITY), wheel vel 2 (WHEELVELOCITY), motor voltage 2 (MOTORVOLTAGE)
	Arg3: vrot     (ROBOTVELOCITY), wheel vel 3 (WHEELVELOCITY), motor voltage 3 (MOTORVOLTAGE)
	Arg4: Kickpermission
	Arg5: mode (ROBOTVELOCITY, WHEELVELOCITY, MOTORVOLTAGE)**/
    DriveVector (double, double, double, bool =false, DriveVectorMode =ROBOTVELOCITY);

    /** Copy-Konstruktor */
    DriveVector (const DriveVector&) throw ();
    /** Zuweisungsoperator */
    const DriveVector& operator= (const DriveVector&) throw ();
    
   
    Vec vtrans;    ///< translatorische Geschwindigkeit in m/s (im Roboterkoordinatensystem)
    double vrot;   ///< rotatorische Geschwindigkeit im Gegenuhrzeigersinn in rad/s (modus: ROBOTVELOCITY)
    bool kick;     ///< Schussanforderung: ausloesen=true, nicht ausloesen=false
    DriveVectorMode mode;  ///< Modus, der beschreibt welche Daten ausgewertet werden und welche Bedeutung diese haben
    double vaux[3]; ///< Radgeschwindigkeiten oder Motorspannungen  (WHEELVELOCITY, MOTORVOLTAGE)
  };

}

#endif

