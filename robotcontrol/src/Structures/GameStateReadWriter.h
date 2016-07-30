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


#ifndef Tribots_GameStateReadWriter_h
#define Tribots_GameStateReadWriter_h

#include <iostream>
#include "GameState.h"

namespace Tribots {

  /** Klasse, um GameState zu schreiben */
  class GameStateWriter {
  public:
    /** Konstruktor; Arg=Zielstream */
    GameStateWriter (std::ostream&) throw ();
    /** Destruktor, fuehrt flush aus */
    ~GameStateWriter () throw ();
    /** schreibe ein GameState;
	Arg1: Zeitstempel in ms
	Arg2: eigentlicher GameState */
    void write (unsigned long int, const GameState&) throw ();
  private:
    std::ostream& dest;
  };


  /** inverse Klasse zu GameStateWriter */
  class GameStateReader {
  public:
    /** Konstruktor; Arg=Quellstream */
    GameStateReader (std::istream&) throw ();
    /** liefert in Arg1 den Zeitstempel in ms, zu dem der naechste GameState vorlag;
	Return: true, wenn ueberhaupt ein naechstes Objekt vorliegt, false, wenn Ende
	der Datei erreicht wurde */
    bool next_timestamp (unsigned long int&) const throw ();
    /** lese alle abgespeicherten Objekte mit Zeitstempel bis zu einem Zeitpunkt (arg4);
	Arg1: Zeitstempel (Rueckgabe)
	Arg2: letzter gelesener GameState (Rueckgabe)
	Arg3: Zeitpunkt in ms, bis zu dem Objekte gelesen werden sollen
	Return: true, wenn ueberhaupt ein naechstes Objekt vorliegt, false, wenn Ende
	der Datei erreicht wurde */
    bool read_until (unsigned long int&, GameState&, unsigned long int) throw ();
  private:
    std::istream& src;
    unsigned long int next;
  };

}


#endif
