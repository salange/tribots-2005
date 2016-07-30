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


#include "LocationShortTimeMemory.h"

using namespace Tribots;

// Konvention zur Speicherung der Informationen in den Ringpuffern:
// - der Anker weist stets auf den neuesten Eintrag
// - Anker+1 zeigt auf den zweit-neuesten Eintrag, Anker+2 auf den dritt-neuesten usw.

namespace {
  const unsigned int num=15;  // Groesse der Ringpuffer

  template<class Triple>
  int search_ringbuffer (const RingBuffer<Triple>& buffer, unsigned long int cycle, Time t) throw() {
    // Ringpuffer nach Zyklus und Zeitstempel untersuchen, -1 bei Misserfolg 
    for (unsigned int i=0; i<num; i++)
      if (buffer[i].cycle==cycle && buffer[i].timestamp.get_msec()==t.get_msec())
	return i;
    return -1;
  }
}

LocationShortTimeMemory::LocationShortTimeMemory (const WorldModelTypeBase& w) throw (std::bad_alloc) : wm(w), robots_pure(num), robots_interacted(num), balls_pure(num), balls_interacted(num) {
  for (unsigned int i=0; i<num; i++)
    robots_pure[i].cycle=robots_interacted[i].cycle=balls_pure[i].cycle=balls_interacted[i].cycle=0;
  obstacles.cycle=0;
}

const RobotLocation& LocationShortTimeMemory::get_robot_location (Time t, bool b) throw () {
  if (b) {
    int index = search_ringbuffer (robots_interacted, wm.get_game_state().cycle_num, t);
    if (index>=0)
      return robots_interacted[index].value;
    else {
      get_interacted (t);
      return robots_interacted.get().value;
    }
  } else {
    int index = search_ringbuffer (robots_pure, wm.get_game_state().cycle_num, t);
    if (index>=0)
      return robots_pure[index].value;
    else {
      robots_pure.step(-1);
      robots_pure.get().value = wm.get_robot(t);
      robots_pure.get().cycle = wm.get_game_state().cycle_num;
      robots_pure.get().timestamp = t;
      return robots_pure.get().value;
    }
  }
}

const BallLocation& LocationShortTimeMemory::get_ball_location (Time t, bool b) throw () {
  if (b) {
    int index = search_ringbuffer (balls_interacted, wm.get_game_state().cycle_num, t);
    if (index>=0)
      return balls_interacted[index].value;
    else {
      get_interacted (t);
      return balls_interacted.get().value;
    }
  } else {
    int index = search_ringbuffer (balls_pure, wm.get_game_state().cycle_num, t);
    if (index>=0)
      return balls_pure[index].value;
    else {
      balls_pure.step(-1);
      balls_pure.get().value = wm.get_ball(t);
      balls_pure.get().cycle = wm.get_game_state().cycle_num;
      balls_pure.get().timestamp = t;
      return balls_pure.get().value;
    }
  }
}

const ObstacleLocation& LocationShortTimeMemory::get_obstacle_location (Time t, bool b) throw () {
  if (obstacles.cycle != wm.get_game_state().cycle_num) {
    obstacles.value = wm.get_obstacles (t);
    obstacles.cycle = wm.get_game_state().cycle_num;
    obstacles.timestamp = t;
  }
  return obstacles.value;
}

void LocationShortTimeMemory::get_interacted (Time t) {
  Time tvis = wm.get_timestamp_latest_update ();
  const RobotLocation& robot_vis = get_robot_location (tvis, false);
  const BallLocation& ball_vis = get_ball_location (tvis, false);

  get_obstacle_location (t, false);
  RobotLocation robot_exec = get_robot_location (t, false);
  BallLocation ball_exec = get_ball_location (t,false);

  interaction_manager.get (robot_exec, ball_exec, obstacles.value, robot_vis, ball_vis, static_cast<unsigned int>(t.diff_msec(tvis)));
  robots_interacted.step(-1);
  robots_interacted.get().value = robot_exec;
  robots_interacted.get().cycle = wm.get_game_state().cycle_num;
  robots_interacted.get().timestamp = t;
  balls_interacted.step(-1);
  balls_interacted.get().value = ball_exec;
  balls_interacted.get().cycle = wm.get_game_state().cycle_num;
  balls_interacted.get().timestamp = t;
}
