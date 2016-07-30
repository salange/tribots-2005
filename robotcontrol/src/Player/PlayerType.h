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


#ifndef tribots_player_type_h
#define tribots_player_type_h

#include "../Structures/DriveVector.h"
#include "../Fundamental/Time.h"


namespace Tribots {

  /** Abstrakte Klasse als Schnittstelle der Planungs- und Entscheidungskomponente nach innen */
  class PlayerType {
  public:
    virtual ~PlayerType () throw () {;}

    /** berechne einen Fahr- und Kickbefehl
	Arg1: Zeitpunkt, zu dem der Fahrtvektor vsl. gesetzt wird
	Ret: gewuenschter Fahr- und Kickbefehl (Geschwindigkeiten) 
        Seiteneffekte: greift auf die Informationen des Weltmodells zu (sollte nur lesender Zugriff sein) */
    virtual DriveVector process_drive_vector (Time) throw ()=0;
  };

}

#endif

