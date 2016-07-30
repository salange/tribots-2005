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


#ifndef Tribots_SPhysGotoPos_h
#define Tribots_SPhysGotoPos_h

#include "../CmdGenerator.h"

namespace Tribots {

  /** Faehrt zu einer vorgegebenen Position und haelt dort an.
      Ignoriert Hindernisse.
      Nutzt eine Modellierung mit physikalischem Bewegungsmodell.
      Arbeitet in der Realitaet ordentlich, schwingt im Simulator ueber, 
      vor allem bei Rotationen */
  class SPhysGotoPos : public CmdGenerator {
  public:
    /** Konstruktor, liest dynamische Eigeschaften (Geschwindigkeiten, Beschleunigungen) aus den RobotProperties */
    SPhysGotoPos () throw ();
    /** Fahrtbefehl erzeugen */
    void getCmd(DriveVector*, const Time&) throw();
    /** initialisieren mit:
	Arg1: Zielposition in Weltkoordinaten,
	Arg2: Zielausrichtung
	Arg3: soll Roboter am Ziel anhalten?
	Arg4: sollen kleine Ungenauigkeiten bei der Position toleriert werden?
	Arg5: sollen kleine Ungenauigkeiten bei der Orientierung toleriert werden? */
    void init (Vec, Angle, bool, bool =true, bool =true) throw ();
    /** initialisieren mit:
	Arg1: Zielposition in Weltkoordinaten,
	Arg2: Zielausrichtung
	Arg3: welche Maximalgeschwindigkeit in m/s soll der Roboter am Ziel haben?
	Arg4: sollen kleine Ungenauigkeiten bei der Position toleriert werden?
	Arg5: sollen kleine Ungenauigkeiten bei der Orientierung toleriert werden? */
    void init (Vec, Angle, double, bool =true, bool =true) throw ();
    /** die dynamischen Eigenschaften setzen: 
	Arg1: maximale translatorische Geschwindigkeit in m/s, 
	Arg2: maximale rotationale Geschwindigkeit in rad/s, 
	Arg3: maximale translatorische Beschleunigung in m/s^2,
	Arg4: maximale rotationale Beschleunigung in rad/s^2 */
    void set_dynamics (double, double, double, double) throw ();
    /** die dynamischen Eigenschaften erfragen; Rueckgabe ueber Argumentreferenzen: 
	Arg1: maximale translatorische Geschwindigkeit in m/s, 
	Arg2: maximale rotationale Geschwindigkeit in rad/s, 
	Arg3: maximale translatorische Beschleunigung in m/s^2,
	Arg4: maximale rotationale Beschleunigung in rad/s^2,
	Arg5: maximale translatorische Bremsverzoegerung in m/s^2,
	Arg6: maximale rotationale Bremsverzoegerung in rad/s^2 */
    void get_dynamics (double&, double&, double&, double&, double&, double&) throw ();

  private:
    // Zielwerte:
    Vec target_pos;
    Angle target_heading;
    double max_target_velocity;
    bool tolerance_pos;
    bool tolerance_heading;

    // Dynamische Eigenschaften:
    double max_tv;
    double max_rv;
    double max_ta;
    double max_ra;
    double max_td;
    double max_rd;

    int acc_delay;  // Glaettungszeitraum fuer Beschleunigungszeitraum in ms
  };

}

#endif 
