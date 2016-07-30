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


#ifndef TribotsTools_RemoteRobotState_h
#define TribotsTools_RemoteRobotState_h

#include "../../Structures/RobotLocation.h"
#include "../../Structures/BallLocation.h"
#include "../../Structures/GameState.h"
#include "../../Structures/DriveVector.h"
#include "../../Structures/RobotData.h"
#include "../../Structures/MessageBoard.h"
#include "../../Structures/ObstacleLocation.h"
#include "../../Structures/VisibleObject.h"
#include "../../Structures/FieldGeometry.h"


namespace TribotsTools {

  /** Struktur, um die Teamzugehoerigkeit zu definieren: eigene Seite und Farblable */
  struct DirectionOfPlay {
    int own_half;       ///< +1 = yellow, -1=blue
    int team_color;   ///< +1 = cyan, -1= magenta
  };

  /** Datentyp, um die relevanten Informationen fuer einen Roboter zu verwalten */
  struct RemoteRobotState {
    std::string name;                       ///< Name des Roboters
    std::string ip;                         ///< IP-Adresse
    int port;                               ///< Port fuer Kommunikation

    bool comm_started;                      ///< Kommunikation initiiert?
    bool comm_okay;                         ///< Kommunikation okay?

    Tribots::RobotLocation robot_pos;       ///< Position des Roboters
    Tribots::BallLocation ball_pos;         ///< Position des Balls

    int team;                               ///< +1, wenn zum Tribotsteam gehoert, -1 sonst
    DirectionOfPlay tribots_team;           ///< Orientierung von mir

    bool in_game;                           ///< Roboter aktiviert?
    Tribots::RefereeState refstate;         ///< RefereeState des Roboters
    std::string playertype;                 ///< Spielertyp
    std::string playerrole;                 ///< Spielerrolle

    bool use_joystick;                      ///< Joystick-Steuerung?
    Tribots::DriveVector joy_drive;         ///< Drive-Vector des Joysticks

    bool robot_data_request;                ///< sollen Roboterdaten uebertragen werden?
    Tribots::RobotData robot_data;          ///< die Roboterdaten

    bool obstacle_data_request;             ///< sollen Hindernisse übertragen werden?
    Tribots::ObstacleLocation obstacles;    ///< die Hindernisse

    bool visible_object_request;            ///< sollen gesehenen Objekte übertragen werden?
    Tribots::VisibleObjectList visible_objects; ///< die gesehenen Objekte

    Tribots::MessageBoard mboard;           ///< Messages

    Tribots::FieldGeometry fieldgeometry;   ///< die Feldgeometrie, zu Kontrollzwecken
  };

}


#endif
