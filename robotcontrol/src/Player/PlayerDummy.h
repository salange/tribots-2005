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


#ifndef tribots_player_dummy_h
#define tribots_player_dummy_h

#include "PlayerType.h"
#include "../WorldModel/WorldModel.h"
#include "../Fundamental/ConfigReader.h"

namespace Tribots {

  /** Dummy-Implementierung eines Spielertyps; tut nichts anderes als
      den Null-Fahrtvektor zu erzeugen. Bei Angabe von PlayerDummy::vtrans
      und PlayerDummy::vrot in der Konfigurationsdatei kann alternativ eine 
      Sollausgabe vorgegeben werden */
  class PlayerDummy:public PlayerType {
  private:
    Vec vtrans;
    double vrot;
    double vaux[3];
    DriveVectorMode dv_mode;
  public:
    PlayerDummy (const ConfigReader&) throw ();
    ~PlayerDummy () throw () {;}
    DriveVector process_drive_vector (Time tt) throw ();
  };

}

#endif

