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


#ifndef Tribots_DriveVectorReadWriter_h
#define Tribots_DriveVectorReadWriter_h

#include <iostream>
#include "DriveVector.h"

namespace Tribots {

  /** Klasse, um DriveVector zu schreiben */
  class DriveVectorWriter {
  public:
    /** Konstruktor; Arg=Zielstream */
    DriveVectorWriter (std::ostream&) throw ();
    /** Destruktor, fuehrt flush aus */
    ~DriveVectorWriter () throw ();
    /** schreibe ein DriveVector;
	Arg1: Zeitstempel in ms, zu der DriveVector vorlag
	Arg2: Zeitstempel in ms, zu der DriveVector ausgefuehrt wird/zu dem Odometrie gemessen wurde
	Arg3: eigentlicher DriveVector */
    void write (unsigned long int, unsigned long int, const DriveVector&) throw ();
  private:
    std::ostream& dest;
  };


  /** inverse Klasse zu DriveVectorWriter */
  class DriveVectorReader {
  public:
    /** Konstruktor; Arg=Quellstream */
    DriveVectorReader (std::istream&) throw ();
    /** liefert in Arg1 den Zeitstempel in ms, zu dem das naechste DriveVector vorlag;
	Return: true, wenn ueberhaupt ein naechstes Objekt vorliegt, false, wenn Ende
	der Datei erreicht wurde */
    bool next_timestamp (unsigned long int&) const throw ();
    /** lese alle abgespeicherten Objekte mit Zeitstempel bis zu einem Zeitpunkt (arg4);
	Arg1: Zeitstempel in ms des Vorliegens des DriveVector (Rueckgabe)
	Arg2: Zeitstempel in ms der Ausfuehrung/Odometriemessung (Rueckgabe)
	Arg3: letzter gelesener DriveVector (Rueckgabe)
	Arg4: Zeitpunkt in ms, bis zu dem Objekte gelesen werden sollen. 
	Massgeblich ist der Zeitpunkt des Vorliegens der Information
	Return: wenn ueberhaupt ein naechstes Objekt vorliegt, false, wenn Ende
	der Datei erreicht wurde */
    bool read_until (unsigned long int&, unsigned long int&, DriveVector&, unsigned long int) throw ();
  private:
    std::istream& src;
    unsigned long int next;
  };

}


#endif
