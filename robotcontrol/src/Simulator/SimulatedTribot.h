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


#ifndef _Tribots_SimulatedTribot_h_
#define _Tribots_SimulatedTribot_h_

#include "../Robot/RobotType.h"
#include "../Fundamental/ConfigReader.h"
#include "../Fundamental/RingBuffer.h"
#include "SimClient.h"


namespace Tribots {

  /** Klasse implementiert die Ansteuerungs-Seite der Simulatoranbindung */
  class SimulatedTribot : public RobotType {
  private:
    struct TDriveVector {
      Time timestamp;
      DriveVector dv;
    };
    SimClient* the_sim_client;           ///< Pointer auf den SimClient
    RobotProperties robot_properties;    ///< die Robotereigenschaften
    RingBuffer<TDriveVector> drv;       ///< zwischengespeicherte DriveVectoren, um Delay zu simulieren
  public:
    SimulatedTribot (const ConfigReader&) throw (std::bad_alloc, Tribots::InvalidConfigurationException);
    ~SimulatedTribot () throw ();

    /** liefere physikalische Eigenschaften des Roboters (Geschwindigkeit, Beschleunigung) */
    RobotProperties get_robot_properties () const throw ();

    /** setze einen Fahr- und Kickbefehl
        Arg1: gewuenschter Fahrbefehl
        Seiteneffekte: Roboter gibt gesetzten Fahrbefehl sowie Odometrie an Weltmodell weiter */
    void set_drive_vector (DriveVector) throw ();
  };

}

#endif


