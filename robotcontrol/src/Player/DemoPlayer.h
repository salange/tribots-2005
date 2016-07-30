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


#ifndef tribots_DemoPlayer_h
#define tribots_DemoPlayer_h

#include "PlayerType.h"
#include "../WorldModel/WorldModel.h"
#include "../Fundamental/ConfigReader.h"

namespace Tribots {

  /** SpieleKlasse die einige Möglichkeiten wie funmove bietet*/
  class DemoPlayer:public PlayerType {
  private:
    Vec targetParkPosition; 
    bool parkpositionfound;
    
  public:
    DemoPlayer (const ConfigReader&) throw ();
    ~DemoPlayer () throw () {;}
    DriveVector process_drive_vector (Time tt) throw ();
  };

}

#endif

