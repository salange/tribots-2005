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


#ifndef Tribots_RefereeStateMachine_h
#define Tribots_RefereeStateMachine_h

#include "../Structures/GameState.h"
#include "../Structures/FieldGeometry.h"
#include "../Fundamental/Time.h"
#include "../Fundamental/Vec.h"


namespace Tribots {

  /** realisiert den Uebergangsgraphen fuer Refereestates */
  class RefereeStateMachine {
  private:
    int team;                // +1 = cyan, -1=magenta
    RefereeState current;    // aktueller Zustand
    Time latest_update;      // letzte Aktualisierung
    Vec ballpos;             // Ballreferenzposition
    bool ballknown;          // Ballposition bekannt?
    double penalty_marker_y; // y-Position des Penalty-Markers
    double center_circle_radius;  // Mittelkreisradius
  public:
    /** Konstruktor;
	Arg1: Teamfarbe +1 cyan oder -1 magenta
	Arg2: aktueller Zustand 
	Arg3: Geometrie des Feldes */
    RefereeStateMachine (const FieldGeometry&, int =1, RefereeState =stopRobot) throw ();
    /** Copy-Konstruktor */
    RefereeStateMachine (const RefereeStateMachine&) throw ();
    /** Zuweisungsoperator */
    const RefereeStateMachine& operator= (const RefereeStateMachine&) throw ();

    /** RefereeState abfragen */
    RefereeState get_state () const throw ();

    /** RefereeState setzen */
    void set_state (RefereeState) throw ();
    /** Teamfarbe setzen; +1=cyan, -1=magenta */
    void set_team_color (int) throw ();

    /** Signal einarbeiten;
	Arg1: Signal
	Arg2: Ballposition
	Arg3: Ballposition bekannt? (wenn nein, steht in Arg2 irgendwas)
	Return: das Signal in own/opponent-Codierung */
    RefboxSignal update (RefboxSignal, Vec, bool) throw ();
    /** zyklische Aktualisierung wegen 10s-Regel; muss regelmaessig aufgerufen werden;
	Arg1: Ballposition 
	Arg3: Ballposition bekannt? (wenn nein, steht in Arg1 irgendwas) */
    void update (Vec, Vec, bool) throw ();
  };

}

#endif
