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


#include <sstream>
#include <cstring>
#include <cmath>
#include "TerminalUserInterface.h"
#include "../../WorldModel/WorldModel.h"
#include "../../Structures/GameState.h"
#include "../../Player/WhiteBoard.h"
#include "../../Structures/Journal.h"
#include "../../ImageProcessing/ImageIO.h"

using namespace std;

namespace {

  // formatierte double-Werte
  string format_double (double d, unsigned int flen, unsigned int prec) {
    std::stringstream inout;
    inout.precision (prec);
    inout.setf(ios_base::fixed,ios_base::floatfield);
    inout << d;
    string res;
    std::getline (inout, res);
    int n=flen-res.length();
    while (n>0) {
      res=string(" ")+res;
      n--;
    }
    return res;
  }
   
} 


Tribots::TerminalUserInterface::TerminalUserInterface (const ConfigReader& vr, Player& pl, ImageProcessing& ip) throw () : the_player(pl), the_image_processing(ip), single_step_mode(0), wait_for_manual_start(false), manual_start_sec (10), requestedImage(false)
{
  // Terminalfenster vorbereiten
  window = initscr();
  cbreak();
  noecho();
  nodelay(window,true);
  init_window ();
  update_window ();
  
  int i;
  debug_image_filename = "debug_image.ppm";
  if ((vr.get ("manual_start_waiting_time", i)>0) && (i>0))
    manual_start_sec=i;
  if (vr.get ("debug_image_filename", debug_image_filename) < 0) {
    throw InvalidConfigurationException("debug_image_filename");
  }
}

Tribots::TerminalUserInterface::~TerminalUserInterface () throw () {
  // Terminalfenster wieder freigeben
  endwin();
}

bool Tribots::TerminalUserInterface::process_messages () throw () {
  if (single_step_mode>0) {
    single_step_mode--;
    if (single_step_mode==0)
      MWM.startstop (false);
  }
  if (wait_for_manual_start && (manual_start_timer.elapsed_msec()>1000*manual_start_sec)) {
    wait_for_manual_start=false;
    MWM.startstop (true);
  }
  if (requestedImage) {
    try {
      if (the_image_processing.is_image_available()) {
	const Image* img = the_image_processing.get_image();
	save_image(img);
	the_image_processing.free_image();
	requestedImage = false;
      }
    } catch (TribotsException& e) {
      JERROR(e.what());
    }
  }

  // Tastatur abfragen
  char c=getch();
  switch (c) {
  case ' ' : // Roboter stoppen 
    MWM.startstop (false);
    single_step_mode=0;
    break;
  case 'a' : // Roboter aktivieren, aber den RefereeState nicht veraendern
    MWM.startstop (true);
    single_step_mode=0;
    break;
  case 'g' : // Roboter aktivieren und auf Refereestate FreePlay gehen
    MWM.startstop (true);
    single_step_mode=0;
    MWM.update_refbox (SIGstop);
    MWM.update_refbox (SIGstart);
    break;
  case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': // Einzelschrittmodus
    MWM.startstop (true);
    single_step_mode=static_cast<unsigned int>(c-'0');
    break;    
  case 'i':
    if (!requestedImage) {
      try {
	the_image_processing.request_image_raw();
	requestedImage = true;
      } catch (TribotsException& e) {
	JERROR(e.what());
	requestedImage = false;
      }
    }
    break;
  case 'I':
    if (!requestedImage) {
      try {
	requestedImage = true;
	the_image_processing.request_image_processed();
      } catch (TribotsException& e) {
	JERROR(e.what());
	requestedImage = false;
      }
    }
    break;
  case 't' : // Start in 10 Sekunden
    wait_for_manual_start=true;
    manual_start_timer.update();
    break;
  case 's' : // Seitenwechsel
    MWM.set_own_half(-MWM.get_own_half());
    break;
  case 'f' : // Bildschirm refresh
    init_window();
    break;
  case 'p' : // Selbstlokalisierung zuruecksetzen
    WorldModel::get_main_world_model().reset ();
    break;
  case 'G' : // Spielertyp Goalie/GoaliePadua setzen
    if (string(the_player.get_player_type())==string("Goalie"))
      the_player.change_player_type ("GoaliePadua");
    else
      the_player.change_player_type ("Goalie");
    break;
  case 'F' : // Spielertyp FunMove/ParkMove
      the_player.change_player_type ("FunPlayer");
    break;
  case 'J' : // Spielertyp Joystick setzen
    the_player.change_player_type ("JoystickPlayer");
    break;
  case 'R' : // Spielertyp RCPlayer setzen und Spielerrolle wechseln
    if (string(the_player.get_player_type())!=string("RCPlayer"))
      the_player.change_player_type ("RCPlayer");
    else {
      // Rollenwechsel
      unsigned int next_role = WhiteBoard::getTheWhiteBoard()->getPlayerRole();
      next_role = (next_role+1)%num_roles;
      WhiteBoard::getTheWhiteBoard()->changePlayerRole(PlayerRole(next_role));
    }
    break;
  case 'x': case 'X': case 'q': case 'Q': 
    return false;   // Programm beenden
  }

  // Bildschirmansicht aktualisieren
  update_window();
  return true;
}


void Tribots::TerminalUserInterface::init_window () throw () {
  clear();
  addstr ("TRIBOTS - ROBOTCONTROL");
  move (2,0); addstr("Programmzeit: ");
  move (3,0); addstr("Spielsituation: ");
  move (4,0); addstr("Spielertyp: ");
  move (5,0); addstr("Eigene Haelfte: "); 
  move (7,0); addstr("Roboterposition: ");
  move (8,0); addstr("Robotergeschwindigkeit: ");
  move (9,0); addstr("Ballposition (rel): ");
  move (10,0); addstr("Ballgeschwindigkeit: ");
  move (12,0); addstr("-----------------------------------------------------");
}  

void Tribots::TerminalUserInterface::update_window () throw () {
  std::stringstream inout;
  std::string line;
  Time current_time;
  inout << current_time << '\n';
  std::getline (inout, line);
  move (2,14); addstr(line.c_str()); addstr("    ");
  move (3,16); addstr(Tribots::referee_state_names[MWM.get_game_state().refstate]);
  if (wait_for_manual_start)
    addstr ("manueller Start");
  else if (MWM.get_game_state().in_game)
    addstr ("aktiviert      ");
  else
    addstr ("deaktiviert    ");
  RobotLocation rl = MWM.get_robot_location (current_time);
  inout << '(' << format_double(rl.pos.x,6,0) << ", " <<  format_double(rl.pos.y,6,0) << ", " << format_double(rl.heading.get_deg(),3,0) << ", " << ( rl.kick ? "kick" : " -- " ) << ")\n";
  std::getline (inout, line);
  move (7,17); addstr(line.c_str());
  inout << '(' << format_double(rl.vtrans.x,5,2) << ", " << format_double(rl.vtrans.y,5,2) << ", " << format_double(rl.vrot,5,2) << ")\n";
  std::getline (inout, line);
  move (8,24); addstr(line.c_str());
  BallLocation bl = MWM.get_ball_location (current_time);
  inout << '(' << format_double(((bl.pos-rl.pos)/rl.heading).x,6,0) << ", " << format_double(((bl.pos-rl.pos)/rl.heading).y,6,0) << ")";
  if (WhiteBoard::getTheWhiteBoard()->doPossesBall(current_time)) inout << " (own)\n"; else inout << "      \n";
  std::getline (inout, line);
  move (9,20); addstr(line.c_str());
  inout << '(' << format_double(bl.velocity.x,6,2) << ", " << format_double(bl.velocity.y,6,2) << ")\n";
  std::getline (inout, line);
  move (10,22); addstr(line.c_str());
  move (4,12); addstr(the_player.get_player_type());
  if (strcmp (the_player.get_player_type(), "RCPlayer")==0) {
    addstr(" / ");
    addstr(playerRoleNames[WhiteBoard::getTheWhiteBoard()->getPlayerRole()]);
    addstr ("      ");
  } else {
    addstr("                         "); 
  }
  move (5,16);
  WorldModel& the_world_model (WorldModel::get_main_world_model ());
  if (the_world_model.get_own_half()>0)
    addstr("gelb");
  else
    addstr("blau");
  move (13,0);
}


void Tribots::TerminalUserInterface::save_image(const Image* img) const throw(TribotsException)
{
  try {
    ImageIO* io = new PPMIO();
    io->write(*img, debug_image_filename);
    delete io;
  } catch (TribotsException& e) {
    JERROR (e.what());
  }
}


