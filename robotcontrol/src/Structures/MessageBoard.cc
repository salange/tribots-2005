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


#include "MessageBoard.h"
#include "../Fundamental/stringconvert.h"
#include "../WorldModel/WorldModel.h"  // nur fuer debug
#include <iostream>

#define DEBUG_INCOMING 0


Tribots::MessageBoard::MessageBoard () throw (std::bad_alloc) : incoming (50), outgoing (50), null_string ("") {
  clear();
}

const std::vector<std::string> Tribots::MessageBoard::get_incoming () const throw () {
  return incoming;
}

const std::vector<std::string> Tribots::MessageBoard::get_outgoing () throw () {
  outgoing_stream << std::flush;
  std::string line;
  while (!outgoing_stream.eof()) {
    std::getline (outgoing_stream, line);
    if (outgoing_stream.eof() && line.length()==0)
      break;
    outgoing.push_back (line);
  }
  outgoing_stream.clear();    // eof zuruecksetzen
  return outgoing;
}

void Tribots::MessageBoard::clear () throw () {
  incoming.clear();
  outgoing.clear();
}

void Tribots::MessageBoard::publish (const std::string& s) throw () {
  outgoing.push_back (s);
}

void Tribots::MessageBoard::publish (const char* s) throw () {
  outgoing.push_back (std::string(s));
}

std::ostream& Tribots::MessageBoard::publish_stream () throw () {
  return outgoing_stream;
}

void Tribots::MessageBoard::receive (const std::string& s) throw () {
  incoming.push_back (s);
#if DEBUG_INCOMING
  LOUT << "MBoard::receive: " << s << '\n';
#endif
}

void Tribots::MessageBoard::receive (const char* s) throw () {
  incoming.push_back (std::string(s));
#if DEBUG_INCOMING
  LOUT << "MBoard::receive: " << s << '\n';
#endif
}

void Tribots::MessageBoard::receive (const std::vector<std::string>& s) throw () {
  incoming.insert (incoming.end(), s.begin(), s.end());
#if DEBUG_INCOMING
  for (unsigned int i=0; i<s.size(); i++) {
    LOUT << "MBoard::receive: " << s[i] << '\n';
  }
#endif
}

const std::string& Tribots::MessageBoard::scan_for_prefix (const std::string& s) const throw () {
  std::vector<std::string>::const_iterator it = incoming.begin();
  std::vector<std::string>::const_iterator itend = incoming.end();
  while (it<itend) {
    if (Tribots::prefix (s, *it))
      return (*it);
    else
      it++;
  }
  return null_string;
}

const std::string& Tribots::MessageBoard::scan_for_prefix (const char* s) const throw () {
  std::string s1 (s);
  return scan_for_prefix (s1);
}
