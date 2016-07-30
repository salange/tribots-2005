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


#ifndef _Tribots_AddGotoPosPlayer_h_
#define _Tribots_AddGotoPosPlayer_h_

#include "PlayerType.h"
#include "../Fundamental/ConfigReader.h"
#include "../Behavior/Skills/SPhysGotoPos.h"
#include "../Structures/GameState.h"

namespace Tribots {

  /** Klasse AddGotoPosPlayer als zusaetzlicher Player zu einer "normalen"
      Playerklasse. D.h., wird ueber das MessageBoard ein GotoPos-Signal
      gesendet, so uebernimmt dieser Spieler die Kontrolle und faehrt zur
      angegebenen Position. Anschliessend wartet er bis zum naechsten
      Wechsel des Referee-States */
  class AddGotoPosPlayer : public PlayerType {
  private:
    PlayerType* the_elementary_player;             // die "normale" Playerklasse
    bool is_active;                                // GotoPos aktiv? (oder richtiger Spieler?)
    RefereeState latest_refstate;                  // letzter Refereestate
    SPhysGotoPos goto_pos_skill;                   // Skill, um zu einer Position zu fahren
  public:
    /** Initialisierung wie bei GotoPosPlayer; arg2 ist Zeiger auf "normale" Playerklasse */
    AddGotoPosPlayer (const ConfigReader&, PlayerType*) throw (InvalidConfigurationException, std::bad_alloc);
    /** Destruktor */
    ~AddGotoPosPlayer () throw ();

    /** Berechnung eines DriveVector */
    DriveVector process_drive_vector (Time) throw ();
  };

}

#endif

