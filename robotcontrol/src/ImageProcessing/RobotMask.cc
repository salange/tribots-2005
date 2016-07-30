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

#include "RobotMask.h"

#include <fstream>
#include <iostream>

using namespace std;

/* skips spaces and recursivly multi-line comments on a given stream. */
static void skip(istream& in);
/* convenience methods, terminates program */
static void error(const string& msg);

const RobotMask*
RobotMask::instance = 0;

RobotMask::RobotMask(std::string filename)
  : mask(0), width(0), height(0)
{
  ifstream in(filename.c_str());
  if (!in) {
    cerr << "Could not open mask file: " << filename << "\n\r";
    exit(1);
  }

  char c;
  int type, maxCol;

  skip(in);
  if (!in.get(c)) error("Unexpected end of file.");
  if (c != 'P' && c != 'p') error("Not a PPM file.");

  skip(in);
  in >> type;
  skip(in);
  in >> width;
  skip(in);
  in >> height;
  skip(in);
  in >> maxCol;    // is ignored \todo
  
  if (type != 6) { // is binary format?
    error("Not a binary ppm file.");
  }
  in.ignore(1);    // \todo this assumes exactly one character before data...
  unsigned char* buf = new unsigned char[width*height*3];
  in.read(reinterpret_cast<char*>(buf), width*height*3);
  mask = new bool[width*height];

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      mask[x+y*width] = buf[x*3+y*width*3] > 127;  // check red channel marked
    }
  }
  delete [] buf;
}

RobotMask::~RobotMask()
{
  if (mask) delete [] mask;
}

void
RobotMask::load(std::string filename)
{
  if (!instance) {
    instance  = new RobotMask(filename);
  }
  else {
    error("Mask has already been loaded.");
  }
}

static void error(const string& msg) {
  cerr << msg << "\n\r";
  exit(1);
}

/* skips spaces and recursivly multi-line comments on a given stream.
 */
static void skip(istream& in)
{
  char c;
  do {
    if (!in.get(c)) return;
  } while(isspace(c) || c=='\n' || c=='\r');
  if (c=='#') {                  // start of a comment
    do {                         // look for the end of the line
      if (!in.get(c)) return;
    } while(c!='\n');
    skip(in);                    // skip again at start of next line.
  }
  else                           // c was neither comment(#) nor space
    in.putback(c);               // so better put it back,
}

