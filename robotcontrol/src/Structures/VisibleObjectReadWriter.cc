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


#include "VisibleObjectReadWriter.h"
#include "../Fundamental/stringconvert.h"

using namespace Tribots;
using namespace std;


VisibleObjectWriter::VisibleObjectWriter (std::ostream& d) throw () : dest(d) {;}

VisibleObjectWriter::~VisibleObjectWriter () throw () {
  dest << std::flush;
}

void VisibleObjectWriter::write (unsigned long int tav, unsigned long int trec, const VisibleObject& obj) throw () {
  dest << tav << '\t' 
       << trec << '\t' 
       << obj.pos.x << '\t' 
       << obj.pos.y << '\t' 
       << obj.object_type << '\t' 
       << obj.width << '\n';
}

void VisibleObjectWriter::write (unsigned long int tav, unsigned long int trec, const VisibleObjectList& obj) throw () {
  for (unsigned int i=0; i<obj.objectlist.size(); i++) 
    dest << tav << '\t' 
	 << trec << '\t' 
	 << obj.objectlist[i].pos.x << '\t' 
	 << obj.objectlist[i].pos.y << '\t' 
	 << obj.objectlist[i].object_type << '\t' 
	 << obj.objectlist[i].width << '\n';
}


VisibleObjectReader::VisibleObjectReader (std::istream& s) throw () : src(s), next(0) {
  if (src.good()) src >> next;
}

bool VisibleObjectReader::next_timestamp (unsigned long int& t) const throw () {
  if (src.eof())
    return false;
  t=next;
  return true;
}

bool VisibleObjectReader::read_until (unsigned long int& tav, unsigned long int& trec, VisibleObjectList& obj, unsigned long int tuntil) throw () {
  tav=next;
  bool read_anything = false;
  obj.objectlist.clear();
  while (!src.eof() && next<=tuntil) {
    if (tav!=next) {
      obj.objectlist.clear();
      tav=next;
    }
    string line;
    deque<string> parts;
    getline (src, line);
    split_string (parts, line);
    src >> next;
    if (parts.size()>=5) {
      VisibleObject dest;
      string2ulint (trec, parts[0]);
      string2double (dest.pos.x, parts[1]);
      string2double (dest.pos.y, parts[2]);
      string2int (dest.object_type, parts[3]);
      string2double (dest.width, parts[4]);
      obj.objectlist.push_back (dest);
      read_anything=true;
    }
  }
  obj.timestamp.set_msec (trec);
  return read_anything;
}
