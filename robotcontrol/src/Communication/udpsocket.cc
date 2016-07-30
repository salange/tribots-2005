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

#include "udpsocket.h"
#include "../Structures/Journal.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

using namespace std;

#ifdef MEMSET
#define bzero(a, b) memset(a, 0, b)
#endif

bool UDPsocket::init_socket_fd(int port) {
  struct sockaddr_in cli_addr;

  bzero((char *) &serv_addr, sizeof(serv_addr)) ;

  int fd;
  if( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    JERROR("can't open socket");
    return false ;       /* Can't open socket. */
  }

  socket_fd= fd;

  if (port <= 0)
    return true;

  bzero((char *) &cli_addr, sizeof(cli_addr)) ;
  cli_addr.sin_family           = AF_INET ;
  cli_addr.sin_addr.s_addr      = htonl(INADDR_ANY) ;
  cli_addr.sin_port             = htons(port) ;
   
  if (bind(fd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0){
    char ebuf[100];
    sprintf(ebuf, "can't bind local address (port= %d)", port);
    JERROR(ebuf);
    return false ;  /* Can't bind local address */
  }

  return true;
}

bool UDPsocket::init_serv_addr(const char* host, int port) {
  struct hostent      *host_ent ;
  struct in_addr      *addr_ptr ;
 
  if((host_ent = (struct hostent *)gethostbyname(host)) == NULL) {
    /* Check if a numeric address */
    if((int)inet_addr(host) == -1){
      JERROR("could'n initialize server address");
      return false;
    }
  }
  else {
    addr_ptr = (struct in_addr *) *host_ent->h_addr_list ;
    host = inet_ntoa(*addr_ptr) ;
  }

  bzero((char *) &serv_addr, sizeof(serv_addr)) ;
  serv_addr.sin_family		= AF_INET ;
  serv_addr.sin_addr.s_addr	= inet_addr(host) ;
  serv_addr.sin_port		= htons(port) ;
  return true;
}

bool UDPsocket::send_msg(const char *buf, int len) {
  if (!buf) return false;
  //n = strlen(buf) ;
    
  int res = sendto(socket_fd, buf, len, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (res != len ) {
    perror("socket send_msg");
    std::cerr << "problems with sending, res= " << res << " len= " << len << "To : " << serv_addr.sin_addr.s_addr << " Port: " << ntohs(serv_addr.sin_port) <<"\n";

    for (int i=0; i<len; i++)
      {
	std::cerr << "[" << buf[i] << "]\n";
      }
    
    JERROR("problems with sending");
    return false ;
  }

  return true;
}

bool UDPsocket::recv_msg(char *buf, int & len, bool redirect) {
  return recv_msg(buf,len,MAXMESG,redirect);
}

bool UDPsocket::recv_msg(char *buf, int & len, int max_len, bool redirect) {
  unsigned int addr_len ;
  struct sockaddr_in    recv_addr ;
      
  addr_len = (unsigned int) sizeof(recv_addr) ;
  len = recvfrom(socket_fd, buf, max_len, 0,(struct sockaddr *)&recv_addr, &addr_len);
  if(len < 0) {
    if( len == -1 && errno == EWOULDBLOCK){
      //cout << "-" << flush;
    }
    len = 0;
    buf[0]= '\0';
    return false ;
  }
  //cout << buf << flush;
  buf[len] = '\0' ;


#if 1
  if ( redirect && 
       (serv_addr.sin_addr.s_addr != recv_addr.sin_addr.s_addr //new host
         || serv_addr.sin_port != recv_addr.sin_port ))
  {           //and another port
    std::cerr << "Now sender redirected from port " << ntohs(serv_addr.sin_port)
    	     << " to port " << ntohs(recv_addr.sin_port) << " on same server";
    JMESSAGE("Now sender redirected from port");
    serv_addr.sin_addr.s_addr = recv_addr.sin_addr.s_addr; //new host 
    serv_addr.sin_port = recv_addr.sin_port ; // Aendert die Sendeadresse, muss noch geaendet werden
  }
#else  
  if ( redirect  
       && serv_addr.sin_addr.s_addr == recv_addr.sin_addr.s_addr //same host
       && serv_addr.sin_port != recv_addr.sin_port ) {           //but another port
    JMESSAGE("\nNow sender redirected from port " << ntohs(serv_addr.sin_port)
	     << " to port " << ntohs(recv_addr.sin_port) << " on same server");
    serv_addr.sin_port = recv_addr.sin_port ; // Aendert die Sendeadresse, muss noch geaendet werden
    }
#endif  
  return true;
  
}

void UDPsocket::set_fd_nonblock(int fd) {
/* doc:
O_NONBLOCK 
      The bit that enables nonblocking mode for the file. If this bit is set, read requests on the file can return
      immediately with a failure status if there is no input immediately available, instead of blocking. Likewise, write
      requests can also return immediately with a failure status if the output can't be written immediately. 

O_NDELAY 
      This is a synonym for O_NONBLOCK, provided for compatibility with BSD. 

FASYNC
if you set the FASYNC status flag on a file descriptor (see section File Status Flags), a SIGIO signal is sent whenever input
or output becomes possible on that file descriptor. The process or process group to receive the signal can be selected
by using the F_SETOWN command to the fcntl function. If the file descriptor is a socket, this also selects the recipient of
SIGURG signals that are delivered when out-of-band data arrives on that socket; see section Out-of-Band Data. 
*/

  if ( fcntl( fd , F_SETOWN, getpid()) == -1) {
    JERROR("\nfcntl returns -1");
    return;
  }

  //cout << "\nfcntl returns != -1";
  int val = fcntl(fd, F_GETFL, 0) ;

#if 1 
  val |= O_NDELAY ;
#else
  val |= O_NONBLOCK ;
#endif
  //val |= FASYNC;
  fcntl(fd, F_SETFL, val) ;
}

void UDPsocket::set_fd_sigio(int fd) {
/* doc:
O_NONBLOCK 
      The bit that enables nonblocking mode for the file. If this bit is set, read requests on the file can return
      immediately with a failure status if there is no input immediately available, instead of blocking. Likewise, write
      requests can also return immediately with a failure status if the output can't be written immediately. 

O_NDELAY 
      This is a synonym for O_NONBLOCK, provided for compatibility with BSD. 

FASYNC
if you set the FASYNC status flag on a file descriptor (see section File Status Flags), a SIGIO signal is sent whenever input
or output becomes possible on that file descriptor. The process or process group to receive the signal can be selected
by using the F_SETOWN command to the fcntl function. If the file descriptor is a socket, this also selects the recipient of
SIGURG signals that are delivered when out-of-band data arrives on that socket; see section Out-of-Band Data. 
*/

  if ( fcntl( fd , F_SETOWN, getpid()) == -1) {
    JERROR("\nfcntl returns -1");
    return;
  }

  //cout << "\nfcntl returns != -1";
  int val = fcntl(fd, F_GETFL, 0) ;
  
  val |= FASYNC;
  fcntl(fd, F_SETFL, val) ;
}

void UDPsocket::close_socket_fd() {
  close(socket_fd);
}

