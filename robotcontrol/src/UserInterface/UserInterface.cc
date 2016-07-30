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

#include "UserInterface.h"
#include "../Structures/Journal.h"

#include "TerminalUserInterface/TerminalUserInterface.h"
#include "AddComUserInterface/AddComUserInterface.h"

Tribots::UserInterface::UserInterface(const ConfigReader& conf, Player& pl, ImageProcessing& ip) throw (TribotsException, std::bad_alloc)
{
  // Soll mit einem remote Programm kommuniziert werden?
  // das Lokale Interface wird von diesem erzeugt und hat vorrang bei der Steuerung
  bool useComUserInterface = false;
  if (conf.get("add_com_user_interface", useComUserInterface) <=0){
    JERROR("no config line \"add_com_user_interface\" found");
    throw Tribots::InvalidConfigurationException ("add_com_user_interface");
  }
  if (useComUserInterface)
    the_user_interface = new AddComUserInterface(conf, pl, ip);
  else
    {
      string confline;
      if (conf.get("user_interface_type", confline)<=0) {
	JERROR("no config line \"user_interface_type\" found");
	throw Tribots::InvalidConfigurationException ("user_interface_type");
      }
      if (confline=="TerminalUserInterface")
	{
	  the_user_interface = new TerminalUserInterface(conf, pl, ip);
	}
      else 
	{
	  JERROR("No UserInterfaceType of this type found");
	  throw Tribots::InvalidConfigurationException ("user_interface_type");
	}
    }
}

Tribots::UserInterface::~UserInterface() throw ()
{
  delete the_user_interface;
}
