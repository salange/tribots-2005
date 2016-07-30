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


#include "Policy.h"

using namespace TribotsTools;
using namespace Tribots;
using namespace std;

namespace{
  const unsigned int max_player = 4;  // maximale Anzahl RCPlayer gleichzeitig aktiv
}


StaticPolicy::StaticPolicy (const Tribots::ConfigReader& cfg, const char* nm) throw (std::bad_alloc, Tribots::TribotsException) : Policy(nm), roles(max_player, max_player) {
  for (unsigned int i=0; i<max_player; i++)
    for (unsigned int j=0; j<max_player; j++)
      roles(i,j)="";
  vector<string> change_lines;
  for (unsigned int i=0; i<max_player; i++) {
    cfg.get ((name+string("::")+static_cast<char>(i+'1')).c_str(), change_lines);
    for (unsigned int j=0; j<=i; j++)
      if (change_lines.size()>=j+1) roles(i,j) = change_lines[j];
      else cerr << "ACHTUNG: Rollenwechselstrategie " << name << " undefiniert fuer den " << j+1 << "-ten unter " << i+1 << " Spielern\n";
  }
}

StaticPolicy::StaticPolicy (const StaticPolicy& src) throw (std::bad_alloc) : Policy(src), roles (src.roles) {;}

Policy* StaticPolicy::clone () const throw (std::bad_alloc) {
  return new StaticPolicy (*this);
}

const Tribots::Table<std::string> StaticPolicy::get_roles () const throw () {
  return roles;
}

DynamicDefendOffendPolicy::DynamicDefendOffendPolicy (const Tribots::ConfigReader& cfg, const char* nm) throw (std::bad_alloc, Tribots::TribotsException) : Policy (nm), defend_policy (NULL), offend_policy (NULL) {
  string policypolicy;
  cfg.get ((nm+string("::defensive_policy")).c_str(), policypolicy);
  defend_policy = new StaticPolicy (cfg, policypolicy.c_str());
  cfg.get ((nm+string("::offensive_policy")).c_str(), policypolicy);
  offend_policy = new StaticPolicy (cfg, policypolicy.c_str());
  hyst_y1=-200;
  hyst_y2=200;
  was_defend=true;
}

DynamicDefendOffendPolicy::DynamicDefendOffendPolicy (const DynamicDefendOffendPolicy& src) throw (std::bad_alloc) : Policy(src), hyst_y1(src.hyst_y1), hyst_y2(src.hyst_y2), defend_policy(src.defend_policy->clone()), offend_policy(src.offend_policy->clone()), was_defend(src.was_defend) {;}

DynamicDefendOffendPolicy::~DynamicDefendOffendPolicy () throw () {
  if (offend_policy) delete offend_policy;
  if (defend_policy) delete defend_policy;
}

Policy* DynamicDefendOffendPolicy::clone () const throw (std::bad_alloc) {
  return new DynamicDefendOffendPolicy (*this);
}

void DynamicDefendOffendPolicy::update (RemoteTeamState& ts) throw () {
  // Ballposition in Erfahrung bringen
  bool ball_found=false;
  unsigned int nearest_robot=99999;
  double smallest_dist=1e99;
  unsigned int num_robots=ts.robot_states.size();
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
    Vec ball = ts.robot_states[nearest_robot]->team*ts.robot_states[nearest_robot]->tribots_team.own_half*ts.robot_states[nearest_robot]->ball_pos.pos;
    if (ball.y<hyst_y1 && !was_defend) {
      was_defend=true;
      cerr << "schalte auf defensive Strategie um\n";
    } else if (ball.y>hyst_y2 && was_defend) {
      was_defend=false;
      cerr << "schalte auf offensive Strategie um\n";
    }
  }
}

const Tribots::Table<std::string> DynamicDefendOffendPolicy::get_roles () const throw () {
  if (was_defend)
    return defend_policy->get_roles();
  else
    return offend_policy->get_roles();
}

