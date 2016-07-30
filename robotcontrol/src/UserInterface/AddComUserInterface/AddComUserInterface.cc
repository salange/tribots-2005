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

#include "AddComUserInterface.h"
#include "../TerminalUserInterface/TerminalUserInterface.h"
#include "../../Structures/Journal.h"



Tribots::AddComUserInterface::AddComUserInterface (const ConfigReader& conf, Player& _pl, ImageProcessing& _ip) throw (Tribots::TribotsException, std::bad_alloc) : pl(_pl), ip(_ip), commFailCount(0), commMaxFailCount(20)
{
  string confline;
  if (conf.get("user_interface_type", confline)<=0) {
    JERROR("no config line \"user_interface_type\" found");
    throw Tribots::InvalidConfigurationException ("user_interface_type");
  }
  if (conf.get("comm_max_fail_count", commMaxFailCount)<0) {
    JERROR("wrong comm_max_fail_count");
    throw Tribots::InvalidConfigurationException ("user_interface_type");
  }

  if (confline=="TerminalUserInterface")
    {
      the_local_user_interface = new TerminalUserInterface(conf, pl, ip);
    }
  else 
    {
      JERROR("No UserInterfaceType of this type found");
      throw Tribots::InvalidConfigurationException ("user_interface_type");
    }

  the_communication = TribotsUdpServer::get_instance_pointer();
  the_communication->receive_all();
}


Tribots::AddComUserInterface::~AddComUserInterface () throw () 
{
  delete the_local_user_interface;
  TribotsUdpServer::delete_instance();
}


bool Tribots::AddComUserInterface::process_messages () throw () 
{
  the_communication->processRequests(WorldModel::get_main_world_model(), pl);
  
  if (the_communication->receive_all())
    {
      commFailCount = 0;

      bool new_ig;
      if (the_communication->getInGame(new_ig))
	{
	  MWM.startstop(new_ig);
	}

      RefboxSignal new_rs;
      if (the_communication->getRefboxSignal (new_rs))
	{
	  MWM.update_refbox (new_rs);
	}

      GameState new_game_state;
      if(the_communication->getGameState(new_game_state))
	if ((new_game_state.refstate != MWM.get_game_state().refstate) || (new_game_state.in_game != MWM.get_game_state().in_game)) {
	      MWM.startstop (new_game_state.in_game);
	      switch (new_game_state.refstate) {
	      case stopRobot: MWM.update_refbox (SIGstop); break;
	      case freePlay: MWM.update_refbox (SIGstart); break;
	      case preOwnKickOff: MWM.update_refbox (SIGownKickOff); break;
	      case preOpponentKickOff: MWM.update_refbox (SIGopponentKickOff); break;
	      case preOwnFreeKick: MWM.update_refbox (SIGownFreeKick); break;
	      case preOpponentFreeKick: MWM.update_refbox (SIGopponentFreeKick); break;
	      case preOwnGoalKick: MWM.update_refbox (SIGownGoalKick); break;
	      case preOpponentGoalKick: MWM.update_refbox (SIGopponentGoalKick); break;
	      case preOwnCornerKick: MWM.update_refbox (SIGownCornerKick); break;
	      case preOpponentCornerKick: MWM.update_refbox (SIGopponentCornerKick); break;
	      case preOwnThrowIn: MWM.update_refbox (SIGownThrowIn); break;
	      case preOpponentThrowIn: MWM.update_refbox (SIGopponentThrowIn); break;
	      case preOwnPenalty: MWM.update_refbox (SIGownPenalty); break;
	      case preOpponentPenalty: MWM.update_refbox (SIGopponentPenalty); break;
	      case postOpponentKickOff: case postOpponentGoalKick: 
	      case postOpponentCornerKick: case postOpponentThrowIn:
	      case postOpponentPenalty: case postOpponentFreeKick: 
	      case ownPenalty: case opponentPenalty: MWM.update_refbox (SIGready); break;
	      case errorState: MWM.update_refbox (SIGstop); break;
	      }
	    }
	    
      int new_oh;
      if(the_communication->getOwnHalf(new_oh))
	if ( new_oh != ((WorldModel::get_main_world_model()).get_own_half()))
	  (WorldModel::get_main_world_model()).set_own_half(new_oh);

      char pt[200];
      if (the_communication->getPlayerType(pt))
	{
	  //std::cerr << "Received new Player type: " << pt << "\n\r";
	  if ( strcmp(pt,pl.get_player_type())!=0) 
	    pl.change_player_type(pt);
	}

      float xhint, yhint, hhint;
      if (the_communication->getSLHint(xhint, yhint, hhint))
	{
	  int dir = (MWM.get_own_half()>0 ? +1 : -1);
	  MWM.reset(dir*Vec(xhint,yhint), Angle::rad_angle(hhint)+(dir<0 ? Angle::half : Angle::zero));
	  std::cerr << "Will reset World model SL! with x: " << xhint << " y: " << yhint << " h: " << hhint << "\n";
	}
      if (the_communication->getSLHint(xhint, yhint))
	{
	  int dir = (MWM.get_own_half()>0 ? +1 : -1);
	  MWM.reset(dir*Vec(xhint,yhint));
	  std::cerr << "Will reset World model SL! with x: " << xhint << " y: " << yhint << "\n";
	}
     
      // Receive all Messages ... notify world model
      (WorldModel::get_main_world_model()).get_message_board().clear();
      vector<string> msgList;
      the_communication->getMessageList(msgList);
      (WorldModel::get_main_world_model()).get_message_board().receive (msgList);   // neue Messages einfuegen
    }
  else {
    if (commFailCount++ >= commMaxFailCount) {
      (WorldModel::get_main_world_model()).get_message_board().clear();
    }
    
  }

  return the_local_user_interface->process_messages();
}
