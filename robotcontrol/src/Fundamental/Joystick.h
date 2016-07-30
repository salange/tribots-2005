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


#ifndef _Tribots_Joystick_h_
#define _Tribots_Joystick_h_

#include <vector>
#include <stdexcept>
#include <string>


namespace Tribots {

  /** Klasse, um einen Joystick anzubinden, API zu linux/joystick.h */
  class Joystick {
  private:
    int file_descriptor;        ///< File Deskriptor Geraetedatei
    std::vector<double> axis;   ///< Zustand der Achsen (im Intervall [-1,1])
    std::vector<bool> button;   ///< Zustand der Knoepfe; true=gedrueckt
    void update () throw ();    ///< Joystick Events abfragen
  public:
    /** Joystick initialisieren, uebergeben wird der Name der Geraetedatei (z.B. /dev/input/js0) */
    Joystick (const char*) throw (std::invalid_argument, std::bad_alloc);
    /** Destruktor */
    ~Joystick () throw ();
    /** Joystick Typbeschreibung */
    std::string get_type () throw (std::bad_alloc);
    /** Versionsnummer */
    int get_version () throw ();

    /** Joystick abfragen: Achsen */
    const std::vector<double>& get_axis_state () throw ();
    /** Joystick abfragen: Knoepfe */
    const std::vector<bool>& get_button_state () throw ();
  };

}

#endif

