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


#ifndef tribots_add_com_user_interface_h
#define tribots_add_com_user_interface_h

#include "../../Structures/TribotsException.h"
#include "../UserInterface.h"
#include "../UserInterfaceType.h"
#include "../../Communication/tribotsUdpServer.h"

namespace Tribots {

  /** einfache MMI auf Grundlage eines Terminalfensters mit Funktionalitaet zum 
      Starten/Stoppen des Roboters und Beenden des Programms 
      Seiteneffekte: greift auf das Weltmodell zu (lesend und schreibend) */
  class AddComUserInterface : public UserInterfaceType {
  protected:
    UserInterfaceType * the_local_user_interface;
    TribotsUdpServer * the_communication;

    Player &pl;
    ImageProcessing &ip;

    int commFailCount;
    int commMaxFailCount;

  public:
    /** Konstruktor */
    AddComUserInterface (const ConfigReader&, Player&, ImageProcessing&) throw (Tribots::TribotsException, std::bad_alloc);

    /** Destruktor */
    virtual ~AddComUserInterface () throw ();

    /** Tastaturabfrage */
    virtual bool process_messages () throw ();
  };
  
}

#endif
