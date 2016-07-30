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


#include "DriveVectorReadWriter.h"
#include "../Fundamental/stringconvert.h"

using namespace Tribots;
using namespace std;


DriveVectorWriter::DriveVectorWriter (std::ostream& d) throw () : dest(d) {;}

DriveVectorWriter::~DriveVectorWriter () throw () {
  dest << std::flush;
}

void DriveVectorWriter::write (unsigned long int tav, unsigned long int trec, const DriveVector& obj) throw () {
  dest << tav << '\t' 
       << trec << '\t'
       << obj.vtrans.x << '\t'
       << obj.vtrans.y << '\t'
       << obj.vrot << '\t'
       << obj.kick << '\t'
       << obj.mode << '\t'
       << obj.vaux[0] << '\t'
       << obj.vaux[1] << '\t'
       << obj.vaux[2] << '\n';
}


DriveVectorReader::DriveVectorReader (std::istream& s) throw () : src(s), next(0) {
  if (src.good()) src >> next;
}

bool DriveVectorReader::next_timestamp (unsigned long int& t) const throw () {
  if (src.eof())
    return false;
  t=next;
  return true;
}

bool DriveVectorReader::read_until (unsigned long int& tav, unsigned long int& trec, DriveVector& obj, unsigned long int tuntil) throw () {
  tav=next;
  bool read_anything = false;
  while (!src.eof() && next<=tuntil) {
    tav=next;
    string line;
    deque<string> parts;
    getline (src, line);
    split_string (parts, line);
    src >> next;
    if (parts.size()>4) {
      read_anything=true;
      string2ulint (trec, parts[0]);
      string2double (obj.vtrans.x, parts[1]);
      string2double (obj.vtrans.y, parts[2]);
      string2double (obj.vrot, parts[3]);
      string2bool (obj.kick, parts[4]);
    }
    if (parts.size()>7) {
      int mode;
      string2int (mode, parts[5]);
      obj.mode = DriveVectorMode (mode);
      string2double (obj.vaux[0], parts[6]);
      string2double (obj.vaux[1], parts[7]);
      string2double (obj.vaux[2], parts[8]);
    }
  }
  return read_anything;
}
