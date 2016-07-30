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

#ifndef _tribots_tribots_udp_server_h_
#define _tribots_tribots_udp_server_h_

#include "tribotsUdpCommunication.h"
#include "../WorldModel/WorldModel.h"
#include "../Player/Player.h"

namespace Tribots {
  class TribotsUdpServer: public TribotsUdpCommunication {
    
  protected:
    static TribotsUdpServer  * tribotsUdpCom;

    /* DO NOT CALL THIS FUNCTION .. use get_instance_pointer instead!!
     * class is singleton!
     */
    TribotsUdpServer(int port=6012);
  public:
    static TribotsUdpServer *get_instance_pointer(int port=6012);
    static void delete_instance();
    ~TribotsUdpServer();

    bool processRequests(WorldModel &_wm, Player & _pl);
    bool processXMLRequests(WorldModel &_wm, Player & _pl);
    static bool hasInstance();
  };
}

#endif
