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


#ifndef _Tribots_FileRobot_h_
#define _Tribots_FileRobot_h_

#include "RobotType.h"
#include "../Fundamental/ConfigReader.h"
#include "../Structures/DriveVectorReadWriter.h"

namespace Tribots {

  /** Klass FileRobot liest Odometrie und DriveVector aus Datei */
  class FileRobot : public RobotType {
  public:
    /** Konstruktor, liest Dateinamen aus ConfigReader */
    FileRobot (const ConfigReader&) throw (std::bad_alloc);
    /** Destruktor */
    ~FileRobot () throw ();
    /** liefert RobotProperties aus ConfigReader */
    RobotProperties get_robot_properties () const throw ();
    /** liest Odometrie und DriveVector aus Datei und schreibt sie ins Weltmodell */
    void set_drive_vector (DriveVector) throw ();
  private:
    std::ifstream* odo_stream;
    std::ifstream* drv_stream;
    DriveVectorReader* odo_reader;
    DriveVectorReader* drv_reader;
    RobotProperties robot_properties;
  };

}

#endif
