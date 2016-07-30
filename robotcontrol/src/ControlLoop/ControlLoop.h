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


#ifndef tribots_control_loop_h
#define tribots_control_loop_h

#include "../Robot/Robot.h"
#include "../Player/Player.h"
#include "../WorldModel/WorldModel.h"
#include "../ImageProcessing/ImageProcessing.h"
#include "../UserInterface/UserInterface.h"


/** Namespace Tribots enthaelt alle neuen Softwareteile fuer die Tribots */
namespace Tribots {

  /** Implementierung der zentralen Rgelschleife
      Aufgaben: Initialisierung der Komponenten, Kontrollfluss, Datenfluss, 
                Zeitkontrolle, Start-/Stopp-Abfrage von Tastatur */
  class ControlLoop {
  private:
    Robot* the_robot;
    Player* the_player;
    WorldModel* the_world_model;
    ImageProcessing* the_image_processing;
    UserInterface* the_user_interface;
    const ConfigReader& configuration_list;

    Time timestamp;           ///< Attribut, um die Schleifendurchlaeufe zu takten
    long int loop_time;       ///< Laenge eines Regelintervalls in ms
    Time first_cycle_time;    ///< Startzeitpunkt der Regelschleife

    bool report_computation_time;              ///< Gesamt-Rechenzeiten erfassen?
    bool report_computation_time_detailed;     ///< Rechenzeit in jeder Iteration erfassen
    bool report_computation_time_gnuplot;      ///< Rechenzeit in jeder Iteration fuer gnuplot erfassen
    unsigned long int usec_image_processing;   ///< bisherige Rechenzeit fuer Bildverarbeitung
    unsigned long int usec_world_model;        ///< bisherige Rechenzeit fuer Welt-Modell (update-Funktion)
    unsigned long int usec_player;             ///< bisherige Rechenzeit fuer Entscheidungsfindung
    unsigned long int usec_robot;              ///< bisherige Rechenzeit fuer Roboteransteuerung
    unsigned long int usec_user_interface;     ///< bisherige Rechenzeit fuer User Interface und Kommunikation
    unsigned long int usec_idle;               ///< bisherige Wartezeit
    unsigned long int num_cycles;              ///< Anzahl Iterationen

  public:
    /** Konstruktor */
    ControlLoop (const ConfigReader&) throw (TribotsException, std::bad_alloc);
    /** Destruktor */
    ~ControlLoop () throw ();
    /** starte Hauptschleife */
    void loop () throw ();
  };

}

#endif

