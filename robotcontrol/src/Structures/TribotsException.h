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


#ifndef tribots_tribots_exception_h
#define tribots_tribots_exception_h

#include <exception>
#include <string>


namespace Tribots {

  /** Ausnahme fuer Fehler der Tribotsanwednung, allgemein */
  class TribotsException : public std::exception {
  protected:
    std::string what_string;    ///< der string, der bei Aufruf von what() zurueckgeliefert wird
  public:
    /** Konstruktor; Arg1: Beschreibung des Fehlers */
    TribotsException (const char*);
    ~TribotsException () throw () {;}

    /** liefere Fehlerbeschreibung */
    virtual const char* what () throw();
  };

  /** Ausnahme fuer Hardwarefehler, der eventuell bei einem weiteren Versuch
   *  mit dem gleichen Aufruf nicht wieder auftritt. Zum Beispiel: Ausfall
   *  der Kamera (repariert sich selbst), Ausfall der WLAN-Verbindung.
   */
  class HardwareException : public TribotsException {
  protected:
    std::string config_argument;   ///< die fehlerhafte Konfigurationsangabe
  public:    
    /** Konstruktor; Arg1: Bezeichnung fuer fehlende Konfigurationszeile */
    HardwareException (const char*);
    ~HardwareException () throw () {;}
  };

  /** Ausnahme fuer irreparable Fehler der Hardware, die einen Weiterbetrieb des Roboters nicht erlauben,
      z.B. Ausfall von Motoren, Ausfall der Board-Kommunikation, Absturz der Kamera */
  class BadHardwareException : public TribotsException {
  public:
    /** Konstruktor; Arg1: Beschreibung des Fehlers */    
    BadHardwareException (const char*);
    ~BadHardwareException () throw () {;}
  };
  
  /** Ausnahme fuer Fehler aufgrund fehlerhafter Konfigurations-Angaben */
  class InvalidConfigurationException : public TribotsException {
  protected:
    std::string config_argument;   ///< die fehlerhafte Konfigurationsangabe
  public:    
    /** Konstruktor; Arg1: Bezeichnung fuer fehlende Konfigurationszeile */
    InvalidConfigurationException (const char*);
    ~InvalidConfigurationException () throw () {;}
  };



  // hier koennen weitere benoetigte Ausnahmen eingefuegt werden

}

#endif

