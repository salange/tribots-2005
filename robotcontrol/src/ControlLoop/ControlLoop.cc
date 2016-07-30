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



#include "ControlLoop.h"
#include "../UserInterface/UserInterface.h"
#include "../Structures/Journal.h"
#include "../Structures/GameState.h"
#include "../Simulator/SimClient.h"
#include <sstream>
#include <cmath>
#include <unistd.h>
#include <cfloat>

using namespace Tribots;
using namespace std;

namespace {
  // Hilfsfunktion zum Umrechnen von doubles in strings:
  double potenzen10 [] = { 1,10,100,1000,10000,100000 };
  string to_string (double value, unsigned int precision =0) {  // precision=Anzahl Nachkommastellen
    stringstream inout;
    string sss;
    double faktor = ( precision<=5 ? potenzen10[precision] : 100000 );
    inout << floor(value*faktor+0.5)/faktor << '\n';
    inout >> sss;
    return sss;
  }

  inline bool is_inf (const double& x) throw () { return x>DBL_MAX; }
  inline bool is_nan (const double& x) throw () { return x!=x; }
  inline bool is_mininf (const double& x) throw () { return x<-DBL_MAX; }
  inline bool is_number (const double& x) throw () { return (!is_inf(x)) && (!is_mininf(x)) && (!is_nan(x)); }
    
}


ControlLoop::ControlLoop (const ConfigReader& conf) throw (TribotsException, bad_alloc) : configuration_list (conf), num_cycles (0) {
  // Reihenfolge der Initialisierung beachten: 
  // erst Weltmodell (da andere Komponenten darauf zugreifen koennen)
  // Roboter vor Spieler (da Spieler ggf. die Robotereigenschaften benoetigt)
  the_world_model = new WorldModel (configuration_list);
  the_robot = new Robot (configuration_list);
  the_player = new Player (configuration_list);
  the_image_processing = new ImageProcessing (configuration_list);
  the_user_interface = new UserInterface (conf,*the_player,*the_image_processing);

  if (configuration_list.get ("loop_time", loop_time)<=0 || loop_time<=0) {
    JERROR("no config line \"loop_time\" found");
    throw InvalidConfigurationException ("loop_time");
  }

  JMESSAGE("startup: all components alive");

  // ggf. Anfangsposition setzen
  vector<double> dd;
  if (configuration_list.get ("initial_position", dd)>=2) {
    if (dd.size()>=3)
      the_world_model->reset (Vec (dd[0], dd[1]),Angle::deg_angle(dd[2])-Angle::quarter);
    else
      the_world_model->reset (Vec (dd[0], dd[1]));
  }
  
  report_computation_time=report_computation_time_detailed=report_computation_time_gnuplot=false;
  configuration_list.get ("report_computation_time", report_computation_time);
  configuration_list.get ("report_computation_time_detailed", report_computation_time_detailed);
  configuration_list.get ("report_computation_time_gnuplot", report_computation_time_gnuplot);
  usec_image_processing=usec_world_model=usec_player=usec_robot=usec_user_interface=usec_idle=0;
}

ControlLoop::~ControlLoop () throw () {
  the_robot->emergency_stop();
  try{
    if (report_computation_time) {
      JMESSAGE((string("average loop time was ")+to_string(static_cast<double>(first_cycle_time.elapsed_msec())/static_cast<double>(num_cycles),2)+string(" ms")).c_str());
      JMESSAGE((string("average image processing time was ")+to_string(static_cast<double>(usec_image_processing)/static_cast<double>(1000*num_cycles),2)+string(" ms")).c_str());
      JMESSAGE((string("average world model time was ")+to_string(static_cast<double>(usec_world_model)/static_cast<double>(1000*num_cycles),2)+string(" ms")).c_str());
      JMESSAGE((string("average decision time was ")+to_string(static_cast<double>(usec_player)/static_cast<double>(1000*num_cycles),2)+string(" ms")).c_str());
      JMESSAGE((string("average robot communication time was ")+to_string(static_cast<double>(usec_robot)/static_cast<double>(1000*num_cycles),2)+string(" ms")).c_str());
      JMESSAGE((string("average user interface and communication time was ")+to_string(static_cast<double>(usec_user_interface)/static_cast<double>(1000*num_cycles),2)+string(" ms")).c_str());
      JMESSAGE((string("average idle time was ")+to_string(static_cast<double>(usec_idle)/static_cast<double>(1000*num_cycles),2)+string(" ms")).c_str());
    }
  }catch(exception&){;} // irgnoriern

  delete the_user_interface;
  delete the_image_processing;
  delete the_player;
  delete the_robot;
  delete the_world_model;
  SimClient::delete_sim_client ();
}

void ControlLoop::loop () throw () {
  VisibleObjectList list_of_visible_objects;
  first_cycle_time.update();
  Time time_reporter;
  int drive_vector_delay = the_world_model->get_robot_properties().drive_vector_delay;

  while (true) {
    stringstream gnuplot;
    gnuplot << "CONTROLLOOP_ELAPSED_GNUPLOT: "
	    << WorldModel::get_main_world_model().get_game_state().cycle_num
	    << " ";

    // stelle Startzeit des Schleifendurchlaufs fest (wegen Taktung)
    timestamp.update();
    Time expected_execution_time (timestamp);
    expected_execution_time.add_msec (loop_time+drive_vector_delay);
    num_cycles++;
    the_world_model->init_cycle(timestamp, expected_execution_time);

    // fuehre Bildverarbeitung durch und integriere die Ergebnisse im Weltbild (durch Seiteneffekt)
    if (report_computation_time) time_reporter.update();
    the_image_processing->process_image ();
    if (report_computation_time) usec_image_processing+=time_reporter.elapsed_usec();
    if (report_computation_time_detailed) LOUT << "ImageProcessingTime: " << time_reporter.elapsed_usec() << " usec\n";
    if (report_computation_time_gnuplot) gnuplot << time_reporter.elapsed_usec() << " ";

    // aktualisiere Weltbild (falls notwendig)
    if (report_computation_time) time_reporter.update();
    the_world_model->update();
    if (report_computation_time) usec_world_model+=time_reporter.elapsed_usec();
    if (report_computation_time_detailed) LOUT << "WorldModelTime: " << time_reporter.elapsed_usec() << " usec\n";
    if (report_computation_time_gnuplot) gnuplot << time_reporter.elapsed_usec() << " ";


    // berechne einen Spielzug
    if (report_computation_time) time_reporter.update();
    DriveVector intended_drive_vector = the_player->process_drive_vector (expected_execution_time);
    if (!is_number(intended_drive_vector.vtrans.x) || !is_number(intended_drive_vector.vtrans.y) || !is_number(intended_drive_vector.vrot)) {
      cerr << "ACHTUNG: Player liefert NAN!!!\n";
      JWARNING("Player liefert NAN in DriveVector");
      intended_drive_vector.vtrans.x=intended_drive_vector.vtrans.y=intended_drive_vector.vrot=0;
      intended_drive_vector.kick=false;
    }
    if (!the_world_model->get_game_state().in_game) {  // ueberpruefen, ob der Spieler tatsaechlich im Spiel ist
      intended_drive_vector.vtrans.x=intended_drive_vector.vtrans.y=intended_drive_vector.vrot=0;
      intended_drive_vector.kick=false;
    }
    if (report_computation_time) usec_player+=time_reporter.elapsed_usec();
    if (report_computation_time_detailed) LOUT << "PlayerTime: " << time_reporter.elapsed_usec() << " usec\n";
    if (report_computation_time_gnuplot) gnuplot << time_reporter.elapsed_usec() << " ";

    // Benutzerinteraktion zum Beenden der Regelschleife
    if (report_computation_time) time_reporter.update();    
    if (!the_user_interface->process_messages())
      break;
    if (report_computation_time) usec_user_interface+=time_reporter.elapsed_usec();
    if (report_computation_time_detailed) LOUT << "UserInterfaceTime: " << time_reporter.elapsed_usec() << " usec\n";
    if (report_computation_time_gnuplot) gnuplot << time_reporter.elapsed_usec() << " ";

    // warten bis 5 ms vor Ende der Regelschleife
    long int timediff = timestamp.elapsed_usec ();
    long int wait_usec = 1000*loop_time-timediff-5000;
    if (wait_usec>=1000) {
      usleep (wait_usec);
      usec_idle+=wait_usec;
    }

    // Roboter ansteuern
    if (report_computation_time) time_reporter.update();
    the_robot->set_drive_vector (intended_drive_vector);
    if (report_computation_time) usec_robot+=time_reporter.elapsed_usec();
    if (report_computation_time_detailed) LOUT << "RobotTime: " << time_reporter.elapsed_usec() << " usec\n";
    if (report_computation_time_gnuplot) gnuplot << time_reporter.elapsed_usec() << " ";

    // Evtl. nochmal warten, falls Ansteuerung schneller als 5 msec
    timediff = timestamp.elapsed_usec ();
    long int wait_usec2 = 1000*loop_time-timediff;
    if (wait_usec2>=1000) {
      usleep (wait_usec2);
      usec_idle+=wait_usec2;
    }

    if (report_computation_time_gnuplot) {
      string str;
      getline(gnuplot, str);
      JMESSAGE(str.c_str());
    }
    
  }

  // Beenden:
  the_robot->emergency_stop();
}

