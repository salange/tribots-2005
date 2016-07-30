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


#include "PlayerDummy.h"
#include "PlayerFactory.h"
#include "../WorldModel/WorldModel.h"
#include "../Structures/GameState.h"

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public PlayerBuilder {
    static Builder the_builder;
  public:
    Builder () {
      PlayerFactory::get_player_factory ()->sign_up (string("Dummy"), this);
    }
    PlayerType* get_player (const std::string&, const ConfigReader& reader, PlayerType*) throw (TribotsException,bad_alloc) {
      return new PlayerDummy (reader);
    }
  };
  Builder the_builder;
}




PlayerDummy::PlayerDummy (const ConfigReader& reader) throw () {
  vtrans=Vec(0,0);
  vrot=0;
  for (int i=0; i<3; i++) vaux[i] = 0;
  dv_mode = ROBOTVELOCITY;

  int h;
  if (reader.get("PlayerDummy::DriveVectorMode", h)) {
    switch (h) {
    case 0: dv_mode = ROBOTVELOCITY; break;
    case 1: dv_mode = WHEELVELOCITY; break;
    case 2: dv_mode = MOTORVOLTAGE; break;
    default:
      dv_mode = ROBOTVELOCITY;
    }
  }

  vector<double> dd (2);
  double d;
  if (reader.get("PlayerDummy::vtrans", dd)>=2) {
    vtrans.x=dd[0];
    vtrans.y=dd[1];
  }
  if (reader.get("PlayerDummy::vrot", d)>=1)
    vrot=d;
  
  vector<double> ddd (3);
  if (reader.get("PlayerDummy::vaux", ddd)>=3) {
    for (int i=0; i<3; i++) vaux[i]=ddd[i]; 
  }
}

DriveVector PlayerDummy::process_drive_vector (Time tt) throw () {

  MWM.get_message_board().publish("Dummy an Erde");
  MWM.get_message_board().publish("<Spielzug><Was>Pass</Was><Wo>x=1000 y=2000</Wo></Spielzug>");

  if (MWM.get_game_state().refstate==stopRobot)
    return DriveVector (Vec(0,0),0,false);
  else {
    if (dv_mode == ROBOTVELOCITY)
      return DriveVector (vtrans,vrot,false);
    return DriveVector(vaux[0], vaux[1], vaux[2], false, dv_mode);
  }
}
