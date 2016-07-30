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


#ifndef _Tribots_JoystickPlayer_h_
#define _Tribots_JoystickPlayer_h_

#include <stdexcept>
#include "PlayerType.h"
#include "PhysicalMotionModel.h"
#include "../Fundamental/ConfigReader.h"
#include "../Fundamental/Joystick.h"
#include "../Structures/TribotsException.h"


namespace Tribots {

  /** Joystickanbindung, realisiert als Spielertyp */
  class JoystickPlayer : public PlayerType {
  protected:
    Joystick* js;
    PhysicalMotionModel motion;
  private:
    double max_velocity;
    double max_rot_velocity;
    bool previously_changed;

    unsigned int x_axis, y_axis, phi_axis;     // Nummer der Joystick-Achse, um x,y,phi-Bewegung zu steuern
    double x_diraxis, y_diraxis, phi_diraxis;     // Orientierung der Joystick-Achsen (wo ist +/-)
    unsigned int kick_button, accel_button, decel_button;  // Nummer der Knoepfe, um zu kicken, Geschwindigkeit erhoehen/verringern
  public:
    /** Initialisierung, Name des Devices wird aus dem ConfigReader entnommen */
    JoystickPlayer (const ConfigReader&) throw (InvalidConfigurationException, std::bad_alloc);
    /** Destruktor */
    ~JoystickPlayer () throw ();

    /** Abfrage des Joysticks und Berechnung eines DriveVector */
    DriveVector process_drive_vector (Time) throw ();
  };

}

#endif

