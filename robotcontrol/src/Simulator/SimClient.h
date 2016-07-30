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


#ifndef _Tribots_SimClient_h_
#define _Tribots_SimClient_h_

#include "../Fundamental/Time.h"
#include "../Fundamental/Vec.h"
#include "../Structures/DriveVector.h"
#include <stdexcept>
#include <vector>

//#define USE_ODESIM 1


#if USE_ODESIM
#include "odeserver/odesimcomm.h"
using namespace OdeServer;
#endif

namespace Tribots {

  /** Klasse als Simulatoranbindung, kommuniziert mit dem Simulator */
  class SimClient {
  public:
    /** Aufrufroutine, Singleton; Arg1=Simulator-Konfigurationsdatei */
    static SimClient* get_sim_client (const char*) throw (std::bad_alloc, std::invalid_argument);
    /** entfernt ein ggf. vorhandenes Objekt vom Typ SimClient */
    static void delete_sim_client () throw ();

  private:
    static SimClient* the_only_sim_client;
    
#if USE_ODESIM
    OdeSimComm * odesimcomm;
#endif    
    /** Konstruktor, privat wegen Singleton */
    SimClient (const char*) throw (std::bad_alloc, std::invalid_argument);
    /** Destruktor */
    ~SimClient () throw ();
  public:

    /** setzen eines Fahrtvektors */
    void set_drive_vector (DriveVector) throw ();
    /** Kommunizieren mit dem Simulator und Aktualisierung der Weltbeschreibung */
    void update () throw ();

    Time timestamp;        ///< Zeitstempel fuer nachfolgende Attribute
    Vec robot_position;    ///< Roboterposition; Koordinatensystem wie bei Spiel blaues->gelbes Tor
    Angle robot_heading;   ///< Roboterausrichtung; Koordinatensystem wie bei Spiel blaues->gelbes Tor
    Vec ball_position;     ///< Ballposition; Koordinatensystem wie bei Spiel blaues->gelbes Tor
    std::vector<Vec> obstacle_positions;  ///< Hindernispositionen

  };

}

#endif


