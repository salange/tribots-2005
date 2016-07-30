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


#ifndef _Tribots_AddJoystickPlayer_h_
#define _Tribots_AddJoystickPlayer_h_

#include "JoystickPlayer.h"

namespace Tribots {

  /** Klasse AddJoystickPlayer als zusaetzliche Joystickanbindung zu einer "normalen"
      Playerklasse. D.h. bei GameState::stop_robot werden die Kommandos vom Joystick
      genommen, ansonsten von der normalen Playerklasse. Ausserdem wird Not-Aus am
      Joystick realisiert */
  class AddJoystickPlayer : public JoystickPlayer {
  private:
    PlayerType* the_elementary_player;  ///< die "normale" Playerklasse
    DriveVector remoteCtrDrv;
    unsigned int start_button, stop_button;  // Nummer des Start- und Stop-Knopfes
  public:
    /** Initialisierung wie bei JoystickPlayer; arg2 ist Zeiger auf "normale" Playerklasse */
    AddJoystickPlayer (const ConfigReader&, PlayerType*) throw (InvalidConfigurationException, std::bad_alloc);
    /** Destruktor */
    ~AddJoystickPlayer () throw ();

    /** Abfrage des Joysticks und Berechnung eines DriveVector */
    DriveVector process_drive_vector (Time) throw ();
  };

}

#endif

