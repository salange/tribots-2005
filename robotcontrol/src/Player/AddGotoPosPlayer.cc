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


#include "AddGotoPosPlayer.h"
#include "PlayerFactory.h"
#include "../WorldModel/WorldModel.h"
#include "../Fundamental/stringconvert.h"

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public PlayerBuilder {
    static Builder the_builder;
  public:
    Builder () {
      PlayerFactory::get_player_factory ()->sign_up (string("AddGotoPosPlayer"), this);
    }
    PlayerType* get_player (const std::string&, const ConfigReader& reader, PlayerType* pt) throw (TribotsException,bad_alloc) {
      return new AddGotoPosPlayer (reader, pt);
    }
  };
  Builder the_builder;
}






AddGotoPosPlayer::AddGotoPosPlayer (const ConfigReader& cfg, PlayerType* pt) throw (InvalidConfigurationException, std::bad_alloc) : the_elementary_player (pt), is_active(false) {
  if (pt==NULL)
    throw InvalidConfigurationException("AddGotoPosPlayer ohne richtigen Player");
  goto_pos_skill.set_dynamics (1, 2, 2, 2);
}

AddGotoPosPlayer::~AddGotoPosPlayer () throw () {
  delete the_elementary_player;
}

DriveVector AddGotoPosPlayer::process_drive_vector (Time t) throw () {
  DriveVector dest = the_elementary_player->process_drive_vector (t);
  
  string prline = MWM.get_message_board().scan_for_prefix ("GotoPos:");
  deque<string> parts;
  split_string (parts, prline);
  if (parts.size()>=4) {
    // pruefen, ob neue Position vorgegeben wurde
    Vec tp;
    double thd;
    string2double (tp.x, parts[1]);
    string2double (tp.y, parts[2]);
    string2double (thd, parts[3]);
    int oh = MWM.get_own_half();
    goto_pos_skill.init (oh*tp, Angle::deg_angle (thd) + (oh>0 ? Angle::zero : Angle::half), true);
    is_active=true;
    latest_refstate=MWM.get_game_state().refstate;
  }
  
  if (is_active) {
    // pruefen, ob GotoPos weiterhin aktiv sein soll
    RefereeState crefstate = MWM.get_game_state().refstate;
    if (latest_refstate!=crefstate) {
      if (crefstate==postOpponentKickOff || crefstate==postOpponentGoalKick || crefstate==postOpponentCornerKick || crefstate==postOpponentThrowIn || crefstate==postOpponentFreeKick || crefstate==postOpponentPenalty)
	latest_refstate = crefstate;
      else
	is_active=false;
    }
  }

  if (is_active) {
    // wenn aktiv, dann DriveVector von GotoPosSkill holen
    goto_pos_skill.getCmd (&dest, t);
    dest.kick=false;
  }

  return dest;
}
