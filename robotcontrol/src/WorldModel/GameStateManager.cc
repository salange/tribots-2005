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


#include "GameStateManager.h"
#include "WorldModel.h"

using namespace Tribots;
using namespace std;

GameStateManager::GameStateManager (const ConfigReader& reader, const FieldGeometry& fg) throw () : rsm (fg) {
  gs.refstate = stopRobot;
  rsm.set_state (gs.refstate);
  gs.in_game = false;
  gs.refbox_signal = SIGstart;
  gs.own_score = gs.opponent_score = 0;
  reader.get ("loop_time", gs.intended_cycle_time);
  gs.actual_cycle_time = gs.intended_cycle_time;
  gs.cycle_num = 0;
}

GameStateManager::~GameStateManager() throw () {;}

const GameState& GameStateManager::get () const throw () {
  return gs;
}

void GameStateManager::update () throw () {
  Time now;
  now.add_msec (-30);
  BallLocation bp = MWM.get_ball_location (now, false);
  RobotLocation rp = MWM.get_robot_location (now, false);
  rsm.update (bp.pos, rp.pos, (bp.pos_known==BallLocation::known));
  gs.refstate = rsm.get_state ();
}

void GameStateManager::init_cycle (Time now, Time exp_exec) throw () {
  // Mittlere Zykluszeit anpassen und Iterationsnummer inkrementieren
  if (gs.cycle_num++>0)
    gs.actual_cycle_time=0.98*gs.actual_cycle_time+0.02*now.diff_msec(gs.cycle_start_time);
  gs.cycle_start_time = now;
  gs.expected_execution_time = exp_exec;
}

void GameStateManager::update (RefboxSignal sig) throw () {
  Time now;
  BallLocation bp = MWM.get_ball_location (now, false);
  RefboxSignal sig2 = rsm.update (sig, bp.pos, (bp.pos_known==BallLocation::known));
  if (sig2==SIGownGoalScored)
    gs.own_score++;
  if (sig2==SIGopponentGoalScored)
    gs.opponent_score++;
  gs.latest_update.update();
  gs.refbox_signal = sig2;
  gs.refstate = rsm.get_state ();
}
  
void GameStateManager::set_in_game (bool b) throw () {
  gs.in_game=b;
}

