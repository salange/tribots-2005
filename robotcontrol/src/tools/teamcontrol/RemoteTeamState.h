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


#ifndef TribotsTools_RemoteTeamState_h
#define TribotsTools_RemoteTeamState_h

#include "RemoteRobotState.h"
#include <vector>

namespace TribotsTools {

  /** Struktur, die die relevanten Informationen fuer den Trainer zusammenfasst */
  struct RemoteTeamState {
    std::vector<const RemoteRobotState*> robot_states;          ///< die Roboterzustaende
    std::vector<Tribots::MessageBoard*> message_boards;   ///< die MessageBoards fuer jeden Roboter
    Tribots::RefereeState refstate;                                ///< der Spielzustand

    RemoteTeamState (unsigned int n) : robot_states(n), message_boards(n) {;}
  };

}

#endif
