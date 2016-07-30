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


#include "RobotLocationReadWriter.h"
#include "../Fundamental/stringconvert.h"

using namespace Tribots;
using namespace std;


RobotLocationWriter::RobotLocationWriter (std::ostream& d) throw () : dest(d) {;}

RobotLocationWriter::~RobotLocationWriter () throw () {
  dest << std::flush;
}

void RobotLocationWriter::write (unsigned long int tav, unsigned long int t1, const RobotLocation& rloc_vis, unsigned long int t2, const RobotLocation& rloc_exec) throw () {
  dest << tav << '\t'
       << t1 << '\t'
       << rloc_vis.pos.x << '\t' 
       << rloc_vis.pos.y << '\t' 
       << rloc_vis.heading.get_rad() << '\t' 
       << rloc_vis.vtrans.x << '\t' 
       << rloc_vis.vtrans.y << '\t' 
       << rloc_vis.vrot << '\t' 
       << rloc_vis.kick << '\t'
       << t2 << '\t'
       << rloc_exec.pos.x << '\t' 
       << rloc_exec.pos.y << '\t' 
       << rloc_exec.heading.get_rad() << '\t' 
       << rloc_exec.vtrans.x << '\t' 
       << rloc_exec.vtrans.y << '\t' 
       << rloc_exec.vrot << '\t' 
       << rloc_exec.kick << '\t'
       << rloc_vis.stuck.robot_stuck << '\n';
}


RobotLocationReader::RobotLocationReader (std::istream& s) throw () : src(s), next(0) {
  if (src.good()) src >> next;
}

bool RobotLocationReader::next_timestamp (unsigned long int& t) const throw () {
  if (src.eof())
    return false;
  t=next;
  return true;
}

bool RobotLocationReader::read_until (unsigned long int& tav, unsigned long int& t1, RobotLocation& rloc_vis, unsigned long int& t2, RobotLocation& rloc_exec, unsigned long int tuntil) throw () {
  tav=next;
  bool read_anything = false;
  while (!src.eof() && next<=tuntil) {
    tav=next;
    string line;
    deque<string> parts;
    getline (src, line);
    split_string (parts, line);
    src >> next;
    if (parts.size()==7) {
      // altes Format mit nur einem Zeitstempel
      double trans;
      string2double (rloc_vis.pos.x, parts[0]);
      string2double (rloc_vis.pos.y, parts[1]);
      string2double (trans, parts[2]);
      rloc_vis.heading.set_rad (trans);
      string2double (rloc_vis.vtrans.x, parts[3]);
      string2double (rloc_vis.vtrans.y, parts[4]);
      string2double (rloc_vis.vrot, parts[5]);
      string2bool (rloc_vis.kick, parts[6]);
      rloc_vis.stuck.robot_stuck = false;
      rloc_exec=rloc_vis;
      t1=t2=tav;
      read_anything=true;
    } else if (parts.size()>=17) {
      // neues Format mit zwei Positionen und insgesamt 3 Zeitstempeln
      double trans;
      string2ulint (t1, parts[0]);
      string2double (rloc_vis.pos.x, parts[1]);
      string2double (rloc_vis.pos.y, parts[2]);
      string2double (trans, parts[3]);
      rloc_vis.heading.set_rad (trans);
      string2double (rloc_vis.vtrans.x, parts[4]);
      string2double (rloc_vis.vtrans.y, parts[5]);
      string2double (rloc_vis.vrot, parts[6]);
      string2bool (rloc_vis.kick, parts[7]);
      string2ulint (t2, parts[8]);
      string2double (rloc_exec.pos.x, parts[9]);
      string2double (rloc_exec.pos.y, parts[10]);
      string2double (trans, parts[11]);
      rloc_exec.heading.set_rad (trans);
      string2double (rloc_exec.vtrans.x, parts[12]);
      string2double (rloc_exec.vtrans.y, parts[13]);
      string2double (rloc_exec.vrot, parts[14]);
      string2bool (rloc_exec.kick, parts[15]);
      string2bool (rloc_vis.stuck.robot_stuck, parts[16]);
      rloc_exec.stuck.robot_stuck = rloc_vis.stuck.robot_stuck;
      read_anything=true;
    }
  }
  return read_anything;
}
