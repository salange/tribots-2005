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

#include "WhiteBoard.h"
#include "../WorldModel/WorldModel.h"
#include "../Fundamental/stringconvert.h"

using namespace std;

#define LEFT 0
#define MIDDLE 1
#define RIGHT 2

#define DISPLAY_AREAS 1
#define DISPLAY_DRIBBLE_CORRIDOR 0
#define DISPLAY_FREEGOALPOS_CORRIDORS 0
#define DISPLAY_SHOOTCORRIDOR 1

namespace Tribots {

  //////////////////////// singleton implementation ///////////////////////////

  WhiteBoard* WhiteBoard::instance = 0;
  
  WhiteBoard* 
  WhiteBoard::getTheWhiteBoard()
  {
    if (instance == 0) {
      instance = new WhiteBoard();
    }
    return instance;
  }


  ////////////////////////// init and deinit code /////////////////////////////
  
  WhiteBoard::WhiteBoard()
    :m_bDoNotGetBallPossession(false),ballInOppGoalData(0),possesBallData(0),abs2relData(0),rel2absData(0),playerRole(PlayerRole(4)),cycles_without_team_posses_ball(10),cycles_without_advanced_team_posses_ball(10)
  {
    frames_ball_owned=0;
  }


  WhiteBoard::~WhiteBoard()
  {
    if (instance == this) {       // cleanup singleton
      instance = 0;
    }

    // <<<<<<<<< put your clean up code here

    if (ballInOppGoalData) delete ballInOppGoalData;
    if (possesBallData) delete possesBallData;
    if (abs2relData) delete abs2relData;
    if (rel2absData) delete rel2absData;
  }

  //////////////////////////// isBallInOppGoal ////////////////////////////////

  bool
  WhiteBoard::isBallInOppGoal(const Time& t)
  {
    BoolData* data;
    if (ballInOppGoalData) {
      data = dynamic_cast<BoolData*>(ballInOppGoalData);
    
      if (data->t     == t &&
	  data->cycle == 
	  WorldModel::get_main_world_model().get_game_state().cycle_num) {
	return data->b;          // Zwischengespeichertes Ergebnis zurück
      }
    }
    else {                       // erste Anfrage überhaupt
      data = new BoolData();     // Zwischenspeicher erzeugen
      ballInOppGoalData = data;
    }

    data->t = t;
    data->cycle = 
      WorldModel::get_main_world_model().get_game_state().cycle_num;

    BallLocation ballLocation =
      WorldModel::get_main_world_model().get_ball_location(t);
    FieldGeometry field = 
      WorldModel::get_main_world_model().get_field_geometry(); 

    if (ballLocation.pos.y < field.field_length / 2. ||
	ballLocation.pos.x < -field.goal_width ||
	ballLocation.pos.x > field.goal_width ||
	ballLocation.pos.y > field.field_length / 2. + field.goal_length * 2) {
      return data->b =  false;
    }
    else {
      return data->b = true;
    }
  }

  ////////////////////////////// doPossesBall /////////////////////////////////

  bool
  WhiteBoard::doPossesBall(const Time& t)
  {
    BoolData* data;
    if (possesBallData) {
      data = dynamic_cast<BoolData*>(possesBallData);
    
      if (data->t     == t &&
	  data->cycle == 
	  WorldModel::get_main_world_model().get_game_state().cycle_num) {
	return data->b;          // Zwischengespeichertes Ergebnis zurück
      }
    }
    else {                       // erste Anfrage überhaupt
      data = new BoolData();     // Zwischenspeicher erzeugen
      possesBallData = data;
      
    }

    data->t = t;
    data->cycle = 
      WorldModel::get_main_world_model().get_game_state().cycle_num;

    BallLocation ballLocation =
      WorldModel::get_main_world_model().get_ball_location(t);

    Vec relBall = getAbs2RelFrame(t) * ballLocation.pos;

    bool ballclose=true;
    if (
    (relBall.y> 350|| relBall.y<0)  // nicht schiessen, wenn ball zu weit weg
        ||
     (fabs(relBall.x) > 120)     )
    {  
      ballclose=false;
   }
      
    if (ballclose){
frames_ball_owned++;

     }
   else 
frames_ball_owned--;

if (frames_ball_owned<0)
{
frames_ball_owned=0;
}
if (frames_ball_owned>10)
{
frames_ball_owned=10;
}


if (frames_ball_owned <5){
data->b = false; 
return false;
}

if(!m_bDoNotGetBallPossession)
{
data->b = true; 
return true;
}
return true;
  }

  ///////////////////////////// getRel2AbsFrame ///////////////////////////////

  const Frame2d&
  WhiteBoard::getRel2AbsFrame(const Time& t)
  {
    Frame2dData* data;
    if (rel2absData) {
      data = dynamic_cast<Frame2dData*>(rel2absData);
    
      if (data->t     == t &&
	  data->cycle == 
	  WorldModel::get_main_world_model().get_game_state().cycle_num) {
	return data->frame;      // Zwischengespeichertes Ergebnis zurück
      }
    }
    else {                       // erste Anfrage überhaupt
      data = new Frame2dData();  // Zwischenspeicher erzeugen
      rel2absData = data;
    }
    
    RobotLocation robotLocation = 
      WorldModel::get_main_world_model().get_robot_location(t);
    
    data->frame.set_position(robotLocation.pos);
    data->frame.set_angle(robotLocation.heading);

    return data->frame;
  }

  ///////////////////////////// getAbs2RelFrame ///////////////////////////////

  const Frame2d&
  WhiteBoard::getAbs2RelFrame(const Time& t)
  {
    Frame2dData* data;
    if (abs2relData) {
      data = dynamic_cast<Frame2dData*>(abs2relData);
    
      if (data->t     == t &&
	  data->cycle == 
	  WorldModel::get_main_world_model().get_game_state().cycle_num) {
	return data->frame;      // Zwischengespeichertes Ergebnis zurück
      }
    }
    else {                       // erste Anfrage überhaupt
      data = new Frame2dData();  // Zwischenspeicher erzeugen
      abs2relData = data;
    }

    data->frame = getRel2AbsFrame(t);    // rel2abs holen
    data->frame.invert();                // und invertieren

    return data->frame;
  }
  
  const PlayerRole WhiteBoard::getPlayerRole()
  {
    return playerRole;
  }

  void WhiteBoard::changePlayerRole(PlayerRole newrole)
  {
    playerRole=newrole;
    MOUT << "NewRole: " << playerRoleNames[newrole] << '\n';
  }

  void WhiteBoard::kickOffPosition(Vec& p, Angle& h) {
    p = kick_off_pos;
    h = kick_off_heading;
  }

  void WhiteBoard::checkMessageBoard()
  {
    // nach Rollenwechsel suchen 
    string prline = MWM.get_message_board().scan_for_prefix ("ChangeRole:");
    deque<string> parts;
    split_string (parts, prline);
    if (parts.size()>1) {
      for (unsigned int i=0; i<num_roles; i++) {
	if (parts[1]==playerRoleNames[i]) {
	  changePlayerRole (PlayerRole(i));
	}
      }
    }
    if (MWM.get_message_board().scan_for_prefix ("WhichRole?").length()>0)
      MOUT << "NewRole: " << playerRoleNames[playerRole] << '\n';

    // nach TeamBallbesitz suchen
    if (MWM.get_message_board().scan_for_prefix ("OwnsBall!").length()>0)
      cycles_without_team_posses_ball=0;
    else
      cycles_without_team_posses_ball++;
      
    // nach erweitertem TeamBallbesitz suchen
    if (MWM.get_message_board().scan_for_prefix ("NearBall!").length()>0)
      cycles_without_advanced_team_posses_ball=0;
    else
      cycles_without_advanced_team_posses_ball++;
  
    // nach KickOffPos suchen
    prline=MWM.get_message_board().scan_for_prefix ("KickOffPos:");
    if (prline.length()>0) {
      deque<string> parts;
      split_string (parts, prline);
      if (parts.size()>=4) {
	string2double (kick_off_pos.x, parts[1]);
	string2double (kick_off_pos.y, parts[2]);
	double d;
	string2double (d, parts[3]);
	kick_off_heading.set_deg(d);
      }
    }
  }
  
  bool WhiteBoard::teamPossesBall() 
  {
    return (cycles_without_team_posses_ball<6);
  }
  
  bool WhiteBoard::advancedTeamPossesBall() 
  {
    return (cycles_without_advanced_team_posses_ball<6);
  }

}
