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


#ifndef TribotsTools_CycleContainer_h
#define TribotsTools_CycleContainer_h

#include <fstream>
#include <stdexcept>
#include <deque>
#include "CycleInfo.h"
#include "../../Structures/RobotLocationReadWriter.h"
#include "../../Structures/BallLocationReadWriter.h"
#include "../../Structures/ObstacleLocationReadWriter.h"
#include "../../Structures/GameStateReadWriter.h"
#include "../../Structures/VisibleObjectReadWriter.h"

namespace TribotsTools {

  /** Klasse, um die CycleInfo aus Dateien zu lesen und zu verwalten */
  class CycleContainer {
  private:
    std::ifstream* rloc_stream;              // Eingabestream fuer Roboterposition
    std::ifstream* bloc_stream;              // Eingabestream fuer Ballposition
    std::ifstream* oloc_stream;              // Eingabestream fuer Hindernisse
    std::ifstream* vloc_stream;              // Eingabestream fuer gesehene Objekte
    std::ifstream* log_stream;               // Eingabestream fuer Loginfo
    std::ifstream* ppos_stream;              // Eingabestream fuer Suchpositionen der Selbstlokalisierung
    std::ifstream* gs_stream;                // Eingabestream fuer Gamestates

    unsigned int next_log_timestamp;         // naechster gelesener Timestamp fuer .log-Datei
    unsigned int next_ppos_timestamp;        // naechster gelesener Timestamp fuer Selbstlokalisierungs-Suchpositionen

    Tribots::RobotLocationReader* rloc_reader;
    Tribots::BallLocationReader* bloc_reader;
    Tribots::ObstacleLocationReader* oloc_reader;
    Tribots::VisibleObjectReader* vloc_reader;
    Tribots::GameStateReader* gs_reader;

    std::deque<CycleInfo> cycle_queue;       // bisher gelesene Infos
    unsigned long int current_cycle;         // Nummer des aktuellen Zyklus
    std::string fname_praefix;               // Dateinamen-Praefix

    bool read_next (CycleInfo&, unsigned long int) throw ();    // liest naechste Zyklen bis Nr arg2 von Datei; liefert bei Erfolg true

  public:
    /** Konstruktor, uebergeben wird der Basisname der Logdateien */
    CycleContainer (const char*) throw (std::invalid_argument);
    /** liefere die aktuelle Zyklusnummer */
    long int cycle_num () const throw ();
    /** weiterschalten um arg1 Iteration, falls moeglich 
        aktuelle Iterationsnummer wird zurueckgegeben */
    long int step (long int =1) throw ();
    /** einen bestimmten Zyklus aufrufen, falls moeglich 
        aktuelle Iterationsnummer wird zurueckgegeben */
    long int set (unsigned long int) throw ();
    /** eine bestimmte Programmzeit aufrufen, falls moeglich 
        aktuelle Iterationsnummer wird zurueckgegeben */
    long int set_time (unsigned long int) throw ();
    /** Infos der aktuellen Iteration holen, voausgesetzt es sind Infos da */
    const CycleInfo& get () const throw ();
    /** liefere Anzahl gespeicherter Zyklen */
    unsigned int size() const throw ();
    /** Datei neu laden */
    bool revert ();
    /** Datei neu laden */
    bool revert (const char*);
    /** aktuelle Zyklusinfo ersetzen */
    void replace (const CycleInfo&);
  protected:
    /** Logdateien oeffnen, liefert true bei Erfolg */
    bool open_files (const char* praefix);
    /** Logdateien schliessen */
    void close_files ();
  };

}

#endif

