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


#include "Player.h"
#include "PlayerFactory.h"
#include "../Structures/Journal.h"
#include <cstring>

using namespace std;

const char* Tribots::Player::get_player_type () const throw () { return player_descriptor; }

Tribots::Player::~Player () throw () {
  delete the_player;
  delete [] player_descriptor;
}

Tribots::Player::Player (const ConfigReader& vread) throw (TribotsException, bad_alloc) : the_player(NULL), configuration_list(vread) {
  string confline;
  if (vread.get("player_type", confline)<=0) {
    JERROR("no config line \"player_type\" found");
    throw Tribots::InvalidConfigurationException ("player_type");
  }
  really_change_player_type (confline.c_str(), vread);
}

bool Tribots::Player::change_player_type (const char* pt) throw () {
  return change_player_type (pt, configuration_list);
}

bool Tribots::Player::change_player_type (const char* pt, const ConfigReader& vread) throw () {
  try{
    really_change_player_type (pt, vread);
  }catch(bad_alloc&){
    JWARNING("Change of player type failed due to lack of memory");
    return false;
  }catch(TribotsException&){
    JWARNING("Change of player type failed");
    return false;
  }
  return true;
}

void Tribots::Player::really_change_player_type (const char* pt, const ConfigReader& vread) throw (TribotsException, bad_alloc) {
  PlayerType* new_player;
  char* new_descriptor;
  try{
    string plt (pt);
    new_player = PlayerFactory::get_player_factory()->get_player (plt, vread);
  }catch(invalid_argument){
    throw Tribots::InvalidConfigurationException ("player_type");
  }

  new_descriptor = new char [strlen(pt)+1];
  strcpy(new_descriptor,pt);
  if (the_player!=NULL) {
    delete the_player;
    delete [] player_descriptor;
  }

  the_player=new_player;
  player_descriptor=new_descriptor;
}

void Tribots::Player::getPlayerTypeList(std::vector<std::string> &ptl)
{
  PlayerFactory::get_player_factory()->player_list(ptl);
}
