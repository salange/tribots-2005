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


#ifndef Tribots_ErrorMinimiserSL_h
#define Tribots_ErrorMinimiserSL_h

#include "FieldLUT.h"
#include "OdometryContainer.h"
#include "VisualPositionOptimiser.h"
#include "RobotPositionKalmanFilter.h"
#include "../Structures/VisibleObject.h"
#include "../Structures/RobotLocation.h"
#include "../Fundamental/ConfigReader.h"
#include <fstream>


namespace Tribots {

  // ACHTUNG: Filter verwendet ein eigenes Koordinatensystem unabhaengig von der Spielrichtung.
  //          Ursprung ist der Spielfeldmittelpunkt
  //          die positive y-Achse weist in Richtung des blauen Tores

  /** Selbstlokalisierung mit Hilfe von (a) einem Fehlerminimierungsansatz auf der Bildinformation,
      (b) einem Kalman-Filter zum Verfolgen der Position sowie (c) expliziten Alternativpositionen */
  class ErrorMinimiserSL {
  private:
    /** Struktur, um alternative Positionen zu verwalten */
    struct AltPos {
      AltPos ();                               // Zufallsinitialisierung
      AltPos (Vec);                            // Position gegeben, usrichtung zufaellig
      AltPos (Vec, Angle);                     // Position und Orientierung gegeben
      RobotPositionKalmanFilter rpos_filter;   // Kalman-Filter fuer Roboterposition und Geschwindigkeit
      Time init_time;                          // Zeitpunkt der Erzeugung
      double winning_coefficient;              // Zahl, die ein Mass fuer die Qualitaet der Alternative, indem verglichen wird, ob die Alternative besser als andere ist
      double latest_error;                     // visueller Fehler bei der letzten Iteration
      bool heading_checked;                    // true, wenn die Tor-Orientierung bereits ueberprueft wurde
      double ema_right_goal;                   // EMA-Kriterium, das die Tor-Ausrichtung bestaetigt
      double ema_wrong_goal;                   // EMA-Kriterium, das die Tor-Ausrichtung widerlegt
    };

    FieldLUT* field_lut;                       // Die Abstandstabelle
    VisualPositionOptimiser* vis_optimiser;    // Optimierungsroutine fuer Sensorinformation
    const OdometryContainer& odobox;           // Odometrieinformation
    std::ofstream* plog;                       // fuer debug-Ausgabe, erzeugt .ppos-Datei mit Odometrie-, Bild- und fusionierter Position
    Time latest_update;                        // Zeitstempel der letzten Aktualisierung
    unsigned int cycles_since_reset;           // Anzahl Zyklen seit dem letzten Reset

    unsigned int max_lines;                    // maximale Anzahl zu beruecksichtigender Liniensegmente

    bool consider_yellow_goal;                 // gelbes Tor beruecksichtigen?
    bool consider_blue_goal;                   // blaues Tor beruecksichtigen?
    bool use_internal_alternatives;            // fuer den Hauptfilter intern vier Alternativpositionen berechnen?

    AltPos main_position;                      // derzeitige Positions-Hypothese
    unsigned int num_external_alternatives;    // Anzahl Alternativhypothesen
    std::vector<AltPos> external_alternatives; // Array mit Alternativpositionen

    void update_alternative (AltPos&, const VisibleObjectList&, const VisibleObjectList&, bool, bool);   // Fortschreiben einer Position (arg1) mit Liniensegmenten (arg2) und Toren (arg3). arg4 gibt an, ob interne Alternativen verwendet werden sollen, arg5 ist true, wenn es sich um die Hauptalternative handelt, sonst false

  public:
    /** Konstruktor */
    ErrorMinimiserSL (const ConfigReader&, const OdometryContainer&, const FieldGeometry&) throw (std::bad_alloc);
    /** Destruktor */
    ~ErrorMinimiserSL () throw ();

    /** Position fortschreiben und mit Bildinformation abgleichen;
	arg1: erkannte weise Linien
	arg2: erkannte Tore 
	ret: wurde die interne Repraesentation tatsaechlich veraendert, d.h. wurde Bildinformation eingearbeitet? */
    bool update (const VisibleObjectList&, const VisibleObjectList&) throw ();

    /** Roboterposition und -geschwindigkeit fuer den Zeitpunkt arg1 prognostizieren; 
	Qualitaetswert bezieht sich nur auf die Qualitaet des letzten Sensorintegrationsschritts 
	Angaben im SL-KOORDINATENSYSTEM */
    RobotLocation get (Time) const throw ();

    /** Roboterposition zufaellig reinitialisieren */
    void reset () throw ();
    /** Roboterposition an Position arg1 reinitialisieren */
    void reset (Vec) throw ();
    /** Roboterposition an Position arg1 mit Ausrichtung arg2 reinitialisieren; 
	arg2 misst die Verdrehung des Roboterkoordinatensystems vom Weltkoordinatensystem */
    void reset (Vec, Angle) throw ();
  };

}

#endif
