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


#include "RobotFactory.h"

using namespace Tribots;
using namespace std;

Tribots::RobotFactory* Tribots::RobotFactory::the_only_factory (NULL);

RobotFactory::RobotFactory () throw () {;}

RobotFactory* RobotFactory::get_robot_factory () throw (std::bad_alloc) {
  if (!the_only_factory)
    the_only_factory = new RobotFactory;
  return the_only_factory;
}

RobotFactory::~RobotFactory() throw () {;}

void RobotFactory::sign_up (const std::string descriptor, RobotBuilder* pointer) throw (std::bad_alloc) {
  list_of_plugins [descriptor] = pointer;
}

RobotType* RobotFactory::get_robot (const std::string descriptor, const ConfigReader& reader) throw (TribotsException,bad_alloc,invalid_argument) {
  map<std::string, RobotBuilder*>::iterator mit = list_of_plugins.find (descriptor);
  RobotType* new_wm = NULL;
  if (mit!=list_of_plugins.end())
    new_wm = mit->second->get_robot (descriptor, reader, NULL);
  else
    throw invalid_argument (string("unknown robot type ")+descriptor);

  return new_wm;
}

  
