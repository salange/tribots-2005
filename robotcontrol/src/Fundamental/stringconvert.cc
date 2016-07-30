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


#include "stringconvert.h"
#include <cctype>
#include <cstdio>

using namespace std;

void Tribots::split_string (deque<string>& dest, const string& src) throw (std::bad_alloc) {
  dest.clear();
  if (src.length()==0)
    return;
  int i=-1;
  bool was_white = isspace (src[0]);
  for (unsigned int j=0; j<src.length(); j++) {
    if (isspace (src[j])) {
      if (was_white) {
	i=j;
      } else {
	was_white=true;
	dest.push_back (src.substr (i+1, j-i-1));
	i=j;
      }
    } else {
      was_white=false;
    }
  }
  if (!was_white) {
    dest.push_back (src.substr (i+1, src.length()));
  }
}

bool Tribots::prefix (const std::string& p, const std::string w) throw () {
  const unsigned int n=p.length();
  if (n>w.length())
    return false;
  for (unsigned int i=0; i<n; i++)
    if (p[i]!=w[i])
      return false;
  return true;
}

bool Tribots::string2double (double& dest, const string& src) throw () {
  char* end_char;
  dest = strtod (src.c_str(), &end_char);
  return ((*end_char)=='\0');
}

bool Tribots::string2float (float& dest, const string& src) throw () {
  double d;
  bool success = Tribots::string2double (d, src);
  dest=d;
  return success;
}

bool Tribots::string2lint (long int& d, const string& s) throw () {
  char* end_char;
  d = strtol (s.c_str(), &end_char, 0);
  if ((*end_char)!='\0')
    return false;
  return true;
}

bool Tribots::string2ulint (unsigned long int& d, const string& s) throw () {
  char* end_char;
  d = strtoul (s.c_str(), &end_char, 0);
  if ((*end_char)!='\0')
    return false;
  if ((s[0]=='-') && (d!=0))
      return false;
  return true;
}

bool Tribots::string2int (int& d, const string& s) throw () {
  long int l;
  bool success = Tribots::string2lint (l, s);
  d=l;
  return success;
}

bool Tribots::string2uint (unsigned int& d, const string& s) throw () {
  unsigned long int l;
  bool success = Tribots::string2ulint (l, s);
  d=l;
  return success;
}

bool Tribots::string2bool (bool& d, const string& s) throw () {
  if (s==string("0")) {
    d=false;
    return true;
  }
  if (s==string("1")) {
    d=true;
    return true;
  }
  if (s==string("true")) {
    d=true;
    return true;
  }
  if (s==string("false")) {
    d=false;
    return true;
  }
  return false;
}

