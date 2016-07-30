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


#ifndef TribotsTools_RefboxClient_h
#define TribotsTools_RefboxClient_h

// angepasst aus und angelehnt an CTCPIP_Client, der mit der Refereebox mitgeliefert wird.

#include "../../Structures/GameState.h"
#include <fstream>

namespace TribotsTools {

  /** Klasse, die sich mit der Refereebox verbinden kann und 
      Refereebox-Signale in Tribots::RefboxSignal umwandelt */
  class RefboxClient {
  public:
    /** Konstruktor, Name der Logdatei wird uebergeben */
    RefboxClient (const char* ="/dev/null") throw ();
    
    /** Destruktor, loest Verbindung zu Refereebox auf */
    ~RefboxClient () throw ();

    /** Verbinden mit der Refereebox;
	Arg1: IP-Adresse
	Arg2: Port 
	Return: erfolgreich verbunden? */
    bool connect (const char*, int) throw ();
    /** Verbindung aufloesen */
    void disconnect () throw ();

    /** pruefen, ob Informationen eingegangen sind und ggf. umsetzen in RefboxSignals 
	liegen keine Nahrichten vor, wird SIGnop zurueckgeliefert */
    Tribots::RefboxSignal listen () throw ();

    /** abfragen, ob Verbindung okay */
    bool is_okay () const throw ();
    
    /** abfragen, ob Verbindung hergestellt wurde */
    bool is_connected () const throw ();
    
  private:
    Tribots::RefboxSignal latest_signal;    // letztes Signal, das RefereeState veraendert hat (um SIGstart und SIGready unterscheiden zu koennen)
    int sockfd;                             // Socket -Filedescriptor
    int okayfailed;                         // zaehlt, wie oft select !=0 geliefert hat
    bool connected;                         // true, wenn Verbindung hergestellt wurde
    std::ofstream logstream;                // Logdatei
  };

}

#endif
