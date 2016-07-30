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


#include "Joystick.h"
#include <linux/joystick.h>
#include <fcntl.h>

#include <iostream>

using namespace std;

namespace {
  const double MAX_AXIS_VALUE = 32768;   // Groesste Auslenkung der Achsen
}

Tribots::Joystick::Joystick (const char* devname) throw (std::invalid_argument, std::bad_alloc) : axis (6), button (12) {
  file_descriptor = open(devname, O_RDONLY);
  if (file_descriptor<0)
    throw std::invalid_argument ("invalid device name in Tribots::Joystick::Joystick");
  
  fcntl(file_descriptor, F_SETFL, O_NONBLOCK); //don't block
  unsigned char n;
  ioctl(file_descriptor, JSIOCGAXES, &n);  // Anzahl Achsen abfragen
  axis.resize (n);
  ioctl(file_descriptor, JSIOCGBUTTONS, &n);  // Anzahl Knoepfe abfragen
  button.resize (n);
}

Tribots::Joystick::~Joystick () throw () {
  close (file_descriptor);
}

void Tribots::Joystick::update () throw () {
  js_event ev;
  int evsize = sizeof (struct js_event);
  while (true) {
    int size = read (file_descriptor, &ev, evsize);
    if (size!=evsize)
      break;

    if (ev.type==JS_EVENT_BUTTON)
      button[ev.number]=(ev.value==1);
    else if (ev.type==JS_EVENT_AXIS)
      axis[ev.number]=static_cast<double>(ev.value)/MAX_AXIS_VALUE;
  }
}

const std::vector<double>& Tribots::Joystick::get_axis_state () throw () {
  update ();
  return axis;
}

const std::vector<bool>& Tribots::Joystick::get_button_state () throw () {
  update ();
  return button;
}

std::string Tribots::Joystick::get_type () throw (std::bad_alloc) {
  char buffer [500];
  ioctl(file_descriptor, JSIOCGNAME(500), buffer);
  return std::string (buffer);
}

int Tribots::Joystick::get_version () throw () {
  int n;
  ioctl(file_descriptor, JSIOCGVERSION, &n);
  return n;
}

