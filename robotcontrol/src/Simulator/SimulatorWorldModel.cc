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


#include "SimulatorWorldModel.h"
#include "../WorldModel/WorldModelFactory.h"
#include "../Structures/Journal.h"

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public WorldModelBuilder {
    static Builder the_builder;
  public:
    Builder () {
      WorldModelFactory::get_world_model_factory ()->sign_up (string("Simulator"), this);
    }
    WorldModelType* get_world_model (const std::string&, const ConfigReader& reader, WorldModelType*) throw (TribotsException,bad_alloc) {
      return new SimulatorWorldModel (reader);
    }
  };
  Builder the_builder;
}




SimulatorWorldModel::SimulatorWorldModel (const ConfigReader& reader) throw (std::bad_alloc, Tribots::InvalidConfigurationException) : WorldModelTypeBase (reader), the_sim_client (NULL), ball_filter (reader), obstacle_filter (reader, get_field_geometry()), odobox (20, 5, 5), velocity_filter (10), stuck_sensor (reader) {
  string confline;
  if (reader.get("Simulator::robot_config_file", confline)<=0) {
    JERROR("no config line \"Simulator::robot_config_file\" found");
    throw Tribots::InvalidConfigurationException("Simulator::robot_config_file");
  }
  try{
    the_sim_client=SimClient::get_sim_client (confline.c_str());
  }catch(std::invalid_argument&){
    JERROR("cannot connect to simulator");
    throw Tribots::InvalidConfigurationException("Simulator::robot_config_file");
  }
}

SimulatorWorldModel::~SimulatorWorldModel () throw () {;}

RobotLocation SimulatorWorldModel::get_robot (Time t) const throw () {
  RobotLocation dest = odobox.add_movement (cpos, cpos_time, t);
  dest.stuck = stuck_sensor.get_stuck_location(dest.pos, dest.vtrans);
  return dest;
}

BallLocation SimulatorWorldModel::get_ball (Time t) const throw () {
  return ball_filter.get(t);
}

ObstacleLocation SimulatorWorldModel::get_obstacles (Time) const throw () {
  return obstacle_filter.get_with_poles_and_stuck();
}

void SimulatorWorldModel::set_drive_vector (DriveVector dv, Time t) throw () {
  odobox.add_drive_vector (dv, t);
}

void SimulatorWorldModel::set_odometry (DriveVector, Time) throw () {;}

void SimulatorWorldModel::set_visual_information (const VisibleObjectList&) throw () {;}

void SimulatorWorldModel::set_visual_information (const VisibleObject&, Time) throw () {;}

void SimulatorWorldModel::update_localisation () throw () {
  the_sim_client->update();
  double own = -get_own_half();
  Angle add = (own<0.0 ? Angle::half : Angle::zero);
  cpos.pos = own*the_sim_client->robot_position;
  cpos.heading = the_sim_client->robot_heading+add;
  cpos.quality = 1;
  cpos_time = the_sim_client->timestamp;
  ball_filter.update (own*the_sim_client->ball_position, the_sim_client->timestamp, cpos.pos);
  vector<double> olwidth (0);
  vector<Vec> olpos (the_sim_client->obstacle_positions.size());
  olwidth.assign (the_sim_client->obstacle_positions.size(), 600);   // Standardbreite 600mm
  for (unsigned int i=0; i<the_sim_client->obstacle_positions.size(); i++)
    olpos[i]=own*the_sim_client->obstacle_positions[i];
  obstacle_filter.update (olpos, olwidth, the_sim_client->timestamp);
  velocity_filter.update (cpos, cpos_time);
  stuck_sensor.update ();
}

const RobotLocation& SimulatorWorldModel::get_slfilter_robot_location (Time& t) const throw () {
  return velocity_filter.get (t);
}

void SimulatorWorldModel::reset () throw () {;}

void SimulatorWorldModel::reset (const Vec) throw () {;}

Time SimulatorWorldModel::get_timestamp_latest_update () const throw () { return cpos_time; }
