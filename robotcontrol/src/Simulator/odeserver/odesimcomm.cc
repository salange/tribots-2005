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

//test unison
#include "odesimcomm.h"
#include <string.h>
//#include "macro_msg.h"
#include <sys/time.h>
#include <unistd.h>

namespace OdeServer{


int ServerData::add_client(sockaddr_in client_addr)
  {
    for (int i=0;i<MAX_CLIENTS;i++)
    {
      if (!client_active[i])
      {
        serv_addr[i]= client_addr;
	client_active[i]=true;
	client_not_responding[i]=0;
        return i;// return number of added client
      }


    }
    return -1;// list full no more clients available
  }


int ServerData::is_active_client(sockaddr_in client_addr)
  {
    for (int i=0;i<MAX_CLIENTS;i++)
    {
      if  (serv_addr[i].sin_addr.s_addr == client_addr.sin_addr.s_addr && 
      	   serv_addr[i].sin_port == client_addr.sin_port && 
	   client_active[i]==true){
        client_not_responding[i]=0;
	return i; // return number of client
        }
    }

    //not found as active client
    return -1;
  }
  
  
  void ServerData::check_for_inactivity(){
    
    for (int i=0;i<MAX_CLIENTS;i++)
    {
       client_not_responding[i]=client_not_responding[i]+1;
       if  ( client_active[i]==true && client_not_responding[i] > 30 ){
        client_active[i]=false;
	cout <<"Client "<< i<<" not Responding"<<endl;
	}
    }
  
  
  
  }
  
  
/****************************************************************************/
/****************************************************************************/
OdeSimComm::~OdeSimComm()
{}

bool OdeSimComm::use_as_server(int port)
{
  is_server=true;  // if you need to know inside some functions whether this is a server
  return sock.init_socket_fd(port);
}

bool OdeSimComm::use_as_client(char const* addr, int port)
{
  sock.init_socket_fd ();
  return sock.init_serv_addr(addr,port);
}

bool OdeSimComm::use_nonblock()
{
  sock.set_fd_nonblock();
  return true;
}


bool OdeSimComm::send()
{
  sock.send_msg ((char *) simstate, sizeof(*simstate));
  return true;
}



bool OdeSimComm::receive()
{
  int length;
  if (sock.recv_msg((char *) simstate, length, sizeof (*simstate)))
  {
   
    if (this->is_server)
    {

   /*   for (int i=0;i<MAX_CLIENTS;i++)
     {
       cout<<"ACTIVE CLIENT "<< i << " : " <<serverdata.client_active[i]<<endl;
      }

	*/
	//cout <<"INET "<<sock.serv_addr.sin_addr.s_addr<<"  PORT "<<sock.serv_addr.sin_port<<endl;
    
	last_client=this->serverdata.is_active_client(sock.serv_addr);
       //cout <<"communication with client no :"<<last_client<<endl;
        
      if (last_client==-1)
      {
        int res=this->serverdata.add_client(sock.serv_addr);
        cout <<"NEW CLIENT CONNECTED :"<<res<<endl;
        last_client=res;
      }
    }
    
    return true;

  }
  else
  {
    return false;
  }

}


}
