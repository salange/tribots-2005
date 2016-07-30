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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void RemoteRobotWidget::init() {
  comm=NULL;
  rsm=NULL;
  state.comm_started = state.comm_okay = false;
  state.in_game = false;
  state.tribots_team.team_color=1;
  state.tribots_team.own_half=1;
  state.team=1;
  state.playertype = "";
  state.playerrole = "";
  state.refstate = Tribots::errorState;
  state.use_joystick = false;
  state.robot_data_request =false;
  state.obstacle_data_request = false;
  for (int i=0; i<Tribots::num_referee_states; i++)
    refstate_select->insertItem (Tribots::referee_state_names [i]);
  for (unsigned int i=0; i<Tribots::num_roles; i++)
    role_select->insertItem (Tribots::playerRoleNames[i]);
  vcc_view->display(0);
  numComNotOK=100;
}

void RemoteRobotWidget::destroy() {
  if (comm) delete comm;
  if (rsm) delete rsm;
}

void RemoteRobotWidget::connectClicked() {
  if (!comm) {
    state.ip = ip_edit->text().ascii();
    int posdoppelpunkt=state.ip.find(":");
    if (posdoppelpunkt>1) {
      int used_port=state.port;
      string portstr=string(state.ip.substr(posdoppelpunkt+1,state.ip.length()-posdoppelpunkt));
      used_port=atoi(portstr.c_str());
      state.ip=state.ip.substr(0,posdoppelpunkt);
      
      cout <<" IP  :" << state.ip << endl;
      cout <<" Port:" << used_port << endl;
      
      comm = new Tribots::TribotsUdpCommunication (false, used_port, state.ip.c_str());
    } else {
      comm = new Tribots::TribotsUdpCommunication (false, state.port, state.ip.c_str());
    }
    
    if (comm) {
      state.comm_started=true;
      
      comm->clear_to_send();
      comm->clearRequests();
    }
  }
  update_display ();
}

void RemoteRobotWidget::disconnectClicked() {
  if (comm)
    delete comm;
  comm=NULL;
  state.comm_started=false;
  state.comm_okay=false;
  state.in_game=false;
  update_display ();
}

void RemoteRobotWidget::playerChanged(const QString& desc) {
  if (comm)
    comm->putPlayerType (desc.ascii());
}

void RemoteRobotWidget::refboxSignal(Tribots::RefboxSignal sig) {
  Tribots::Vec no_good;
  Tribots::RefboxSignal rs = rsm->update (sig, no_good, false);
  if (comm)
    comm->putRefboxSignal (rs);
}

void RemoteRobotWidget::enableClicked() {
  bool b = true;
  if (comm){
    comm->putInGame (b);
  }
}

void RemoteRobotWidget::disableClicked() {
  bool b = false;
  if (comm)
    comm->putInGame (b);
}

void RemoteRobotWidget::refstateChanged(int index) {
  Tribots::RefereeState rs = Tribots::RefereeState(index);
  // versuche eine Reaktion zu erzeugen, die der beabsichtigten
  // moeglichst aehnlich ist:
  switch (rs) {
  case Tribots::stopRobot: refboxSignal (Tribots::SIGstop); break;
  case Tribots::freePlay: refboxSignal (Tribots::SIGstart); break;
  case Tribots::preOwnKickOff: refboxSignal (Tribots::SIGownKickOff); break;
  case Tribots::preOpponentKickOff: refboxSignal (Tribots::SIGopponentKickOff); break;
  case Tribots::preOwnFreeKick: refboxSignal (Tribots::SIGownFreeKick); break;
  case Tribots::preOpponentFreeKick: refboxSignal (Tribots::SIGopponentFreeKick); break;
  case Tribots::preOwnGoalKick: refboxSignal (Tribots::SIGownGoalKick); break;
  case Tribots::preOpponentGoalKick: refboxSignal (Tribots::SIGopponentGoalKick); break;
  case Tribots::preOwnCornerKick: refboxSignal (Tribots::SIGownCornerKick); break;
  case Tribots::preOpponentCornerKick: refboxSignal (Tribots::SIGopponentCornerKick); break;
  case Tribots::preOwnThrowIn: refboxSignal (Tribots::SIGownThrowIn); break;
  case Tribots::preOpponentThrowIn: refboxSignal (Tribots::SIGopponentThrowIn); break;
  case Tribots::preOwnPenalty: refboxSignal (Tribots::SIGownPenalty); break;
  case Tribots::preOpponentPenalty: refboxSignal (Tribots::SIGopponentPenalty); break;
  case Tribots::postOpponentKickOff: case Tribots::postOpponentGoalKick: 
  case Tribots::postOpponentCornerKick: case Tribots::postOpponentThrowIn:
  case Tribots::postOpponentPenalty: case Tribots::postOpponentFreeKick: 
  case Tribots::ownPenalty: case Tribots::opponentPenalty: refboxSignal (Tribots::SIGready); break;
  case Tribots::errorState: refboxSignal (Tribots::SIGstop); break;
  }
}

void RemoteRobotWidget::communicate() {
  bool extended_request;
   if (!state.comm_okay)
   {
	 if (numComNotOK>4) extended_request=true; else extended_request=false;
   	 numComNotOK++;
   } else
   {
   	numComNotOK=0;
   	extended_request=false;
   }
   
  if (comm && state.comm_started) {
    // Joystick-Steuerung?
    if (state.use_joystick)
      comm->putRemoteCtr(state.joy_drive);
    // MessageBoard senden
    if (state.mboard.get_outgoing().size()>0)
      comm->putMessageList (state.mboard.get_outgoing());
    state.mboard.clear();
    
    comm->setGameStateRequest();
    comm->setOwnHalfRequest();
    comm->setBallLocationRequest();
    comm->setRobotLocationRequest();
    comm->setPlayerTypeRequest();
    
    if (extended_request) {
      // Playerliste abfragen
      comm->setPlayerTypeListRequest();
      comm->setFieldGeometryRequest();
    }
    
    if (state.obstacle_data_request)
      comm->setObstacleLocationRequest();
    if (state.visible_object_request)
      comm->setVisibleObjectListRequest();
    
    // Roboterdaten?
    if (state.robot_data_request)
      comm->setRobotDataRequest();
    
    if (extended_request)	
      comm->putRequests(true); // Mit bestaetigungserwartung versenden
    else 	
      comm->putRequests();
			
    comm->send();
    state.comm_okay = comm->receive_all();
    
    if (state.comm_okay) {
      std::vector<std::string> ptl;
      if (comm->getPlayerTypeList(ptl)) {
	bool pltlist_changed = (ptl.size()!=static_cast<unsigned int>(player_select->count()));
	if (!pltlist_changed) {
	  for (unsigned int i=0; i<static_cast<unsigned int>(player_select->count()); i++) {
	    bool entry_found=false;
	    for (unsigned int j=0; j<ptl.size(); j++)
	      if (ptl[j]==std::string(player_select->text(j).ascii())) {
		entry_found=true;
		break;
	      }
	    if (!entry_found) {
	      pltlist_changed=true;
	      break;
	    }
	  }
	}
	if (pltlist_changed) {
	  player_select->clear();
	  for (unsigned int i=0; i<ptl.size(); i++) {
	    player_select->insertItem (ptl[i].c_str());
	  }
	}
      }
      comm->getFieldGeometry(state.fieldgeometry);
      
      // Hindernisse
      if (state.obstacle_data_request) {
	comm->getObstacleLocation(state.obstacles);
      }
      
      // erkannte Objekte
      if (state.visible_object_request) {
	comm->getVisibleObjectList (state.visible_objects);
      }
	
      // GameState
      Tribots::GameState gs;
      if (comm->getGameState(gs)) {
	state.refstate=gs.refstate;
	state.in_game=gs.in_game;
      } else {
	state.refstate = Tribots::errorState;
	state.in_game=false;
      }
      
      // OwnHalf
      int oh;
      if (comm->getOwnHalf (oh)) {
	if (oh!=state.team*state.tribots_team.own_half) {
	  state.team*=-1;
	  rsm->set_team_color (state.team*state.tribots_team.team_color);
	}
      }
      
      // Ball- und Roboterposition
      comm->getBallLocation(state.ball_pos);
      comm->getRobotLocation(state.robot_pos);
      
      // PlayerType
      char buffer[200];
      comm->getPlayerType(buffer);
      if (state.playertype!=buffer) {
	state.playertype=buffer;
	unsigned int index=0;
	for (unsigned int j=0; j<static_cast<unsigned int>(player_select->count()); j++)
	  if (std::string(buffer)==std::string(player_select->text(j).ascii())) {
	    index=j;
	    break;
	  }
	player_select->setCurrentItem (index);
      }
		
      // Roboterdaten
      if (comm->getRobotData(state.robot_data)) {
	vcc_view->display (static_cast<int>(state.robot_data.motor_vcc));
	if (state.robot_data.motor_vcc<20)
	  vcc_view->setPaletteBackgroundColor (Qt::red);
	else if (state.robot_data.motor_vcc<22)
	  vcc_view->setPaletteBackgroundColor (Qt::yellow);
	else
	  vcc_view->setPaletteBackgroundColor (Qt::green);
      }
      
      // Messages
      std::vector<std::string> msgList;
      comm->getMessageList (msgList);
      state.mboard.receive (msgList);
      std::string mline = state.mboard.scan_for_prefix ("NewRole:");
      if (mline.length()>0) {
	std::deque<std::string> parts;
	Tribots::split_string (parts, mline);
	if (mline.size()>1) {
	  role_select->setCurrentText (parts[1].c_str());
	  state.playerrole = parts[1];
	}
      }
      
    }

    comm->clear_to_send(); // für naechsten Zyklus
    comm->clearRequests();
  } else {
    state.mboard.clear();
  }
  update_display ();  
}

void RemoteRobotWidget::init_robot(const Tribots::ConfigReader& cfg, const Tribots::FieldGeometry& fg, const char* rid) {
  // default-Werte setzen
  state.name = rid;
  state.ip = "";
  state.port = 6012;
  
  cfg.get ((string(rid)+string("::name")).c_str(), state.name);
  cfg.get ((string(rid)+string("::ip")).c_str(), state.ip);
  cfg.get ((string(rid)+string("::port")).c_str(), state.port);
  
  setCaption (state.name.c_str());
  ip_edit->setText (state.ip.c_str());

  rsm = new Tribots::RefereeStateMachine (fg);
  rsm->set_team_color (state.team*state.tribots_team.team_color);
}

void RemoteRobotWidget::update_display() {
  name_edit->setText (state.name.c_str());
//    ip_edit->setBackgroundColor (state.comm_started ? (state.comm_okay ? Qt::green : Qt::blue) : Qt::red));
  if (state.comm_started)
  {
  	if (state.comm_okay)
	{
		ip_edit->setBackgroundColor (Qt::green);
	} else
	if (numComNotOK<4)
	{
		ip_edit->setBackgroundColor (Qt::darkGreen);
	} else
	{
		ip_edit->setBackgroundColor (Qt::blue);
	}
  } else ip_edit->setBackgroundColor (Qt::red);
  name_edit->setBackgroundColor (state.comm_started ? (state.in_game ? Qt::green : Qt::red) : Qt::white);
  QString rsn = Tribots::referee_state_names [state.refstate];
  refstate_select->setCurrentText (rsn);
  player_select->setCurrentText (state.playertype.c_str());
  own_half_edit->setBackgroundColor (state.team*state.tribots_team.own_half>0 ? Qt::yellow : Qt::blue);
  own_half_edit->setText (state.team*state.tribots_team.own_half>0 ? "yellow" : "blue");
  teamcolor_edit->setBackgroundColor (state.team*state.tribots_team.team_color>0 ? Qt::cyan : Qt::magenta);
  teamcolor_edit->setText (state.team*state.tribots_team.team_color>0 ? "cyan" : "magenta");
}

void RemoteRobotWidget::change_team_clicked() {
  int n = -state.team*state.tribots_team.own_half;
  if (comm)
    comm->putOwnHalf (n);
}

void RemoteRobotWidget::direction_of_play_changed( TribotsTools::DirectionOfPlay dp) {
  if (dp.own_half!=state.tribots_team.own_half || dp.team_color!=state.tribots_team.team_color) {
    if (dp.team_color!=state.tribots_team.team_color)
      rsm->set_team_color (-state.team*state.tribots_team.team_color);
    state.tribots_team=dp;
    int n = state.team*state.tribots_team.own_half;
    if (comm)
      comm->putOwnHalf (n);
  }
}

const TribotsTools::RemoteRobotState& RemoteRobotWidget::get_state() {
  return state;
}

void RemoteRobotWidget::slDisplacement(Tribots::Vec p, Tribots::Angle h) {
  if(comm) {
    float x = p.x;
    float y = p.y;
    float a = h.get_rad();
    comm->putSLHint (x, y, a);
  }
}

void RemoteRobotWidget::joystick(Tribots::DriveVector dv) {
  state.use_joystick = true;
  state.joy_drive = dv;
}

void RemoteRobotWidget::no_joystick() {
  state.use_joystick=false;
}

void RemoteRobotWidget::robot_data (bool b) {
  state.robot_data_request = b;
}

Tribots::MessageBoard& RemoteRobotWidget::get_message_board () {
  return state.mboard;
}

void RemoteRobotWidget::obstacle_data( bool b) {
  state.obstacle_data_request = b;
}

void RemoteRobotWidget::visible_object_request( bool b) {
  state.visible_object_request = b;
}

void RemoteRobotWidget::hide() {
  state.in_game=false;
  disconnectClicked();
  QWidget::hide();
}


void RemoteRobotWidget::roleChanged( const QString & desc ) {
  state.mboard.publish_stream() << "ChangeRole: " << desc.ascii() << '\n';
}
