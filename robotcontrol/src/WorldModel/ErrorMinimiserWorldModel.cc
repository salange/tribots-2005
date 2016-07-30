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


#include "ErrorMinimiserWorldModel.h"
#include "WorldModelFactory.h"
#include "WorldModel.h"
#include "update_robot_location.h"
#include "../Fundamental/stringconvert.h"

using namespace std;
using namespace Tribots;


// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public WorldModelBuilder {
    static Builder the_builder;
  public:
    Builder () {
      WorldModelFactory::get_world_model_factory ()->sign_up (string("ErrorMinimiser"), this);
    }
    WorldModelType* get_world_model (const std::string&, const ConfigReader& reader, WorldModelType*) throw (TribotsException,bad_alloc) {
      return new ErrorMinimiserWorldModel(reader);
    }
  };
  Builder the_builder;
}



ErrorMinimiserWorldModel::ErrorMinimiserWorldModel (const ConfigReader& reader) throw (std::bad_alloc) : WorldModelTypeBase (reader), odobox(20, 5, 10), sl(reader, odobox, get_field_geometry()), ball_filter (reader), obstacle_filter (reader, get_field_geometry()), velocity_filter (10), stuck_sensor (reader) {;}

ErrorMinimiserWorldModel::~ErrorMinimiserWorldModel () throw () {;}

void ErrorMinimiserWorldModel::set_drive_vector (DriveVector dv, Time t) throw () {
  odobox.add_drive_vector (dv, t);
}

void ErrorMinimiserWorldModel::set_odometry (DriveVector dv, Time t) throw () {
  odobox.add_odometry (dv,t);
}

void ErrorMinimiserWorldModel::set_visual_information (const VisibleObjectList& vol) throw () {
  visbox.add (vol);
}

void ErrorMinimiserWorldModel::set_visual_information (const VisibleObject& vo, Time t) throw () {
  visbox.add (vo, t);
}

void ErrorMinimiserWorldModel::reset () throw () {
  sl.reset ();
}

void ErrorMinimiserWorldModel::reset (const Vec p) throw () {
  sl.reset (get_own_half()*p);
}

void ErrorMinimiserWorldModel::reset (const Vec p, const Angle h) throw () {
  sl.reset (get_own_half()*p,h+(get_own_half()==-1? Angle::half : Angle::zero));
}

void ErrorMinimiserWorldModel::update_localisation () throw () {
  sl.update (visbox.get_lines(), visbox.get_goals());
  latest_vis_timestamp = visbox.get_lines().timestamp;
  RobotLocation pos_tvis = flip_sides (sl.get (latest_vis_timestamp), get_own_half());
  ball_filter.update (visbox.get_balls(), pos_tvis);
  BallLocation bloc_tvis = ball_filter.get (latest_vis_timestamp);
  obstacle_filter.update (visbox.get_obstacles(), pos_tvis, bloc_tvis);
  velocity_filter.update (pos_tvis, latest_vis_timestamp);
  stuck_sensor.update ();

  BallLocation bloc = ball_filter.get (latest_vis_timestamp);
  const string cb = get_message_board().scan_for_prefix ("Ball: ");
  if (cb.length()>0) {
    deque<string> parts;
    split_string (parts, cb);
    if (parts.size()>=3) {
      Vec bp;
      string2double (bp.x, parts[1]);
      string2double (bp.y, parts[2]);
      ball_filter.comm_ball (bp);
    }
  }
}

RobotLocation ErrorMinimiserWorldModel::get_robot (Time t) const throw () {
  RobotLocation dest = flip_sides (sl.get (t), get_own_half());
  dest.stuck = stuck_sensor.get_stuck_location (dest.pos, dest.vtrans);
  return dest;
}

const RobotLocation& ErrorMinimiserWorldModel::get_slfilter_robot_location (Time& t) const throw () {
  return velocity_filter.get (t);
}

BallLocation ErrorMinimiserWorldModel::get_ball (Time t) const throw () {
  return ball_filter.get (t);
}

ObstacleLocation ErrorMinimiserWorldModel::get_obstacles (Time) const throw () {
  return obstacle_filter.get_with_poles_and_stuck ();
}

Time ErrorMinimiserWorldModel::get_timestamp_latest_update () const throw () { return latest_vis_timestamp; }

const VisibleObjectList& ErrorMinimiserWorldModel::get_visible_objects () throw () { return visbox.get_all(); }
