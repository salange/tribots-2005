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


#include "tribotsUdpServer.h"
#include "../Fundamental/Time.h"
#include <sstream>
#include "../Structures/Journal.h"
#include "../WorldModel/update_robot_location.h"

Tribots::TribotsUdpServer* Tribots::TribotsUdpServer::tribotsUdpCom = NULL;

Tribots::TribotsUdpServer *Tribots::TribotsUdpServer::get_instance_pointer(int port)
{
  if (tribotsUdpCom) {
    return tribotsUdpCom;
  } else {
    return tribotsUdpCom = new TribotsUdpServer(port);
  }
}

Tribots::TribotsUdpServer::TribotsUdpServer(int port) : TribotsUdpCommunication(true, port)
{
  ;
}

Tribots::TribotsUdpServer::~TribotsUdpServer()
{
  ;
}


void Tribots::TribotsUdpServer::delete_instance()
{
   if (! tribotsUdpCom ) {
    return ;
  } else {
     delete tribotsUdpCom;
    tribotsUdpCom = NULL;
  }
}


bool Tribots::TribotsUdpServer::hasInstance()
{
  return (tribotsUdpCom!= NULL);
}


bool Tribots::TribotsUdpServer::processRequests(WorldModel& _wm, Player& _pl)
{
  clear_to_send();
  Time current_time;

  bool something_to_send=false;
  if (requestSettings->request[GameStateIdx]==send_once)
    {
      GameState gs = _wm.get_game_state();
      putGameState(gs);
      something_to_send=true;
    }
  if (requestSettings->request[OwnHalfIdx]==send_once)
    {
      int oh = _wm.get_own_half();
      putOwnHalf(oh);
      something_to_send=true;
    }
  if (requestSettings->request[FieldGeometryIdx]==send_once)
    {
      FieldGeometry fg = _wm.get_field_geometry();
      putFieldGeometry(fg);
      something_to_send=true;
    }
  if (requestSettings->request[BallLocationIdx]==send_once)
    {
      BallLocation bl = flip_sides (_wm.get_ball_location(current_time), _wm.get_own_half());
      putBallLocation(bl);
      something_to_send=true;
    }
  if (requestSettings->request[RobotPropertiesIdx]==send_once)
    {
      RobotProperties rp = _wm.get_robot_properties();
      putRobotProperties(rp);
      something_to_send=true;
    }
  if (requestSettings->request[RobotLocationIdx]==send_once)
    {
      RobotLocation rl = flip_sides (_wm.get_robot_location(current_time), _wm.get_own_half());
      putRobotLocation(rl);
      something_to_send=true;
    }
  if (requestSettings->request[PlayerTypeIdx]==send_once)
    {
      putPlayerType(_pl.get_player_type());
      something_to_send=true;
    }
  if (requestSettings->request[RobotDataIdx]==send_once)
    {
      Time d;
      RobotData rd = _wm.get_robot_data(d);
      putRobotData(rd);
      something_to_send=true;
    }

  something_to_send = something_to_send || processXMLRequests(_wm,_pl);

  if (something_to_send) { // if Abfrage hack!! annahme ... kommunikation steht, sonst kein senden möglich
    // Now process the Message board and send information if available
    MessageBoard& mb = _wm.get_message_board();
    std::vector<std::string> msgList = mb.get_outgoing();
    if (msgList.size()>0) {
      putMessageList(msgList);
    }
  }

  if (something_to_send)
    send();
  
  requestSettings->clear();

  return true;
}


bool Tribots::TribotsUdpServer::processXMLRequests(WorldModel &_wm, Player & _pl)
{
  Time current_time;
  bool res = false;

  //JERROR("processXMLReq");
  for (unsigned i=0; i<ReceivedXMLRequests.size(); i++)
    {
      if (ReceivedXMLRequests[i]==string("PlayerTypeList")) {
	vector<string> ptl;
	_pl.getPlayerTypeList(ptl);
	putPlayerTypeList(ptl);
	res = true;
      }
      else if (ReceivedXMLRequests[i]==string("ObstacleLocation")) {
	const ObstacleLocation& ol = flip_sides (_wm.get_obstacle_location(current_time, false), _wm.get_own_half());
	putObstacleLocation(ol);
	res = true;
      }
      else if (ReceivedXMLRequests[i]==string("VisibleObjectList")) {
	const VisibleObjectList& ol = _wm.get_visible_objects();
	putVisibleObjectList(ol);
	res = true;
      }
      else {
	ostringstream e;
	e << "Received a not known XML request! [" << ReceivedXMLRequests[i] << "]!";
	JERROR(e.str().c_str());
      }
    }
  ReceivedXMLRequests.clear();
  return res;
}
