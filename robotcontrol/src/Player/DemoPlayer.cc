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


#include "DemoPlayer.h"
#include "PlayerFactory.h"
#include "../WorldModel/WorldModel.h"
#include "../Structures/GameState.h"

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace
{
  class Builder : public PlayerBuilder
  {
    static Builder the_builder;
  public:
    Builder ()
    {
      PlayerFactory::get_player_factory ()->sign_up (string("DemoPlayer"), this);
    }
    PlayerType* get_player (const std::string&, const ConfigReader& reader, PlayerType*) throw (TribotsException,bad_alloc)
    {
      return new DemoPlayer (reader);
    }
  };
  Builder the_builder;
}




DemoPlayer::DemoPlayer (const ConfigReader&) throw () {;}

DriveVector DemoPlayer::process_drive_vector (Time t) throw ()
{
  // example of a demo player that always drives to the ball
  // its movement is not very intelligent, it only serves as demo
  if (MWM.get_game_state().refstate==freePlay) {  // check whether it's free play
    RobotLocation rloc = MWM.get_robot_location (t);  // in global coordinates
    BallLocation bloc = MWM.get_ball_location (t);  // in global coordinates
    Vec direction = (bloc.pos-rloc.pos).normalize();
    double ang = (direction.angle()-rloc.heading+Angle::quarter).get_rad_pi();
    DriveVector ret;  // the driving command in local!!!!!! coordinates
    ret.vtrans = 1.0*direction.rotate(-rloc.heading); // drive with 1.0m/s in direction of the ball
    ret.vrot = -1.3*ang; // rotate towards the ball (simple P-controller)
    if ((bloc.pos-rloc.pos).length()<400) // kick if the ball is near
      ret.kick=true;
    else
      ret.kick=false;
    return ret;
  } else {
    return DriveVector (Vec(0,0),0,false);
  }
}






























