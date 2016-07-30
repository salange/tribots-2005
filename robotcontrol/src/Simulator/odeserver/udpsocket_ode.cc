/*
 * Copyright (c) 1999 - 2001, Artur Merke <amerke@ira.uka.de> 
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
/* author: Artur Merke */
#include "udpsocket_ode.h"

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

#ifndef ERROR_STREAM
#define ERROR_STREAM cerr
#endif

#ifndef ERROR_OUT
#define ERROR_OUT ERROR_STREAM << "\n\n*** ERROR file=\"" << __FILE__ << "\" line=" << __LINE__
#endif

#ifndef WARNING_STREAM
#define WARNING_STREAM cerr
#endif

#ifndef WARNING_OUT
#define WARNING_OUT WARNING_STREAM << "\n\n*** WARNING file=\"" << __FILE__ << "\" line=" << __LINE__
#endif

#ifdef MEMSET
#define bzero(a, b) memset(a, 0, b)
#endif

#if 0
bool UDPsocketOde::init_socket_fd(int port) {
  struct sockaddr_in cli_addr;
  int fd;
  if( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    return false ;       /* Can't open socket. */
  }

  bzero((char *) &cli_addr, sizeof(cli_addr)) ;
  cli_addr.sin_family           = AF_INET ;
  cli_addr.sin_addr.s_addr      = htonl(INADDR_ANY) ;
  cli_addr.sin_port             = htons(port) ;
   
  if (bind(fd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0){
    return false ;  /* Can't bind local address */
  }
  //set_fd_nonblock(fd);
  socket_fd= fd;
  return true;
}
#endif 


namespace OdeServer{

bool UDPsocketOde::init_socket_fd(int port) {
  struct sockaddr_in cli_addr;
  int fd;
  if( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    ERROR_OUT << "\ncan't open socket";
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
    ERROR_OUT << "can't bind local addres (port= " << port << ")";
    return false ;  /* Can't bind local address */
  }
  //set_fd_nonblock(fd);

  return true;
}

bool UDPsocketOde::init_serv_addr(const char* host, int port) {
  struct hostent        *host_ent ;
  struct in_addr      *addr_ptr ;
 
  if((host_ent = (struct hostent *)gethostbyname(host)) == NULL) {
    /* Check if a numeric address */
    if((int)inet_addr(host) == -1){
      ERROR_OUT << "\ncould'n initialize server address";
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

bool UDPsocketOde::send_msg(const char *buf, int len) {
  if (!buf) return false;
  //n = strlen(buf) ;
    
  int res = sendto(socket_fd, buf, len, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (res != len ) {
    ERROR_OUT << "\nproblems with sending, res= " << res << " len= " << len; 
    return false ;
  }

  return true;
}

bool UDPsocketOde::recv_msg(char *buf, int & len, bool redirect) {
  return recv_msg(buf,len,MAXMESG,redirect);
}

bool UDPsocketOde::recv_msg(char *buf, int & len, int max_len, bool redirect) {
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
  //  cout << "\nNow sender redirected from port " << ntohs(serv_addr.sin_port)
//	 << " to port " << ntohs(recv_addr.sin_port) << " on same server";
    serv_addr.sin_addr.s_addr = recv_addr.sin_addr.s_addr; //new host 
    serv_addr.sin_port = recv_addr.sin_port ; // Aendert die Sendeadresse, muss noch geaendet werden
  }
#else  
  if ( redirect  
       && serv_addr.sin_addr.s_addr == recv_addr.sin_addr.s_addr //same host
       && serv_addr.sin_port != recv_addr.sin_port ) {           //but another port
    cout << "\nNow sender redirected from port " << ntohs(serv_addr.sin_port)
	 << " to port " << ntohs(recv_addr.sin_port) << " on same server";
    serv_addr.sin_port = recv_addr.sin_port ; // Aendert die Sendeadresse, muss noch geaendet werden
    }
#endif  
  return true;
  
}

void UDPsocketOde::set_fd_nonblock(int fd) {
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
    ERROR_OUT << "\nfcntl returns -1";
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

void UDPsocketOde::set_fd_sigio(int fd) {
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
    ERROR_OUT << "\nfcntl returns -1";
    return;
  }

  //cout << "\nfcntl returns != -1";
  int val = fcntl(fd, F_GETFL, 0) ;
  
  val |= FASYNC;
  fcntl(fd, F_SETFL, val) ;
}

void UDPsocketOde::close_socket_fd() {
  close(socket_fd);
}


#if 0
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/***************  T  E  S  T**************************************************/
main() {
  char buf[1000];
  int len;
  UDPsocketOde sock;
  cout << "\ninit_socket_fd = " << sock.init_socket_fd();
  cout << "\ninit_serv_addr = " << sock.init_serv_addr("localhost",6000);
  cout << "\nsend_msg       = " << sock.send_msg("(init BS2k (version5.25))",10);
  cout << "\nrecv_msg       = " << sock.recv_msg(buf,len);
  cout << "\nbuf= " << buf;
  sock.send_msg("(dash 100)",8);
  sock.close_socket_fd();
}
#endif 

}