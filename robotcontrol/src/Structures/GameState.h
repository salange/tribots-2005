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


#ifndef Tribots_GameState_h
#define Tribots_GameState_h

#include "../Fundamental/Time.h"

namespace Tribots {

  /** Liste der moeglichen Spielphasen */
  enum RefereeState { 
    stopRobot,               ///< stop, kein Roboter bewegt sich
    freePlay,                ///< freies Spiel, keine Einschraenkungen
    preOwnKickOff,           ///< vor dem Anstoss, Roboter positionieren sich
    preOpponentKickOff,      ///< vor dem Anstoss, Roboter positionieren sich
    postOpponentKickOff,     ///< nach der Ballfreigabe fuer den Gegner, eigenes Team darf Ball noch nicht beruehren
    preOwnGoalKick,          ///< vor dem Torabschlag
    preOpponentGoalKick,     ///< vor dem Torabschlag
    postOpponentGoalKick,    ///< nach der Ballfreigabe fuer den Gegner, eigenes Team darf noch nicht in den gegnerischen Strafraum
    preOwnCornerKick,        ///< vor dem Eckball
    preOpponentCornerKick,   ///< vor dem Eckball
    postOpponentCornerKick,  ///< nach der Ballfreigabe fuer den Gegner, eigenes Team muss 2m Abstand zum Ball halten
    preOwnThrowIn,           ///< vor dem Ein"wurf"
    preOpponentThrowIn,      ///< vor dem Ein"wurf"
    postOpponentThrowIn,     ///< nach der Ballfreigabe fuer den Gegner, eigenes Team darf Ball noch nicht beruehren
    preOwnFreeKick,          ///< vor dem Freistoss
    preOpponentFreeKick,     ///< vor dem Freistoss
    postOpponentFreeKick,    ///< nach der Ballfreigabe fuer den Gegner, eigenes Team muss 2m Abstand zum Ball halten
    preOwnPenalty,           ///< vor dem Strafstoss
    preOpponentPenalty,      ///< vor dem Strafstoss
    postOpponentPenalty,     ///< nach der Ballfreigabe fuer den Gegner, Torwart darf noch nicht an den Ball
    ownPenalty,              ///< Strafstossdurchfuehrung des Gegners
    opponentPenalty,         ///< Strafstossdurchfuehrung des eigenen Teams
    errorState               ///< irgendein Fehlerzustand
  };

  
  /** Liste der moeglichen Refereebox-Signale */
  enum RefboxSignal {
    SIGnop,                  ///< ohne Bedeutung, veraendert nichts 
    SIGstop,
    SIGhalt,
    SIGstart,
    SIGready,
    SIGcyanKickOff,
    SIGmagentaKickOff,
    SIGownKickOff,
    SIGopponentKickOff,
    SIGcyanFreeKick,
    SIGmagentaFreeKick,
    SIGownFreeKick,
    SIGopponentFreeKick,
    SIGcyanGoalKick,
    SIGmagentaGoalKick,
    SIGownGoalKick,
    SIGopponentGoalKick,
    SIGcyanCornerKick,
    SIGmagentaCornerKick,
    SIGownCornerKick,
    SIGopponentCornerKick,
    SIGcyanThrowIn,
    SIGmagentaThrowIn,
    SIGownThrowIn,
    SIGopponentThrowIn,
    SIGcyanPenalty,
    SIGmagentaPenalty,
    SIGownPenalty,
    SIGopponentPenalty,
    SIGcyanGoalScored,
    SIGmagentaGoalScored,
    SIGownGoalScored,
    SIGopponentGoalScored
  };

    
  /** Struktur, um die momentane Spielphase zu erfassen */
  struct GameState {
    RefereeState refstate;           ///< aktueller Refereestate
    Time latest_update;              ///< zeitpunkt der letzten Aktualisierung von refstate
    bool in_game;                    ///< true, wenn der Roboter im Spiel ist, false sonst
    RefboxSignal refbox_signal;      ///< das zuletzt empfangene Signal der Refereebox

    unsigned int own_score;          ///< Torerfolge eignes Team
    unsigned int opponent_score;     ///< Torerfolge gegnerisches Team

    double intended_cycle_time;      ///< vorgesehene Zykluszeit
    double actual_cycle_time;        ///< tatsaechliche Zykluszeit
    unsigned long int cycle_num;     ///< Zyklusnummer
    Time cycle_start_time;           ///< Zeit, zu der Zyklus gestartet
    Time expected_execution_time;    ///< Zeit, zu der die Umsetzung des Fahrtbefehls erwartet wird
  };


  static const int num_referee_states = 23;                  ///< Anzahl moeglicher Spielsituationen
  static const char referee_state_names [23][23] = {         ///< Bezeichner (strings) der verschiedenen Spielsituationen
    "Stop                  ",
    "Freies Spiel          ",
    "vor eigenem Anstoss   ",
    "vor Anstoss Gegner    ",
    "nach Anstoss Gegner   ",
    "vor eigenem Torabstoss",
    "vor Torabstoss Gegner ",
    "nach Torabstoss Gegner",
    "vor eigenem Eckball   ",
    "vor Eckball Gegner    ",
    "nach Eckball Gegner   ",
    "vor eigenem Einwurf   ",
    "vor Einwurf Gegner    ",
    "nach Einwurf Gegner   ",
    "vor eigenem Freistoss ",
    "vor Freistoss Gegner  ",
    "nach Freistoss Gegner ",
    "vor eigenem Strafstoss",
    "vor Strafstoss Gegner ",
    "nach Strafstoss Gegner",
    "eigener Strafstoss    ",
    "Strafstoss Gegner     ",
    "Fehler                "
  };
  

  static const int num_refbox_signals = 33;                  ///< Anzahl moeglicher Signale
  static const char refbox_signal_names [33][23] = {         ///< Bezeichner (strings) der verschiedenen Signale
    "SIGnop                ",
    "SIGstop               ",
    "SIGhalt               ",
    "SIGstart              ",
    "SIGready              ",
    "SIGcyanKickOff        ",
    "SIGmagentaKickOff     ",
    "SIGownKickOff         ",
    "SIGopponentKickOff    ",
    "SIGcyanFreeKick       ",
    "SIGmagentaFreeKick    ",
    "SIGownFreeKick        ",
    "SIGopponentFreeKick   ",
    "SIGcyanGoalKick       ",
    "SIGmagentaGoalKick    ",
    "SIGownGoalKick        ",
    "SIGopponentGoalKick   ",
    "SIGcyanCornerKick     ",
    "SIGmagentaCornerKick  ",
    "SIGownCornerKick      ",
    "SIGopponentCornerKick ",
    "SIGcyanThrowIn        ",
    "SIGmagentaThrowIn     ",
    "SIGownThrowIn         ",
    "SIGopponentThrowIn    ",
    "SIGcyanPenalty        ",
    "SIGmagentaPenalty     ",
    "SIGownPenalty         ",
    "SIGopponentPenalty    ",
    "SIGcyanGoalScored     ",
    "SIGmagentaGoalScored  ",
    "SIGownGoalScored      ",
    "SIGopponentGoalScored "
  };

}

#endif

