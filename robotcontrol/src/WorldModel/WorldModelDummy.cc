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


#include "WorldModelDummy.h"
#include "WorldModelFactory.h"
#include "../Structures/Journal.h"

#include "WorldModel.h"

using namespace std;
using namespace Tribots;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public WorldModelBuilder {
    static Builder the_builder;
  public:
    Builder () {
      WorldModelFactory::get_world_model_factory ()->sign_up (string("Dummy"), this);
    }
    WorldModelType* get_world_model (const std::string&, const ConfigReader& reader, WorldModelType*) throw (TribotsException,bad_alloc) {
      return new WorldModelDummy (reader);
    }
  };
  Builder the_builder;
}



Tribots::WorldModelDummy::WorldModelDummy (const ConfigReader& vr) : field_geometry (vr), null_stream ("/dev/null")  {
  game_state.in_game = false;
  game_state.refstate = stopRobot;
  bool b;
  vector<double> darray (6);

  if (vr.get ("WorldModelDummy::ball", darray)>=4) {
    ball_loc.pos = Vec (darray[0], darray[1]);
    ball_loc.velocity = Vec (darray[2], darray[3]);
    ball_loc.quality=0.8;
    ball_loc.pos_known=BallLocation::known;
  }
  if (vr.get ("WorldModelDummy::robot", darray)>=6) {
    robot_loc.pos = Vec (darray[0], darray[1]);
    robot_loc.heading.set_deg (darray[2]);
    robot_loc.vtrans = Vec (darray[3], darray[4]);
    robot_loc.vrot = darray[5];
  }
  if (vr.get ("WorldModelDummy::kick", b)>=1)
    robot_loc.kick=b;
}

WorldModelDummy::~WorldModelDummy () throw () {;}

void WorldModelDummy::startstop (bool b) throw () {
  game_state.in_game=b;
}

void WorldModelDummy::update_refbox (RefboxSignal sig) throw () {
  if (sig==Tribots::SIGstop)
    game_state.refstate = Tribots::stopRobot;
  else if (sig==Tribots::SIGstart)
    game_state.refstate = Tribots::freePlay;
}
