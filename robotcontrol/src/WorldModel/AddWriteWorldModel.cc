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


#include "AddWriteWorldModel.h"
#include "WorldModelFactory.h"
#include "update_robot_location.h"
#include "../Structures/Journal.h"
#include <fstream>
#include <iostream>

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public WorldModelBuilder {
    static Builder the_builder;
  public:
    Builder () {
      WorldModelFactory::get_world_model_factory ()->sign_up (string("AddWriteWorldModel"), this);
    }
    WorldModelType* get_world_model (const std::string&, const ConfigReader& reader, WorldModelType* wm) throw (TribotsException,bad_alloc) {
      return new AddWriteWorldModel (reader, wm);
    }
  };
  Builder the_builder;
}




AddWriteWorldModel::AddWriteWorldModel (const ConfigReader& reader, WorldModelType* wm) throw () : the_world_model (wm) {
  string fbasename;
  if (reader.get ("write_world_model_info", fbasename)<=0)
    fbasename = "wminfo";
  string tsarg;

  odometry_out = new ofstream ((fbasename+string(".odo")).c_str());
  drive_vector_out = new ofstream ((fbasename+string(".drv")).c_str());
  visual_info_out = new ofstream ((fbasename+string(".vis")).c_str());
  robot_pos_out = new ofstream ((fbasename+string(".rpos")).c_str());
  ball_pos_out = new ofstream ((fbasename+string(".bpos")).c_str());
  obs_pos_out = new ofstream ((fbasename+string(".opos")).c_str());
  log_out = new ofstream ((fbasename+string(".log")).c_str());
  gs_out = new ofstream ((fbasename+string(".gs")).c_str());
  rawrobotdata_out = new ofstream ((fbasename+string(".rrd")).c_str());
  
  if (!(*odometry_out))
    JWARNING ("error while opening odometry file for writing");
  if (!(*drive_vector_out))
    JWARNING ("error while opening drive_vector file for writing");
  if (!(*visual_info_out))
    JWARNING ("error while opening visual info file for writing");
  if (!(*robot_pos_out))
    JWARNING ("error while opening robot pos file for writing");
  if (!(*ball_pos_out))
    JWARNING ("error while opening ball pos file for writing");
  if (!(*obs_pos_out))
    JWARNING ("error while opening obstacle pos file for writing");
  if (!(*log_out))
    JWARNING ("error while opening log info file for writing");
  if (!(*gs_out))
    JWARNING ("error while opening game state info file for writing");
  if (!(*rawrobotdata_out))
    JWARNING ("error while opening raw robot data file for writing");
  else
    (*rawrobotdata_out) << "vis time \t time \t BoardID \t wheel_vel[0,1,2] \t robot_vel[xm,ym,phi] \t aux_robot_vel[xm,ym,phi] \t motor_current[0,1,2] \t motor_output[0,1,2] \t motor_temp_switch[0,1,2] \t motor_temp[0,1,2] \t vcc \t DriveVector (t+1) x y phi\n";

  visual_writer = new VisibleObjectWriter (*visual_info_out);
  drv_writer = new DriveVectorWriter (*drive_vector_out);
  odo_writer = new DriveVectorWriter (*odometry_out);
  gs_writer = new GameStateWriter (*gs_out);
  robot_writer = new RobotLocationWriter (*robot_pos_out);
  ball_writer = new BallLocationWriter (*ball_pos_out);
  obstacle_writer = new ObstacleLocationWriter (*obs_pos_out);

  first_visual=true;

  // Feldgeometrie ins .log-File schreiben
  const FieldGeometry& fg (the_world_model->get_field_geometry());
  (*log_out) << "FieldGeometry: "
	     << fg.field_length << ' '
	     << fg.field_width << ' '
	     << fg.side_band_width << ' '
	     << fg.goal_band_width << ' '
	     << fg.goal_area_length << ' '
	     << fg.goal_area_width << ' '
	     << fg.penalty_area_length << ' '
	     << fg.penalty_area_width << ' '
	     << fg.center_circle_radius << ' '
	     << fg.corner_arc_radius << ' '
	     << fg.penalty_marker_distance << ' '
	     << fg.line_thickness << ' '
	     << fg.border_line_thickness << ' '
	     << fg.goal_width << ' '
	     << fg.goal_length << ' '
	     << fg.goal_height << ' '
	     << fg.pole_height << ' '
	     << fg.pole_diameter << ' '
	     << fg.pole_position_offset_x << ' '
	     << fg.pole_position_offset_y << ' '
	     << fg.ball_diameter << ' '
	     << fg.lineset_type << '\n';
  Time nulltime;
  nulltime.set_usec (0);
  timeval tv;
  nulltime.get (tv);
  (*log_out) << "StartingTimeval " << tv.tv_sec << ' ' << tv.tv_usec << '\n';
}

AddWriteWorldModel::~AddWriteWorldModel () throw () {
  (*log_out) << flush;
  (*rawrobotdata_out) << flush;

  delete visual_writer;
  delete drv_writer;
  delete odo_writer;
  delete gs_writer;
  delete robot_writer;
  delete ball_writer;
  delete obstacle_writer;

  delete odometry_out;
  delete drive_vector_out;
  delete visual_info_out;
  delete robot_pos_out;
  delete ball_pos_out;
  delete obs_pos_out;
  delete log_out;
  delete gs_out;
  delete rawrobotdata_out;

  delete the_world_model;
}


const RobotLocation& AddWriteWorldModel::get_robot_location (Time t, bool b) throw () {
  return the_world_model->get_robot_location(t, b);
}

const BallLocation& AddWriteWorldModel::get_ball_location (Time t, bool b) throw () {
  return the_world_model->get_ball_location(t, b);
}

const ObstacleLocation& AddWriteWorldModel::get_obstacle_location (Time t, bool b) throw () {
  return the_world_model->get_obstacle_location(t, b);
}

void AddWriteWorldModel::set_drive_vector (DriveVector dv, Time t) throw () {
  Time now;
  drv_writer->write (now.get_msec(), t.get_msec(), dv);
  the_world_model->set_drive_vector (dv,t);
}

void AddWriteWorldModel::set_odometry (DriveVector dv, Time t) throw () {
  Time now;
  odo_writer->write (now.get_msec(), t.get_msec(), dv);
  the_world_model->set_odometry (dv,t);
}

void AddWriteWorldModel::set_visual_information (const VisibleObjectList& vol) throw () {
  if (first_visual) {
    first_visual_timestamp.update();
    first_visual=false;
  }
  image_timestamp = vol.timestamp;
  visual_writer->write (first_visual_timestamp.get_msec(), vol.timestamp.get_msec(), vol);
  the_world_model->set_visual_information (vol);
}

void AddWriteWorldModel::set_visual_information (const VisibleObject& vo, Time t) throw () {
  if (first_visual) {
    first_visual_timestamp.update();
    first_visual=false;
  }
  image_timestamp = t;
  visual_writer->write (first_visual_timestamp.get_msec(), t.get_msec(), vo);
  the_world_model->set_visual_information (vo,t);
}

void AddWriteWorldModel::set_robot_data (const RobotData& rd, Time t) throw() 
{ 

  RobotLocation robot_location = the_world_model->get_slfilter_robot_location (t);
  robot_location.vtrans/=robot_location.heading;

  (*rawrobotdata_out) << first_visual_timestamp << '\t' << t 
		      << '\t' << rd.BoardID 
		      << '\t' << rd.wheel_vel[0] << '\t' << rd.wheel_vel[1] << '\t' << rd.wheel_vel[2]
		      << '\t' << rd.robot_vel[0] << '\t' << rd.robot_vel[1] << '\t' << rd.robot_vel[2]
		      << '\t' << robot_location.vtrans.x << '\t' << robot_location.vtrans.y << '\t' << robot_location.vrot
		      << '\t' << rd.motor_current[0] << '\t' << rd.motor_current[1] << '\t' << rd.motor_current[2]
		      << '\t' << rd.motor_output[0] << '\t' << rd.motor_output[1] << '\t' << rd.motor_output[2]
		      << '\t' << rd.motor_temp_switch[0] << '\t' << rd.motor_temp_switch[1] << '\t' << rd.motor_temp_switch[2]
		      << '\t' << rd.motor_temp[0] << '\t' << rd.motor_temp[1] << '\t' << rd.motor_temp[2]
		      << '\t' << rd.motor_vcc 
		      << '\t' << rd.dv_set.vtrans.x << '\t' << rd.dv_set.vtrans.y << '\t' << rd.dv_set.vrot
		      << "\n";
  the_world_model->set_robot_data (rd, t); 
}

void AddWriteWorldModel::update () throw () {
  the_world_model->update();
  first_visual=true;   // um bei Eintreffen der naechsten Bildinformation den Zeitstempel first_visual_timestamp zu setzen

  RobotLocation rloc_vis = flip_sides (the_world_model->get_robot_location (image_timestamp, false), the_world_model->get_own_half());
  BallLocation bloc_vis = flip_sides (the_world_model->get_ball_location (image_timestamp, false), the_world_model->get_own_half());
  GameState gs = the_world_model->get_game_state ();
  RobotLocation rloc_exec = flip_sides (the_world_model->get_robot_location (gs.expected_execution_time, true), the_world_model->get_own_half());
  BallLocation bloc_exec = flip_sides (the_world_model->get_ball_location (gs.expected_execution_time, true), the_world_model->get_own_half());
  ObstacleLocation oloc = flip_sides (the_world_model->get_obstacle_location (gs.expected_execution_time, true), the_world_model->get_own_half());

  Time now;
  robot_writer->write (now.get_msec(), image_timestamp.get_msec(), rloc_vis, gs.expected_execution_time.get_msec(),  rloc_exec);
  ball_writer->write (now.get_msec(), image_timestamp.get_msec(), bloc_vis, gs.expected_execution_time.get_msec(),  bloc_exec);
  obstacle_writer->write (now.get_msec(), oloc);
  gs_writer->write (now.get_msec(), gs);
}

void AddWriteWorldModel::init_cycle (Time now, Time exec_time) throw () {
  (*log_out) << "\n\%\%\%\%cycle " << now << '\t' << get_game_state().cycle_num+1 << '\n';
  the_world_model->init_cycle(now, exec_time);
}

void AddWriteWorldModel::reset () throw () {
  the_world_model->reset();
}

void AddWriteWorldModel::reset (const Vec p) throw () {
  the_world_model->reset(p);
}

void AddWriteWorldModel::reset (const Vec p, const Angle h) throw () {
  the_world_model->reset(p, h);
}

std::ostream& AddWriteWorldModel::log_stream () throw () {
  return (*log_out);
}

const RobotLocation& AddWriteWorldModel::get_slfilter_robot_location (Time& t) const throw () {
  return the_world_model->get_slfilter_robot_location (t);
}

MessageBoard& AddWriteWorldModel::get_message_board () throw () {
  return the_world_model->get_message_board ();
}

