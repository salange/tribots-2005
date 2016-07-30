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


#ifndef Tribots_PlayerRole_h
#define Tribots_PlayerRole_h


namespace Tribots {

  enum PlayerRole
  {
    Defend1,
    Defend2,
    Defend3,
    Attack1,
    Attack2,
    Support
  };

  static const unsigned int num_roles = 8;

  static const char playerRoleNames [8][20] = {
    "Defend1",
    "Defend2",
    "Defend3",
    "Attack1",
    "Attack2",
    "Support",
    "unknown",
    "unknown"
  };    
  
}

#endif
