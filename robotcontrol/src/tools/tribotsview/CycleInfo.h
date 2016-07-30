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


#ifndef TribotsTools_CycleInfo_h
#define TribotsTools_CycleInfo_h

#include <string>
#include <vector>
#include "../../Structures/RobotLocation.h"
#include "../../Structures/BallLocation.h"
#include "../../Structures/ObstacleLocation.h"
#include "../../Structures/VisibleObject.h"
#include "../../Structures/GameState.h"


namespace TribotsTools {

  /** Struktur, um Suchpositionen (z.B. Partikel) der Selbstlokalisation einzutragen */
  struct PossiblePosition {
    Tribots::Vec pos;         // x,y-Koordinaten
    Tribots::Angle heading;   // Orientierung
    double value;             // Guetewert
  };


  /** Struktur, um die Informationen eines Zykluses zu speichern */
  struct CycleInfo {
    unsigned long int cycle_num;                ///< Iteration
    unsigned long int time_msec;                ///< Programmzeit in msec

    std::vector<Tribots::RobotLocation> rloc_vis;   ///< Roboterdaten, ggf. fuer mehrere Roboter, zum Zeitpunkt der Bildinformation
    std::vector<Tribots::BallLocation> bloc_vis;    ///< Balldaten, ggf. fuer mehrere Roboter, zum Zeitpunkt der Bildinformation
    std::vector<Tribots::RobotLocation> rloc_exec;  ///< Roboterdaten, ggf. fuer mehrere Roboter, zum Zeitpunkt der Befehlsausfuehrung
    std::vector<Tribots::BallLocation> bloc_exec;   ///< Balldaten, ggf. fuer mehrere Roboter, zum Zeitpunkt der Befehlsausfuehrung
    Tribots::ObstacleLocation oloc;             ///< Hindenisdaten
    Tribots::VisibleObjectList vloc;            ///< Gesehene Objekte
    Tribots::GameState gs;                      ///< gegenwaertiger GameState
    std::string logmsg;                         ///< Loginfo
    std::string paintmsg;                       ///< Anweisungen, Linien zu zeichnen
    std::vector<PossiblePosition> ppos;         ///< Partikel des Condensation Filters o. ae.
    
    CycleInfo ();
    CycleInfo (const CycleInfo&);
    const CycleInfo& operator= (const CycleInfo&);
  };

}

#endif

