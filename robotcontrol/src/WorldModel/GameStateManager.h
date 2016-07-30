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


#ifndef Tribots_GameStateManager_h
#define Tribots_GameStateManager_h

#include "../Structures/GameState.h"
#include "../Structures/BallLocation.h"
#include "../Structures/FieldGeometry.h"
#include "../Fundamental/ConfigReader.h"
#include "RefereeStateMachine.h"


namespace Tribots {

  /** Klasse, um die Verwaltung der Gamestates */
  class GameStateManager {
  private:
    GameState gs;                ///< aktueller Gamestate
    BallLocation ref_ball;       ///< Referenzposition des Balls
    RefereeStateMachine rsm;     ///< der Uebergangsautomat fuer RefereeStates

  public:
    /** Konstruktor, setzt GameState */
    GameStateManager (const ConfigReader&, const FieldGeometry&) throw ();
    /** Destruktor */
    ~GameStateManager() throw ();
    /** GameState abfragen */
    const GameState& get () const throw ();
    /** Aktualisieren (10-Sekunden-Regeln, Ballbewegung) */
    void update () throw ();
    /** Signal der Refereebox einarbeiten */
    void update (RefboxSignal) throw ();
    /** Ball in das Spiel (true) oder aus dem Spiel (false) nehmen */
    void set_in_game (bool) throw ();
    /** Mitteilung des Beginns eines neuen Zyklus
	Arg1: Startzeitpunkt fuer neuen Zyklus
	Arg2: Erwartete Ausfuehrungszeit fuer Fahrtbefehl in naechstem Zyklus */
    void init_cycle (Time, Time) throw ();
  };

}


#endif
