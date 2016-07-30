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


#include "VisibleObject.h"

const int Tribots::VisibleObject::unknown=0;
const int Tribots::VisibleObject::ball=1;
const int Tribots::VisibleObject::blue_goal=2;
const int Tribots::VisibleObject::yellow_goal=3;
const int Tribots::VisibleObject::blue_pole=4;
const int Tribots::VisibleObject::yellow_pole=5;
const int Tribots::VisibleObject::teammate=6;
const int Tribots::VisibleObject::opponent=7;
const int Tribots::VisibleObject::obstacle=8;
const int Tribots::VisibleObject::white_line=9;
const int Tribots::VisibleObject::blue_goal_post_left=10;
const int Tribots::VisibleObject::blue_goal_post_right=11;
const int Tribots::VisibleObject::yellow_goal_post_left=12;
const int Tribots::VisibleObject::yellow_goal_post_right=13;


Tribots::VisibleObject::VisibleObject () throw () {
  pos.x=0;
  pos.y=0;
  object_type=0;
  width=0;
}

Tribots::VisibleObject::VisibleObject (Vec pos1, int ot1, double w) throw () : pos(pos1), object_type(ot1), width(w) {;}

Tribots::VisibleObject::VisibleObject (const VisibleObject& src) throw () : pos(src.pos), object_type(src.object_type), width(src.width) {;}

const Tribots::VisibleObject& Tribots::VisibleObject::operator= (const Tribots::VisibleObject& src) throw () {
  pos=src.pos;
  object_type=src.object_type;
  width=src.width;
  return (*this);
}

Tribots::VisibleObjectList::VisibleObjectList (const Tribots::VisibleObjectList& src) throw (std::bad_alloc) : timestamp (src.timestamp), objectlist (src.objectlist) {;}

const Tribots::VisibleObjectList& Tribots::VisibleObjectList::operator= (const Tribots::VisibleObjectList& src) throw (std::bad_alloc) {
  timestamp = src.timestamp;
  objectlist = src.objectlist;
  return *this;
}                                              

void Tribots::VisibleObjectList::writeAt(std::ostream &stream) const
{
  for (unsigned int i=0; i< objectlist.size(); i++)
    stream << "("<< objectlist[i].pos.x << "," << objectlist[i].pos.y << ";" 
	   << objectlist[i].width << "," << objectlist[i].object_type <<  ")";
}

int Tribots::VisibleObjectList::readFrom(std::istream &stream)
{
  int n=0;
  objectlist.clear();
 
  do {
    Tribots::VisibleObject o;
    
    stream.ignore(std::numeric_limits<std::streamsize>::max(),'(');
    stream >> o.pos.x;
    if (stream.fail() || stream.eof()) break;
    stream.ignore(std::numeric_limits<std::streamsize>::max(),',');
    stream >> o.pos.y;
    if (stream.fail() || stream.eof()) break;
    stream.ignore(std::numeric_limits<std::streamsize>::max(),';');
    stream >> o.width;
    if (stream.fail() || stream.eof()) break;
    stream.ignore(std::numeric_limits<std::streamsize>::max(),',');
    stream >> o.object_type;
    if (stream.fail() || stream.eof()) break;
    stream.ignore(std::numeric_limits<std::streamsize>::max(),')');
    
    objectlist.push_back(o);
    n++;
  } while (!(stream.fail() || stream.eof()));
  
  return n;
}
