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


#include "VisualContainer.h"


using namespace Tribots;

VisualContainer::VisualContainer () throw (std::bad_alloc) {
  // nachfolgende Erweiterungen der Vektoren moeglichst vermeiden
  lines.objectlist.reserve (300);
  goals.objectlist.reserve (20);
  balls.objectlist.reserve (5);
  obstacles.objectlist.reserve (100);
  all.objectlist.reserve (400);
  lines.objectlist.clear();
  goals.objectlist.clear();
  balls.objectlist.clear();
  obstacles.objectlist.clear();
  all.objectlist.clear();
  Time null;
  null.set_msec(0);
  lines.timestamp=obstacles.timestamp=goals.timestamp=balls.timestamp=lines.timestamp=null;
}

VisualContainer::~VisualContainer () throw () {;}

void VisualContainer::add (const VisibleObjectList& vis) throw (std::bad_alloc) {
  if (lines.timestamp!=vis.timestamp) {
    lines.timestamp=balls.timestamp=goals.timestamp=obstacles.timestamp=all.timestamp=vis.timestamp;
    lines.objectlist.clear();
    balls.objectlist.clear();
    goals.objectlist.clear();
    obstacles.objectlist.clear();
    all.objectlist.clear();
  }

  std::vector<VisibleObject>::const_iterator it = vis.objectlist.begin();
  std::vector<VisibleObject>::const_iterator itend = vis.objectlist.end();
  while (it<itend) {
    if (it->object_type==VisibleObject::white_line)
      lines.objectlist.push_back (*it);
    else if (it->object_type==VisibleObject::ball)
      balls.objectlist.push_back (*it);
    else if (it->object_type==VisibleObject::obstacle || it->object_type==VisibleObject::teammate || it->object_type==VisibleObject::opponent)
      obstacles.objectlist.push_back (*it);
    else if (it->object_type==VisibleObject::blue_goal || it->object_type==VisibleObject::yellow_goal || it->object_type==VisibleObject::blue_goal_post_left || it->object_type==VisibleObject::yellow_goal_post_left || it->object_type==VisibleObject::blue_goal_post_right || it->object_type==VisibleObject::yellow_goal_post_right)
      goals.objectlist.push_back (*it);
    it++;
  }
  all.objectlist.insert (all.objectlist.end(), vis.objectlist.begin(), vis.objectlist.end());
}

void VisualContainer::add (const VisibleObject& obj, Time t) throw (std::bad_alloc) {
  if (lines.timestamp!=t) {
    lines.timestamp=balls.timestamp=goals.timestamp=obstacles.timestamp=all.timestamp=t;
    lines.objectlist.clear();
    balls.objectlist.clear();
    goals.objectlist.clear();
    obstacles.objectlist.clear();
    all.objectlist.clear();
  }

  if (obj.object_type==VisibleObject::white_line)
    lines.objectlist.push_back (obj);
  else if (obj.object_type==VisibleObject::ball)
    balls.objectlist.push_back (obj);
  else if (obj.object_type==VisibleObject::obstacle || obj.object_type==VisibleObject::teammate || obj.object_type==VisibleObject::opponent)
    obstacles.objectlist.push_back (obj);
  else if (obj.object_type==VisibleObject::blue_goal || obj.object_type==VisibleObject::yellow_goal || obj.object_type==VisibleObject::blue_goal_post_left || obj.object_type==VisibleObject::yellow_goal_post_left || obj.object_type==VisibleObject::blue_goal_post_right || obj.object_type==VisibleObject::yellow_goal_post_right)
    goals.objectlist.push_back (obj);
  all.objectlist.push_back (obj);
}

const VisibleObjectList& VisualContainer::get_lines () const throw () { return lines; }

const VisibleObjectList& VisualContainer::get_goals () const throw () { return goals; }

const VisibleObjectList& VisualContainer::get_balls () const throw () { return balls; }

const VisibleObjectList& VisualContainer::get_obstacles () const throw () { return obstacles; }

const VisibleObjectList& VisualContainer::get_all () const throw () { return all; }

void VisualContainer::clear () throw () {
  lines.objectlist.clear();
  goals.objectlist.clear();
  balls.objectlist.clear();
  obstacles.objectlist.clear();
  all.objectlist.clear();
}

void VisualContainer::shuffle_lines () throw (std::bad_alloc) {
  if (lines.objectlist.size()>0) {
    VisibleObjectList old = lines;
    old.objectlist[0].object_type = VisibleObject::unknown;
    unsigned int count = 1;
    unsigned int base = 2;
    unsigned int n= lines.objectlist.size();
    while (count < n) {
      for (unsigned int i=1; i<base; i+=2) {
        unsigned int j = (i*n)/base;
        if (old.objectlist[j].object_type == VisibleObject::white_line) {
          lines.objectlist [count] = old.objectlist[j];
          old.objectlist[j].object_type = VisibleObject::unknown;
          count++;
        }
      }
      base *= 2;
    }
  }
}

void VisualContainer::prune_lines (unsigned int n) throw () {
  if (lines.objectlist.size()>n) 
    lines.objectlist.resize (n);
}
