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


#include "RefboxClient.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include "../../Fundamental/Time.h"

using namespace std;


TribotsTools::RefboxClient ::RefboxClient (const char* lname) throw () : logstream (lname) {
  latest_signal=Tribots::SIGstop;
  okayfailed=0;
  connected=false;
}

TribotsTools::RefboxClient ::~RefboxClient () throw () {
  disconnect ();
  logstream.flush();
}

namespace {
  inline int socket_connect (int socket, struct sockaddr *addr, socklen_t length) {   // Funktion wegen Namenskonfikt noetig
    return connect (socket, addr, length);
  }
}

bool TribotsTools::RefboxClient::connect (const char* ip, int port) throw () {
  Tribots::Time now;
  if (connected)
    disconnect ();     // wenn Verbinung bereits geoeffnet, erst schliessen

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  struct hostent* serveraddr;
  if ( (serveraddr = gethostbyname(ip)) == 0) {
    cout << "RefboxClient:  unknown host " << endl;
    logstream << now << " unknown host; couldn't connect to refbox" << endl;
    return false;
  }
  memcpy(&address.sin_addr, serveraddr->h_addr, serveraddr->h_length);
  address.sin_port = htons(port);
  int result = socket_connect(sockfd, (struct sockaddr *) &address, sizeof(address));

  if (result != 0) {
    cout << "RefboxClient: couldn't connect to server" << endl;
    logstream << now << " couldn't connect to refbox" << endl;
    perror("Could not connect to server");
    return false;
  }

  cout << "RefboxClient: successfully connected to " << inet_ntoa(address.sin_addr) << ", " << ntohs(address.sin_port) << endl;
  logstream << now << " successfully connected to " << inet_ntoa(address.sin_addr) << ", " << ntohs(address.sin_port) << endl;
  
  // get welcome message
  char rcvd[10];
  rcvd[0]='\0';
  if ( read(sockfd, &rcvd, 10) <= 0 ) {
    cout << "RefboxClient: connection lost" << endl;
    logstream << now << " connection lost" << endl;
    return false;
  }

  connected=true;
  okayfailed=0;
  cout << "RefboxClient: got welcome message = '" << rcvd << "'." << endl;
  logstream << now << " got welcome message = '" << rcvd << "'." << endl;
  return true;
}

void TribotsTools::RefboxClient::disconnect() throw () {
  Tribots::Time now;
  if (connected) {
    logstream << now << " disconnect from refbox" << endl;
    close(sockfd);
    connected=false;
  }
}

Tribots::RefboxSignal TribotsTools::RefboxClient::listen() throw () {
  Tribots::Time now;
  if (!connected)
    return Tribots::SIGnop;

  // pruefen, ob gelesen werden kann
  fd_set rfds;
  struct timeval tv;
  FD_ZERO(&rfds);
  FD_SET(sockfd, &rfds);
  tv.tv_sec = 0; tv.tv_usec = 100;
  int sc = select(static_cast<int>(sockfd)+1, &rfds, NULL, NULL, &tv);
  okayfailed = ((sc==0) ? 0 : okayfailed+1);
  bool message_available = FD_ISSET(sockfd, &rfds);

  if (!message_available) {
    return Tribots::SIGnop;     // keine Nachricht da
  }

  char rcvd[10];
  if (read(sockfd, &rcvd, 10) <0) {
    logstream << now << " error when listening for message from refbox" << endl;
    return Tribots::SIGnop;     // Fehler aufgetreten
  }

  logstream << now << " received message from refbox: '" << rcvd[0] << "' = " << static_cast<int>(rcvd[0]) << "." << endl;
  // jetzt umwandeln des gelesenen in Tribots::RefboxSignals
  bool relevant_signal=true;
  Tribots::RefboxSignal res = Tribots::SIGnop;
  switch (rcvd [0]) {
  case 'a': relevant_signal=false; res=Tribots::SIGmagentaGoalScored; break;
  case 'A': relevant_signal=false; res=Tribots::SIGcyanGoalScored; break;
  case 'H': res=Tribots::SIGhalt; break;
  case 'S': res=Tribots::SIGstop; break;
  case 's': 
    if (latest_signal == Tribots::SIGstop || latest_signal == Tribots::SIGhalt)
      res=Tribots::SIGstart;
    else
      res=Tribots::SIGready;
    break;
  case 'x': res=Tribots::SIGstop; break;
  case 'k': res=Tribots::SIGmagentaKickOff; break;
  case 'K': res=Tribots::SIGcyanKickOff; break;
  case 'f': res=Tribots::SIGmagentaFreeKick; break;
  case 'F': res=Tribots::SIGcyanFreeKick; break;
  case 'g': res=Tribots::SIGmagentaGoalKick; break;
  case 'G': res=Tribots::SIGcyanGoalKick; break;
  case 'c': res=Tribots::SIGmagentaCornerKick; break;
  case 'C': res=Tribots::SIGcyanCornerKick; break;
  case 't': res=Tribots::SIGmagentaThrowIn; break;
  case 'T': res=Tribots::SIGcyanThrowIn; break;
  case 'p': res=Tribots::SIGmagentaPenalty; break;
  case 'P': res=Tribots::SIGcyanPenalty; break;
  default: relevant_signal=false; res=Tribots::SIGnop;
  }
  if (relevant_signal)
    latest_signal = res;
  logstream << now << " interpret refbox message as " << Tribots::refbox_signal_names[res] << endl;
  return res;
}

bool TribotsTools::RefboxClient::is_okay () const throw () {
  return (okayfailed<=6) && connected;
}

bool TribotsTools::RefboxClient::is_connected () const throw () {
  return connected;
}
