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


#ifndef tribots_player_h
#define tribots_player_h

#include <stdexcept>
#include "PlayerType.h"
#include "../Structures/TribotsException.h"
#include "../Fundamental/ConfigReader.h"


namespace Tribots {

  /** Schnittstelle der Planungs- und Entscheidungskomponente nach aussen
      Aufgaben: Berechnen eines Fahrtvektors, Verwaltung verschiedener Spielertypen */
  class Player {
  private:
    PlayerType* the_player;
    char* player_descriptor;
    const ConfigReader& configuration_list;

    /** eigentliche Implementierung des Spielerwechsels */
    void really_change_player_type (const char*, const ConfigReader&) throw (TribotsException, std::bad_alloc);
  public:
    /** Konstruktor
	Arg1: ConfigReader, aus dem alle relevanten Parameter (Spielertyp, Einsatzbereich, usw.) ausgelesen werden 
	Arg2: Beschreibung der Robotereigenschaften (Geschwindigkeit, Beschleunigung, usw.) */
    Player (const ConfigReader&) throw (TribotsException, std::bad_alloc);
    ~Player () throw ();


    /** Wechsel des Spielertyps
	Spielertyp-Parameter werden aus dem ConfigReader gelesen, der mit dem Konstruktor uebergeben wurde
	Arg1: Bezeichner fuer Spielertyp
	Ret: bei Erfolg, true */
    bool change_player_type (const char*) throw ();
    /** Wechsel des Spielertyps 
	Arg1: Bezeichner fuer Spielertyp 
	Arg2: Parameterliste fuer neuen Spielertyp 
	Ret: bei Erfolg, true */
    bool change_player_type (const char*, const ConfigReader&) throw ();

    /** liefere eine Beschreibung des aktuellen Spielertyps */
    const char* get_player_type () const throw ();

    void getPlayerTypeList(std::vector<std::string> &ptl);

    /** berechne einen Fahr- und Kickbefehl
	Arg1: Zeitpunkt, zu dem der Fahrtvektor vsl. gesetzt wird
	Ret: gewuenschter Fahr- und Kickbefehl (Geschwindigkeiten) 
        Seiteneffekte: greift auf die Informationen des Weltmodells zu (sollte nur lesender Zugriff sein) */
    inline DriveVector process_drive_vector (Time t) throw () { return (the_player->process_drive_vector (t)); }
  };

}

#endif

