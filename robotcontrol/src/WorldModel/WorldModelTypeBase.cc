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


#include "WorldModelTypeBase.h"
#include "../Structures/GameState.h"
#include "LocationShortTimeMemory.h"

using namespace Tribots;
using namespace std;

Tribots::WorldModelTypeBase::WorldModelTypeBase (const ConfigReader& vread) throw (InvalidConfigurationException, std::bad_alloc) : field_geometry (vread), gsman (vread, field_geometry), own_half (1), locations (new LocationShortTimeMemory (*this)), null_stream ("/dev/null") {
  string confline;
  if (vread.get ("own_half", confline)>0) {
    if (confline=="yellow")
      own_half=1;
    else if (confline=="blue")
      own_half=-1;
  }
  null_stream.close();   // Zur absoluten Sicherheit, dass nicht rausgeschrieben werden kann bzw. Betriebssystem nicht eingreifen muss
  no_good_rloc.pos=no_good_rloc.vtrans=Vec::zero_vector;
  no_good_rloc.vrot=0;
  no_good_rloc.heading=Angle::zero;
  no_good_rloc.kick=false;
}

Tribots::WorldModelTypeBase::~WorldModelTypeBase () throw () {
  delete locations;
}

const Tribots::FieldGeometry& Tribots::WorldModelTypeBase::get_field_geometry () const throw () { return field_geometry; }

int Tribots::WorldModelTypeBase::get_own_half () const throw () { return own_half; }

const GameState& Tribots::WorldModelTypeBase::get_game_state () const throw () { return gsman.get(); }

const Tribots::RobotProperties& Tribots::WorldModelTypeBase::get_robot_properties () const throw () { return robot_properties; }

void Tribots::WorldModelTypeBase::set_own_half (int h) throw () { own_half = ( h>=0 ? 1 : -1);}

void Tribots::WorldModelTypeBase::set_robot_properties (const Tribots::RobotProperties& rpr) throw () { robot_properties=rpr; }

const Tribots::RobotData& Tribots::WorldModelTypeBase::get_robot_data (Time & _time) const throw () { _time = robot_data_time; return robot_data; };

void Tribots::WorldModelTypeBase::set_robot_data (const Tribots::RobotData& rd, Time _time) throw() { robot_data = rd; robot_data_time = _time;};

std::ostream& Tribots::WorldModelTypeBase::log_stream () throw () {
  return null_stream;
}

void Tribots::WorldModelTypeBase::reset (const Vec p, const Angle) throw () {
  reset (p);
}

void Tribots::WorldModelTypeBase::update () throw () {
  update_game_state ();
  update_localisation ();
}

void Tribots::WorldModelTypeBase::update_game_state () throw () {
  gsman.update();
}

void Tribots::WorldModelTypeBase::update_refbox (RefboxSignal sig) throw () {
  gsman.update (sig);
}

void Tribots::WorldModelTypeBase::startstop (bool b) throw () {
  gsman.set_in_game (b);
}

void WorldModelTypeBase::init_cycle (Time t1, Time t2) throw () {
  gsman.init_cycle (t1, t2);
}

const RobotLocation& WorldModelTypeBase::get_slfilter_robot_location (Time&) const throw () {
  return no_good_rloc;
}

MessageBoard& WorldModelTypeBase::get_message_board () throw () {
  return mboard;
}

const RobotLocation& WorldModelTypeBase::get_robot_location (Time t, bool b) throw () {
  return locations->get_robot_location (t, b);
}

const BallLocation& WorldModelTypeBase::get_ball_location (Time t, bool b) throw () {
  return locations->get_ball_location (t, b);
}

const ObstacleLocation& WorldModelTypeBase::get_obstacle_location (Time t, bool b) throw () {
  return locations->get_obstacle_location (t, b);
}
