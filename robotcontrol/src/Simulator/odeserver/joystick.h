/*
 * Copyright (c) 2002, Artur Merke <amerke@ira.uka.de>
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

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_
#define MAX_BUTTONS 20
#define MAX_AXES 10


using namespace std;

struct Joystick {
  static const int MAX_LEN= 512;
  static const int MAX_AXIS_VALUE= 32767;
  static const int MIN_AXIS_VALUE= -MAX_AXIS_VALUE;
  int joystick_fd;
  int num_axes;
  int num_buttons;
  int version;
  char name[MAX_LEN];
  int button[MAX_BUTTONS];
  int axis[MAX_AXES];
  bool button_chg[MAX_BUTTONS];
  bool axis_chg[MAX_AXES];

  Joystick();
  bool init(const char * joydev);
  int read_all_events();


};

#endif
