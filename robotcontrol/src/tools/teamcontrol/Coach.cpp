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


#include "Coach.h"
#include "../../Fundamental/stringconvert.h"
#include "../../Structures/PlayerRole.h"
#include <algorithm>

using namespace TribotsTools;
using namespace Tribots;
using namespace std;

#define COMM_KICKOFF_VARIANTS 0
#define RCPLAYER_OWNER 1

namespace{
  const unsigned int max_player = 4;  // maximale Anzahl RCPlayer gleichzeitig aktiv
}

Coach::Coach (const Tribots::ConfigReader& cfg, const std::vector<std::string>& ids) throw (Tribots::TribotsException) : robot_ids (ids), desired_roles (ids.size()), actual_roles (ids.size()), role_known (ids.size()), is_active (ids.size()), activation_time(ids.size()), ko_policies (0), comm_ball_mode(false), broadcast_mode(false), owns_ball_mode(false),  rc_player_dynamic_role_change_mode (""), ignore_broadcast_prefix (0) {
  cfg.get ("Coach::communicated_ball_mode", comm_ball_mode);
  cfg.get ("Coach::broadcast_mode", broadcast_mode);
  cfg.get ("Coach::owns_ball_mode", owns_ball_mode);
  cfg.get ("Coach::rc_player_dynamic_role_exchange", rc_player_dynamic_role_change_mode);
  
  // verschiedene Rollenwechselstrategien laden:
  vector<string> change_policies;
  cfg.get ("Coach::rc_player_role_exchange_policies", change_policies);
  for (unsigned int r=0; r<change_policies.size(); r++) {
    if (prefix (string("dyna"), change_policies[r]))
      policies.push_back (new DynamicDefendOffendPolicy (cfg, change_policies[r].c_str()));
    else
      policies.push_back (new StaticPolicy (cfg, change_policies[r].c_str()));
  }
  if (policies_index (rc_player_dynamic_role_change_mode)==-1) rc_player_dynamic_role_change_mode="";
  for (unsigned int i=0; i<role_known.size(); i++)
    role_known[i]=false;

#if COMM_KICKOFF_VARIANTS
  // verschiedene Anstossvarianten laden:
  vector<string> kov;
  cfg.get ("Coach::kick_off_policies", kov);
  for (unsigned int r=0; r<kov.size(); r++) {
    KickOffPolicy policy;
    policy.name = kov[r];
    policy.pos.resize(max_player);
    vector<string> change_lines;
    for (unsigned int i=0; i<max_player; i++) {
      policy.pos[i].clear();
      cfg.get ((policy.name+string("::")+static_cast<char>(i+'1')).c_str(), change_lines);
      for (unsigned int j=0; j+4<=change_lines.size(); j+=4) {
	HPos hp;
	hp.role = change_lines[j];
	string2double (hp.pos.x, change_lines[j+1]);
	string2double (hp.pos.y, change_lines[j+2]);
	double d;
	string2double (d, change_lines[j+3]);
	hp.heading = Angle::deg_angle(d);
	policy.pos[i].push_back (hp);
      }
    }
    ko_policies.push_back (policy);
  }
  
  if (ko_policies.size()==0)
    cerr << "ACHTUNG: keine Anstossvarianten gefunden\n";
  else
    kick_off_own_policy = kick_off_opponent_policy = ko_policies[0].name;
#endif
  
  ignore_broadcast_prefix.push_back(string("NewRole:"));
}

Coach::~Coach () throw () {
  for (unsigned int i=0; i<policies.size(); i++)
    delete policies[i];
}

void Coach::set_comm_ball_mode (bool b) throw () { comm_ball_mode = b; }

bool Coach::get_comm_ball_mode () const throw () { return comm_ball_mode; }

void Coach::set_owns_ball_mode (bool b) throw () { owns_ball_mode = b; }

bool Coach::get_owns_ball_mode () const throw () { return owns_ball_mode; }

void Coach::set_broadcast_mode (bool b) throw() { broadcast_mode = b; }

bool Coach::get_broadcast_mode () const throw () { return broadcast_mode; }

bool Coach::dynamic_role_change_mode (const char* s) throw () {
  if (policies_index(s)>=0) {
    rc_player_dynamic_role_change_mode=s;
    return true;
  } else {
    rc_player_dynamic_role_change_mode="";
    return false;
  }
}

const string& Coach::dynamic_role_change_mode () const throw () {
  return rc_player_dynamic_role_change_mode;
}

void Coach::update (RemoteTeamState& ts) throw () {
  unsigned int num_robots = ts.robot_states.size();


  // Nachrichten weiterverteilen im Broadcast-Modus:
  if (broadcast_mode) {
    for (unsigned int sender=0; sender<num_robots; sender++) {
      if (ts.robot_states[sender]->in_game) {
	const vector<string>& msg (ts.message_boards[sender]->get_incoming());
	for (unsigned int i=0; i<msg.size(); i++) {  // bearbeitet die i-te Nachricht des sender
	  bool bc = true;
	  for (unsigned int k=0; k<ignore_broadcast_prefix.size(); k++) {
	    if (prefix (ignore_broadcast_prefix[k], msg[i])) {
	      bc=false;
	      break;
	    }
	  }
	  for (unsigned int receiver=0; receiver<num_robots; receiver++) {
	    if (sender!=receiver) {
	      ts.message_boards[receiver]->publish (msg[i]);
	    }
	  }
	}
      }
    }
  }
  // Ende Broadcast-Modus


  // Owns Ball Eigenschaften kommunizieren
  bool team_owns = false;
  unsigned int owner = 99999;
  for (unsigned int sender=0; sender<num_robots; sender++)
    if (ts.robot_states[sender]->in_game)
#if RCPLAYER_OWNER
      if (ts.robot_states[sender]->ball_pos.pos_known==BallLocation::known && ts.robot_states[sender]->playertype=="RCPlayer")
#else
      if (ts.robot_states[sender]->ball_pos.pos_known==BallLocation::known)
#endif
      {
	Vec robot_ball = (ts.robot_states[sender]->ball_pos.pos-ts.robot_states[sender]->robot_pos.pos)/ts.robot_states[sender]->robot_pos.heading;
	if (robot_ball.length()<500 && (robot_ball.angle()-Angle::quarter).in_between(-Angle::sixth, Angle::sixth)) {
	  team_owns = true;
	  owner = sender;
	}
      }
  if (owns_ball_mode && team_owns) {
    for (unsigned int receiver=0; receiver<num_robots; receiver++)
      if (receiver!=owner) ts.message_boards[receiver]->publish_stream() << "OwnsBall!\n";
  }
  // Ende OwnsBall

#if 0
  // NearBall Eigenschaft herausfinden
  if (owns_ball_mode) {
    bool nearBall= false;
    unsigned int nearBallRobot = 99999;
    for (unsigned int sender=0; sender<num_robots; sender++)
      if (ts.robot_states[sender]->in_game)
	if (ts.robot_states[sender]->ball_pos.pos_known==BallLocation::known) {
	  Vec robot_ball = (ts.robot_states[sender]->ball_pos.pos-ts.robot_states[sender]->robot_pos.pos)/ts.robot_states[sender]->robot_pos.heading;
	  if (robot_ball.length()<1000 && (robot_ball.angle()-Angle::quarter).in_between(-Angle::sixth, Angle::sixth)) {
	    nearBall = true;
	    nearBallRobot = sender;
	  }
	}
	 
	
	
    if (nearBall) {
      for (unsigned int receiver=0; receiver<num_robots; receiver++)
	if (receiver!=nearBallRobot) ts.message_boards[receiver]->publish_stream() << "NearBall!\n";
    }
  }
  // Ende NearBall

#else
  
  // NearBall Eigenschaft herausfinden
  if (owns_ball_mode) {
    bool nearBall= false;
    unsigned int nearBallRobot = 99999;
    double nearest = 999999;
    for (unsigned int sender=0; sender<num_robots; sender++)
#if RCPLAYER_OWNER
      if (ts.robot_states[sender]->in_game && ts.robot_states[sender]->playertype=="RCPlayer")
#else
      if (ts.robot_states[sender]->in_game)
#endif
      {
	if (ts.robot_states[sender]->ball_pos.pos_known==BallLocation::known) {
	  double lengthToBall;
	  lengthToBall=(ts.robot_states[sender]->ball_pos.pos-ts.robot_states[sender]->robot_pos.pos).length();
	   if (nearest>lengthToBall) {
	    nearest=lengthToBall;
	    nearBall = true;
	    nearBallRobot = sender;
	    }
	  }
	}	
    if (nearBall) {
      for (unsigned int receiver=0; receiver<num_robots; receiver++)
	if (receiver!=nearBallRobot) ts.message_boards[receiver]->publish_stream() << "NearBall!\n";
    }
  }
  // Ende NearBall

#endif
  
  
  // Ball kommunizieren im comm_ball_mode
  if (comm_ball_mode) {
    bool ball_found=false;
    unsigned int nearest_robot=99999;
    double smallest_dist=1e99;
    for (unsigned int sender=0; sender<num_robots; sender++)
      if (ts.robot_states[sender]->in_game)
	if (ts.robot_states[sender]->ball_pos.pos_known==BallLocation::known) {
	  double len = (ts.robot_states[sender]->ball_pos.pos-ts.robot_states[sender]->robot_pos.pos).length();
	  if (len<smallest_dist) {
	    smallest_dist=len;
	    nearest_robot=sender;
	    ball_found=true;
	  }
	}
    if (ball_found) {
      Vec comm_ball = ts.robot_states[nearest_robot]->ball_pos.pos;
      for (unsigned int receiver=0; receiver<num_robots; receiver++) {
	ts.message_boards[receiver]->publish_stream() << "Ball: " << static_cast<int>(comm_ball.x) << ' ' << static_cast<int>(comm_ball.y) << '\n';
      }
    }
  }
  // Ende Ball kommunizieren


  // Rollenverhalten und Teammanagement
  for (unsigned int i=0; i<num_robots; i++) {
    bool b = ts.robot_states[i]->in_game && ts.robot_states[i]->comm_started;
    if (!is_active[i] && b) activation_time[i].update();
    is_active[i] = b;
    string mline = ts.message_boards[i]->scan_for_prefix ("NewRole:");
    if (mline.length()>0) {
      deque<string> parts;
      split_string (parts, mline);
      if (mline.size()>1) {
	desired_roles[i]=actual_roles[i]=parts[1];
	if (!role_known[i]) {
	  role_known[i]=true;
	}
      }
    }
    if (ts.robot_states[i]->comm_started && ts.robot_states[i]->playertype=="RCPlayer" && !role_known[i])
      ts.message_boards[i]->publish_stream() << "WhichRole?\n";
    if (ts.robot_states[i]->playertype!="RCPlayer" || !ts.robot_states[i]->comm_started)
      role_known[i]=false;
  }

  // Hier die Logik des Team-Rollenwechsel:
  if (rc_player_dynamic_role_change_mode.length()>0 && !team_owns) {
    unsigned int num_active_rcplayers=0;
    vector<bool> is_active_rcplayer (num_robots);
    for (unsigned int i=0; i<num_robots; i++)
      if (ts.robot_states[i]->playertype=="RCPlayer" && is_active[i]) {
	num_active_rcplayers++;
	is_active_rcplayer[i]=true;
      } else {
	is_active_rcplayer[i]=false;
      }
    vector<string> policy_roles (num_active_rcplayers);
    Policy& cpol = *policies[policies_index(rc_player_dynamic_role_change_mode)];
    cpol.update (ts);   // evtl. Rollen-Rollenwechsel
    for (unsigned int i=0; i<num_active_rcplayers; i++)
      policy_roles[i] = (cpol.get_roles())(num_active_rcplayers-1, i);

    // aus der policy_roles Liste alle Rollen loeschen, die durch neu aktivierte RCPlayer belegt sind
    Time now;
    for (unsigned int i=0; i<num_robots; i++)
      if (is_active_rcplayer[i] && now.diff_msec(activation_time[i])<1000) {
	if (role_known[i]) {
	  vector<string>::iterator it = find (policy_roles.begin(), policy_roles.end(), actual_roles[i]);
	  if (it!=policy_roles.end()) {
	    policy_roles.erase (it);
	  }
	  is_active_rcplayer[i]=false;
	  num_active_rcplayers--;
	}
      }

    // aus der policy_roles Liste alle Rollen loeschen, die von aktiven RCPlayern belegt sind
    for (unsigned int i=0; i<num_robots; i++)
      if (is_active_rcplayer[i]) {
	if (role_known[i]) {
	  vector<string>::iterator it = find (policy_roles.begin(), policy_roles.end(), actual_roles[i]);
	  if (it!=policy_roles.end()) {
	    policy_roles.erase (it);
	    is_active_rcplayer[i]=false;
	    num_active_rcplayers--;
	  }
	}
      }
    // Wenn Rollen uebriggeblieben sind, diese vergeben
    while (policy_roles.size()>0 && num_active_rcplayers>0) {
      // Roboter nach ihren Positionen von hinten nach vorne ordnen
      unsigned int in=0;
      vector<pair<unsigned int,double> > indeces (num_active_rcplayers);
      for (unsigned int i=0; i<num_robots; i++) {
	if (is_active_rcplayer[i]) {
	  indeces[in].first=i;
	  indeces[in].second=ts.robot_states[i]->team*ts.robot_states[i]->tribots_team.own_half*ts.robot_states[i]->robot_pos.pos.y;
	  in++;
	}
      }
      for (unsigned int i=0; i<indeces.size(); i++) // Blasen-Sortierung
	for (unsigned int j=1; j<indeces.size(); j++)
	  if (indeces[j].second<indeces[j-1].second) {
	    unsigned int swi=indeces[j].first;
	    double swy=indeces[j].second;
	    indeces[j].first=indeces[j-1].first;
	    indeces[j].second=indeces[j-1].second;
	    indeces[j-1].first=swi;
	    indeces[j-1].second=swy;
	  }

      for (unsigned int i=0; i<indeces.size(); i++) {
	desired_roles[indeces[i].first] = policy_roles[0];
	cerr << "Automatische Rollenzuweisung: " << robot_ids[indeces[i].first] << " bekommt Rolle " << policy_roles[0] <<'\n';
	policy_roles.erase (policy_roles.begin());
	is_active_rcplayer[indeces[i].first]=false;
	num_active_rcplayers--;	
      }
    }
  }
  // Ende Rollenwechsel Logik

  for (unsigned int i=0; i<num_robots; i++) {
    if (ts.robot_states[i]->comm_started && ts.robot_states[i]->playertype=="RCPlayer" && role_known[i] && desired_roles[i]!=actual_roles[i]) {
      ts.message_boards[i]->publish_stream() << "ChangeRole: " << desired_roles[i] << '\n';
    }
  }

  // die Rollenverteilung in einen string schreiben
  role_description = "";
  for (unsigned int i=0; i<num_robots; i++) {
    if (ts.robot_states[i]->comm_started) {
      role_description += robot_ids[i];
      if (is_active[i])
	role_description+=string("[aktiv]");
      else
	role_description+=string("[inaktiv]");
      role_description += string(": ") + ts.robot_states[i]->playertype;
      if (ts.robot_states[i]->playertype=="RCPlayer") {
	if (role_known[i]) 
	  role_description+= string(" / ") + actual_roles[i];
	else
	  role_description+= string(" / Rolle unbekannt");
      }
      role_description+=string("\n");
    }
  }
  // Ende Rollenwechsel und Teamstrategie
  
#if COMM_KICKOFF_VARIANTS

  // Kick-Off-Positionen kommunizieren
  if (ts.refstate==Tribots::preOwnKickOff || ts.refstate==Tribots::preOpponentKickOff) {
    int kindex = ko_policies_index (ts.refstate==Tribots::preOwnKickOff ? kick_off_own_policy : kick_off_opponent_policy);
    if (kindex>=0) {
      
      // RCPlayer erfassen
      unsigned int num_active_rcplayers=0;
      vector<bool> is_active_rcplayer (num_robots);
      for (unsigned int i=0; i<num_robots; i++)
	if (ts.robot_states[i]->playertype=="RCPlayer" && is_active[i]) {
	  num_active_rcplayers++;
	  is_active_rcplayer[i]=true;
	} else {
	  is_active_rcplayer[i]=false;
	}

      // Die Liste der Positionen durchgehen und vergeben
      KickOffPolicy red (ko_policies[kindex]);
      for (unsigned  int i=0; i<num_robots; i++) {
	if (is_active_rcplayer[i]) {
	  bool found=false;
	  for (unsigned int j=0; j<red.pos[num_active_rcplayers-1].size(); j++) {
	    if (red.pos[num_active_rcplayers-1][j].role == actual_roles[i]) {
	      cerr << "Kommuniziere KickOff-Position an " << robot_ids[i] << ": " << red.pos[num_active_rcplayers-1][j].pos.x << ' ' << red.pos[num_active_rcplayers-1][j].pos.y << ' ' << red.pos[num_active_rcplayers-1][j].heading.get_deg() << '\n';
	      ts.message_boards[j]->publish_stream() << "KickOffPos: " << red.pos[num_active_rcplayers-1][j].pos.x << ' ' << red.pos[num_active_rcplayers-1][j].pos.y << ' ' << red.pos[num_active_rcplayers-1][j].heading.get_deg() << '\n';
	      red.pos[num_active_rcplayers-1].erase(red.pos[num_active_rcplayers-1].begin()+j);
	      found=true;
	      break;
	    }
	  }
	  if (!found)
	    cerr << "ACHTUNG: Konnte Spieler " << robot_ids[i] << " mit Rolle " << actual_roles[i] << " keine KickOff-Position zuweisen!\n";
	}
      }
    }
  }
  // Ende: Kick-Off-Positionen kommunizieren

#endif

}

void Coach::change_player_role (const char* robotid, const char* roleid) throw () {
  unsigned int i=0;
  while (i<robot_ids.size()) {
    if (robot_ids[i] == string(robotid)) break;
    else i++;
  }
  if (i==robot_ids.size()) {
    cerr << "ACHTUNG: Roboter mit ID " << robotid << " unbekannt.\n";
    return;  // keinen Roboter mit uebergebener ID
  }
  desired_roles[i]=roleid;
}

int Coach::policies_index (const string& s) throw() {
  for (unsigned int i=0; i<policies.size(); i++)
    if (policies[i]->get_name()==s)
      return i;
  return -1;
}

vector<string> Coach::get_list_of_roles () const throw () {
  vector<string> roles (Tribots::num_roles);
  for (unsigned int i=0; i<Tribots::num_roles; i++)
    roles[i]=playerRoleNames[i];
  return roles;
}
 
vector<string> Coach::get_list_of_policies () const throw () {
  vector<string> policy_ids (policies.size());
  for (unsigned int i=0; i<policies.size(); i++) {
    policy_ids[i]=policies[i]->get_name();
  }
  return policy_ids;
}

std::string Coach::get_players_role (const std::string& rid) const throw () {
  unsigned int index=0;
  while (index<robot_ids.size()) {
    if (rid==robot_ids[index])
      break;
    index++;
  }
  if (index==robot_ids.size())
    return string("");
  else
    return actual_roles[index];
}

const std::string& Coach::get_role_description () const throw () {
  return role_description;
}     

Coach::KickOffPolicy::KickOffPolicy () throw (std::bad_alloc) : pos(max_player, max_player) {;}

Coach::KickOffPolicy::KickOffPolicy(const KickOffPolicy& src) throw (std::bad_alloc) : name (src.name), pos(src.pos) {;}

int Coach::ko_policies_index (const std::string& s) throw () {
  for (unsigned int i=0; i<ko_policies.size(); i++)
    if (ko_policies[i].name==s)
      return i;
  return -1;
}

bool Coach::kick_off_own_procedure (const std::string& s) throw () {
  if (ko_policies_index(s)>=0) {
    kick_off_own_policy=s;
    return true;
  }
  return false;
}

bool Coach::kick_off_opponent_procedure (const std::string& s) throw () {
  if (ko_policies_index(s)>=0) {
    kick_off_opponent_policy=s;
    return true;
  }
  return false;
}

const std::string& Coach::kick_off_own_procedure () const throw () {
  return kick_off_own_policy;
}

const std::string& Coach::kick_off_opponent_procedure () const throw () {
  return kick_off_opponent_policy;
}

std::vector<std::string> Coach::get_list_of_kick_off_procedures () const throw () {
  vector<string> dest (ko_policies.size());
  for (unsigned int i=0; i<ko_policies.size(); i++)
    dest[i] = ko_policies[i].name;
  return dest;
}

