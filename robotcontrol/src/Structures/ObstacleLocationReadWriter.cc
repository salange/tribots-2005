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


#include "ObstacleLocationReadWriter.h"
#include "../Fundamental/stringconvert.h"

using namespace Tribots;
using namespace std;


ObstacleLocationWriter::ObstacleLocationWriter (std::ostream& d) throw () : dest(d) {;}

ObstacleLocationWriter::~ObstacleLocationWriter () throw () {
  dest << std::flush;
}

void ObstacleLocationWriter::write (unsigned long int tav, const ObstacleLocation& obj) throw () {
  vector<Vec>::const_iterator pit = obj.pos.begin();
  vector<Vec>::const_iterator pitend = obj.pos.end();
  vector<double>::const_iterator wit = obj.width.begin();
  while (pit<pitend) {
    dest << tav << '\t' 
	 << pit->x << '\t' 
	 << pit->y << '\t' 
	 << (*wit) << '\n';
    pit++;
    wit++;
  }
}


ObstacleLocationReader::ObstacleLocationReader (std::istream& s) throw () : src(s), next(0) {
  if (src.good()) src >> next;
}

bool ObstacleLocationReader::next_timestamp (unsigned long int& t) const throw () {
  if (src.eof())
    return false;
  t=next;
  return true;
}

bool ObstacleLocationReader::read_until (unsigned long int& tav, ObstacleLocation& obj, unsigned long int tuntil) throw () {
  tav=next;
  bool read_anything = false;
  obj.pos.clear();
  obj.width.clear();
  while (!src.eof() && next<=tuntil) {
    if (tav!=next) {
      obj.pos.clear();
      obj.width.clear();
      tav=next;
    }
    string line;
    deque<string> parts;
    getline (src, line);
    split_string (parts, line);
    src >> next;
    if (parts.size()>=3) {
      Vec p;
      double w;
      string2double (p.x, parts[0]);
      string2double (p.y, parts[1]);
      string2double (w, parts[2]);
      obj.pos.push_back (p);
      obj.width.push_back (w);
      read_anything=true;
    }
  }
  return read_anything;
}
