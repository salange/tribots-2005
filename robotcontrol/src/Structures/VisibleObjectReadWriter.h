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


#ifndef Tribots_VisibleObjectReadWriter_h
#define Tribots_VisibleObjectReadWriter_h

#include <iostream>
#include "VisibleObject.h"

namespace Tribots {

  /** Klasse, um VisibleObject und VisibleObjectList zu schreiben */
  class VisibleObjectWriter {
  public:
    /** Konstruktor; Arg=Zielstream */
    VisibleObjectWriter (std::ostream&) throw ();
    /** Destruktor, fuehrt flush aus */
    ~VisibleObjectWriter () throw ();
    /** schreibe ein VisibleObject;
	Arg1: Zeitstempel in ms, zu der VisibleObject vorlag
	Arg2: Zeitstempel in ms, zu der Bild aufgenommen wurde
	Arg3: erkanntes VisibleObject */
    void write (unsigned long int, unsigned long int, const VisibleObject&) throw ();
    /** schreibe eine VisibleObjectList;
	Arg1: Zeitstempel in ms, zu der VisibleObject vorlag
	Arg2: Zeitstempel in ms, zu der Bild aufgenommen wurde
	Arg3: erkannte VisibleObjectList; Zeitstempel der VisibleObjectList wird ignoriert */
    void write (unsigned long int, unsigned long int, const VisibleObjectList&) throw ();
  private:
    std::ostream& dest;
  };


  /** inverse Klasse zu VisibleObjectWriter */
  class VisibleObjectReader {
  public:
    /** Konstruktor; Arg=Quellstream */
    VisibleObjectReader (std::istream&) throw ();
    /** liefert in Arg1 den Zeitstempel in ms, zu dem das naechste VisibleObject vorlag;
	Return: true, wenn ueberhaupt ein naechstes Objekt vorliegt, false, wenn Ende
	der Datei erreicht wurde */
    bool next_timestamp (unsigned long int&) const throw ();
    /** lese alle abgespeicherten Objekte mit Zeitstempel bis zu einem Zeitpunkt (arg4);
	Arg1: Zeitstempel in ms des Vorliegens des VisibleObject (Rueckgabe)
	Arg2: Zeitstempel in ms des Kamerabildes (Rueckgabe)
	Arg3: Liste der erkannten Objekte (Rueckgabe)
	Arg4: Zeitpunkt in ms, bis zu dem Objekte gelesen werden sollen. 
	Massgeblich ist der Zeitpunkt des Vorliegens der Information
	Return: wenn ueberhaupt ein naechstes Objekt vorliegt, false, wenn Ende
	der Datei erreicht wurde */
    bool read_until (unsigned long int&, unsigned long int&, VisibleObjectList&, unsigned long int) throw ();
  private:
    std::istream& src;
    unsigned long int next;
  };

}


#endif
