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


#include "BallLocationReadWriter.h"
#include "../Fundamental/stringconvert.h"

using namespace Tribots;
using namespace std;


BallLocationWriter::BallLocationWriter (std::ostream& d) throw () : dest(d) {;}

BallLocationWriter::~BallLocationWriter () throw () {
  dest << std::flush;
}

void BallLocationWriter::write (unsigned long int tav, unsigned long int t1, const BallLocation& bloc_vis, unsigned long int t2, const BallLocation& bloc_exec) throw () {
  dest << tav << '\t'
       << t1 << '\t'
       << bloc_vis.pos.x << '\t' 
       << bloc_vis.pos.y << '\t' 
       << bloc_vis.velocity.x << '\t' 
       << bloc_vis.velocity.y << '\t' 
       << bloc_vis.quality << '\t'
       << bloc_vis.pos_known << '\t' 
       << bloc_vis.lastly_seen << '\t'
       << t2 << '\t'
       << bloc_exec.pos.x << '\t' 
       << bloc_exec.pos.y << '\t' 
       << bloc_exec.velocity.x << '\t' 
       << bloc_exec.velocity.y << '\t' 
       << bloc_exec.quality << '\t'
       << bloc_exec.pos_known << '\t' 
       << bloc_exec.lastly_seen << '\n';
}


BallLocationReader::BallLocationReader (std::istream& s) throw () : src(s), next(0) {
  if (src.good()) src >> next;
}

bool BallLocationReader::next_timestamp (unsigned long int& t) const throw () {
  if (src.eof())
    return false;
  t=next;
  return true;
}

bool BallLocationReader::read_until (unsigned long int& tav, unsigned long int& t1, BallLocation& bloc_vis, unsigned long int& t2, BallLocation& bloc_exec, unsigned long int tuntil) throw () {
  tav=next;
  bool read_anything = false;
  while (!src.eof() && next<=tuntil) {
    tav=next;
    string line;
    deque<string> parts;
    getline (src, line);
    split_string (parts, line);
    src >> next;
    if (parts.size()>=5 && parts.size()<=7) {
      // altes Format mit nur einem Zeitstempel
      string2double (bloc_vis.pos.x, parts[0]);
      string2double (bloc_vis.pos.y, parts[1]);
      string2double (bloc_vis.velocity.x, parts[2]);
      string2double (bloc_vis.velocity.y, parts[3]);
      string2double (bloc_vis.quality, parts[4]);
      if (parts.size()==7) {
	string2int (bloc_vis.pos_known, parts[5]);
	unsigned long int trans;
	string2ulint (trans, parts[6]);
	bloc_vis.lastly_seen.set_msec (trans);
      } else {
	bloc_vis.pos_known = BallLocation::known;
      }
      bloc_exec=bloc_vis;
      t1=t2=tav;
      read_anything=true;
    } else if (parts.size()>=16) {
      // neues Format mit zwei Positionen und insgesamt 3 Zeitstempeln
      unsigned long int trans;
      string2ulint (t1, parts[0]);
      string2double (bloc_vis.pos.x, parts[1]);
      string2double (bloc_vis.pos.y, parts[2]);
      string2double (bloc_vis.velocity.x, parts[3]);
      string2double (bloc_vis.velocity.y, parts[4]);
      string2double (bloc_vis.quality, parts[5]);
      string2int (bloc_vis.pos_known, parts[6]);
      string2ulint (trans, parts[7]);
      bloc_vis.lastly_seen.set_msec (trans);
      string2ulint (t2, parts[8]);
      string2double (bloc_exec.pos.x, parts[9]);
      string2double (bloc_exec.pos.y, parts[10]);
      string2double (bloc_exec.velocity.x, parts[11]);
      string2double (bloc_exec.velocity.y, parts[12]);
      string2double (bloc_exec.quality, parts[13]);
      string2int (bloc_exec.pos_known, parts[14]);
      string2ulint (trans, parts[15]);
      bloc_exec.lastly_seen.set_msec (trans);
      read_anything=true;
    }
  }
  return read_anything;
}
