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


#include "AddJoystickPlayer.h"
#include "PlayerFactory.h"
#include "../WorldModel/WorldModel.h"
#include "../Structures/GameState.h"
#include "../Communication/tribotsUdpServer.h"

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public PlayerBuilder {
    static Builder the_builder;
  public:
    Builder () {
      PlayerFactory::get_player_factory ()->sign_up (string("AddJoystickPlayer"), this);
    }
    PlayerType* get_player (const std::string&, const ConfigReader& reader, PlayerType* pt) throw (TribotsException,bad_alloc) {
      return new AddJoystickPlayer (reader, pt);
    }
  };
  Builder the_builder;
}





AddJoystickPlayer::AddJoystickPlayer (const ConfigReader& vr, PlayerType* pl) throw (InvalidConfigurationException, std::bad_alloc) : JoystickPlayer (vr), the_elementary_player (pl) {
  if (!pl)
    throw InvalidConfigurationException ("player_type");
  start_button = 1;
  stop_button = 0;
  vector<unsigned int> ii (5);
  if (vr.get ("Joystick::buttons", ii)>=5) {
    start_button = ii[3];
    stop_button = ii[4];
  }
}

AddJoystickPlayer::~AddJoystickPlayer () throw () {
  delete the_elementary_player;
}

DriveVector AddJoystickPlayer::process_drive_vector (Time t) throw () {
  DriveVector dest = the_elementary_player->process_drive_vector (t);  // damit irgendwelche zyklischen Aufgaben erledigt werden koennen

  // if there is a communication take the remote control Vector for joystick control
  if (TribotsUdpServer::hasInstance() && MWM.get_game_state().refstate==stopRobot) {
    TribotsUdpCommunication * com = TribotsUdpServer::get_instance_pointer();
    com->getRemoteCtr(remoteCtrDrv);
    dest = remoteCtrDrv;
  } else if (js) {
    const std::vector<bool>& buttons (js->get_button_state());
    if (buttons[stop_button]) {   // zu GameState::stop_robot wechseln
      MWM.update_refbox (SIGstop);
    } else if (buttons[start_button]) {  // zu GameState::free_play wechseln
      MWM.update_refbox (SIGstart);
    }
    if (MWM.get_game_state().refstate==stopRobot) {
      // wenn Roboter gestoppt, per Joystick steuern
      dest =  JoystickPlayer::process_drive_vector (t);
    }
  }
  return dest;
}

