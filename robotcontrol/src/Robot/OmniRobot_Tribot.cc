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


#include "OmniRobot_Tribot.h"
#include "RobotFactory.h"
#include "../Structures/RobotProperties.h"
#include "../Structures/Journal.h"
#include <fstream>

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public RobotBuilder {
    static Builder the_builder;
  public:
    Builder () {
      RobotFactory::get_robot_factory ()->sign_up (string("OmniRobot_Tribot"), this);
    }
    RobotType* get_robot (const std::string&, const ConfigReader& reader, RobotType*) throw (TribotsException,bad_alloc) {
      return new OmniRobot_Tribot (reader);
    }
  };
  Builder the_builder;
}





Tribots::OmniRobot_Tribot::OmniRobot_Tribot (const ConfigReader & vr) throw (TribotsException, bad_alloc)
{

  string confline;
  
  if (vr.get("hardware_error_log", confline)<=0) {
    JERROR("no config line \"hardware_error_log\" found");
    throw Tribots::InvalidConfigurationException ("hardware_error_log");
  }
  hardware_errout_stream   = new ofstream(confline.c_str());

  if (vr.get("hardware_info_log", confline)<=0) {
    JERROR("no config line \"hardware_info_log\" found");
    throw Tribots::InvalidConfigurationException ("hardware_info_log");
  }  
  hardware_infoout_stream  = new ofstream(confline.c_str());

  if (vr.get("robot_config_file", confline)<=0) {
    JERROR("no config line \"robot_config_file\" found");
    throw Tribots::InvalidConfigurationException ("robot_config_file");
  }  
  ctrRobot = new ctrOmniRobot(confline.c_str(), "CTR_OMNIROBOT", hardware_errout_stream, hardware_infoout_stream);
  
  if (vr.get("communication", confline)<=0) {
    JERROR("no config line \"communication\" found");
    throw Tribots::InvalidConfigurationException ("communication");
  }
  if (strcmp(confline.c_str(),"blocking") == 0) blocking_communication = true;
  else blocking_communication = false;
    
  if (!ctrRobot->init())
    {
      throw Tribots::BadHardwareException("Init_Robot");
    }

  amIKicking     = false;
  lastKickStart.update();
  lastKickEnd.update();

  // Default-Werte setzen
  robot_properties.max_velocity                  = 1.5;
  robot_properties.max_acceleration              = 0.5;
  robot_properties.max_deceleration              = 0.8;
  robot_properties.max_rotational_velocity       = 3.0;
  robot_properties.max_rotational_acceleration   = 0.5;
  robot_properties.max_rotational_deceleration   = 1.0;
  robot_properties.max_robot_radius              = 300;
  robot_properties.min_robot_radius              = 200;
  robot_properties.kicker_width                  = 290;
  robot_properties.kicker_distance               = 190;
  robot_properties.robot_width                   = 450;
  robot_properties.robot_length                  = 450;
  robot_properties.omnidirectional_drive         = true;
  robot_properties.drive_vector_delay            = 0;

  // ggf. aus dem config-File Werte ueberschreiben
  robot_properties.read (vr);

}

Tribots::OmniRobot_Tribot::~OmniRobot_Tribot() throw()
{
  delete ctrRobot;
}

void Tribots::OmniRobot_Tribot::set_drive_vector (DriveVector dv) throw ()
{

  // Entscheide ob ein Kick Kommando zu senden ist
  // Zeitsteuerung für kick ... Ausfahren und Einfahren des Kickers
  bool kickCom   = false;
  bool kickValue = false;
  Time t_now; // aktuelle Zeit

  if (!amIKicking && dv.kick && (abs(lastKickEnd.diff_msec(t_now)) > 400) )
    {
      kickCom       = true;
      kickValue     = true;
      amIKicking    = true;
      lastKickStart = t_now;
    }
  else if (amIKicking && (abs(lastKickStart.diff_msec(t_now)) > 100 ) )
    {
      kickCom       = true;
      kickValue     = false;
      amIKicking    = false;
      lastKickEnd   = t_now;
    }
  
  DriveVector dv_to_set;
  dv_to_set = dv;

  bool robot_changes_dv = true; // -> ins config file!!

  float _r_xm, _r_ym, _r_pm;

  if (robot_changes_dv)
    ctrRobot->get_realizable_vel_vec((float) lastDriveState.vtrans.x, (float) lastDriveState.vtrans.y,  (float) lastDriveState.vrot,
				     (float) dv.vtrans.x, (float) dv.vtrans.y, (float) dv.vrot, (float) abs(t_now.diff_msec(lastDriveStateTime)),
				     _r_xm, _r_ym, _r_pm);
  dv_to_set.vtrans.x = _r_xm;
  dv_to_set.vtrans.y = _r_ym;
  dv_to_set.vrot     = _r_pm;

  lastDriveState     = dv_to_set;
  lastDriveStateTime = t_now;

  // Kommunikation mit Roboter und update des Weltmodells
  if (blocking_communication)
    {
      Time t_dv;
      timeval sendtime;
      bool comres=false;
      
      switch (dv_to_set.mode) {
      case ROBOTVELOCITY:
	comres = ctrRobot->setVel_vec_blocking(dv_to_set.vtrans.x, dv_to_set.vtrans.y, dv_to_set.vrot, &data[0], &sendtime);
	break;
      case WHEELVELOCITY:
	comres = ctrRobot->setVel_wheels_blocking(dv_to_set.vaux[0], dv_to_set.vaux[1], dv_to_set.vaux[2], &data[0], &sendtime);
	break;
      case MOTORVOLTAGE:
	std::cerr << "Voltage drive vector not yet implemented!!\n";
	break;
      }
      
      t_dv.set(sendtime);
      dv_to_set.kick = amIKicking;
      if (comres) {
	if (WorldModel::is_main_world_model_available()) {
	  Time tw_dv = t_dv;
	  tw_dv.add_msec (robot_properties.drive_vector_delay);
	  WorldModel::get_main_world_model().set_drive_vector (dv_to_set, tw_dv);
	}
	else {
	  JWARNING("comres set but no world model available");
	}
      }

      if (kickCom) ctrRobot->setKickPort_blocking(kickValue);

      if (comres) {
	  Time t_odo;
	  DriveVector odo;
	  odo.vtrans.x = data[0].robot_vel[0];
	  odo.vtrans.y = data[0].robot_vel[1];
	  odo.vrot     = data[0].robot_vel[2];
	  odo.kick     = amIKicking;           // OK, diese Info stimmt nicht ganz mit Timestamp überein ... sollte aber reichen
	  t_odo.set(data[0].timestamp_vel);
	  if (WorldModel::is_main_world_model_available()) {
	    WorldModel::get_main_world_model().set_odometry (odo, t_odo);
	  }
	  else {
	    JWARNING("comres set but no world model available");
	  }
	}
      //std::cerr << "get: " << odo.vtrans.x << ' ' << odo.vtrans.y << ' ' << odo.vrot << ' ' << (odo.kick ? "kick       " : "--           ") << '\n';
    }
  else
    {
      if (kickCom) 
	{
	  //usleep(1);  // Kommunikation geht sonst nicht !!! (zwei Befehle hintereinander mag er nit)
	  ctrRobot->setKickPort_nonblocking(kickValue);
	}
      else
	{
	  Time t_dv;
	  timeval sendtime;
	  bool comres=false;

	  switch (dv_to_set.mode) {
	  case ROBOTVELOCITY:
	    comres = ctrRobot->setVel_vec_nonblocking(dv_to_set.vtrans.x, dv_to_set.vtrans.y, dv_to_set.vrot, &sendtime);
	    break;
	  case  WHEELVELOCITY:
	    comres = ctrRobot->setVel_wheels_nonblocking(dv_to_set.vaux[0], dv_to_set.vaux[1], dv_to_set.vaux[2], &sendtime);
	    break;
	  case MOTORVOLTAGE:
	    std::cerr << "Voltage drive vector not yet implemented!!\n";
	    break;
	  }

	  t_dv.set(sendtime);
	  dv_to_set.kick = amIKicking;
	  if (comres) {
	    if (WorldModel::is_main_world_model_available()) {
	      Time tw_dv = t_dv;
	      tw_dv.add_msec (robot_properties.drive_vector_delay);
	      WorldModel::get_main_world_model().set_drive_vector (dv_to_set, tw_dv);
	    }
	    else {
	      JWARNING("comres set but no world model available");
	    }
	  }
	}
      

      Time t_odo;
      DriveVector odo;
      
      int n=ctrRobot->getData_nonblocking(data, 9);

      for (int i=n-1; i>=0; i--)
	{
	  odo.vtrans.x = data[i].robot_vel[0];
	  odo.vtrans.y = data[i].robot_vel[1];
	  odo.vrot     = data[i].robot_vel[2];
	  odo.kick     = amIKicking;           // OK, diese Info stimmt nicht ganz mit Timestamp überein ... sollte aber reichen
	  t_odo.set(data[i].timestamp_vel);
	  if (WorldModel::is_main_world_model_available()) {	  
	    WorldModel::get_main_world_model().set_odometry (odo, t_odo);
	  }
	}

      RobotData rd;
      Time      rd_time;
      fill_robot_data(data[0], dv_to_set, rd, rd_time);
      if (WorldModel::is_main_world_model_available()) {
	WorldModel::get_main_world_model().set_robot_data(rd, rd_time);
      }
    }
}

Tribots::RobotProperties Tribots::OmniRobot_Tribot::get_robot_properties () const throw ()
{
  return robot_properties;
}


void Tribots::OmniRobot_Tribot::fill_robot_data( const ctrOmniRobotData_t &src, const DriveVector &dv_set, RobotData &rd, Time &rd_time)
{
  rd.BoardID = src.tmc_version;
  if (rd.BoardID == 0)
    sprintf(rd.robotIdString,"TMC OLD SW OLD");
  else if (rd.BoardID == 1)
    sprintf(rd.robotIdString,"TMC (1.2) SW (1.06)");
  else if (rd.BoardID == 2)
    sprintf(rd.robotIdString,"TMC (1.2) SW (1.16)");
  else 
    sprintf(rd.robotIdString,"UNBEKANNT : %d", rd.BoardID);

  for (int i=0; i<3; i++)
    {
      rd.wheel_vel[i]     = src.wheel_vel[i];
      rd.robot_vel[i]     = src.robot_vel[i];
      rd.motor_current[i] = src.current[i];
      rd.motor_output[i]  = src.output[i];
      
      rd.motor_temp_switch[i] = src.temp_switch[i];
      rd.motor_temp[i]    = src.temp[i];
    }
  
  rd.motor_vcc = src.vcc;

  rd.dv_set = dv_set;

  rd_time.set(src.timestamp_vel);
}
