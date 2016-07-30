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


// mit USE_SIMULATOR 0 kann der Code auch ohne verfuegbaren simsrv uebersetzt werden
#ifndef USE_SIMULATOR
#define USE_SIMULATOR 0
#endif

#ifndef USE_ODESIM
#define USE_ODESIM 0
#endif

#include "SimClient.h"
#include "../WorldModel/WorldModel.h"

#if USE_SIMULATOR
#include "simsrv2/tribotClient/simInterface.h"
#endif



Tribots::SimClient* Tribots::SimClient::the_only_sim_client (NULL);

using namespace Tribots;

SimClient* SimClient::get_sim_client (const char* conf) throw (std::bad_alloc, std::invalid_argument) {
  if (!the_only_sim_client)
    the_only_sim_client = new SimClient (conf);
  return the_only_sim_client;
}

void SimClient::delete_sim_client () throw () {
  if (the_only_sim_client)
    delete the_only_sim_client;
}

SimClient::SimClient (const char* conf) throw (std::bad_alloc, std::invalid_argument) : obstacle_positions (20) {
#if USE_ODESIM
  odesimcomm=new OdeSimComm();
  odesimcomm->use_nonblock();

  //if (vr.get("odeserver_address",odeserver_address,MAX_STRING_LEN)>-1)
 // simclient.use_as_client(odeserver_address, odeserver_port);
 // else 
  odesimcomm->use_as_client("localhost",30001);
//  odesimcomm->use_as_client("10.0.0.106",30001);
  odesimcomm->send(); // Send something so the server knows we are here and the port and inet adress

  obstacle_positions.resize (0);
#endif
#if USE_SIMULATOR
  if (!SimInit(conf))
    throw std::invalid_argument ("Cannot connect to simulator");
  SimSetMotorOn();
  SimSetOmniVelocity(0,0,0);
  obstacle_positions.resize (0);
#endif
}

SimClient::~SimClient () throw () {
#if USE_SIMULATOR
  SimSetRobotPosition (100000,100000,0);   // auf "Parkposition" setzen, da Roboter als Hindernis weiterlebt
  SimUpdate();
  SimExit();
#endif
}

void SimClient::set_drive_vector (DriveVector dv) throw () {
#if USE_ODESIM
 odesimcomm->simstate->steer[1]=-dv.vtrans.x;
 odesimcomm->simstate->steer[0]=dv.vtrans.y;
 odesimcomm->simstate->steer[2]=dv.vrot;
 odesimcomm->simstate->kick=dv.kick;

#endif 
#if USE_SIMULATOR
  SimSetOmniVelocity (static_cast<int>(1000*dv.vtrans.y), -static_cast<int>(1000*dv.vtrans.x), static_cast<int>(1000*dv.vrot));  // hier bin ich mir nicht sicher, ob Faktor 1000 ueberall angebracht ist und Koordinatensysteme uebereinstimmen
  if (dv.kick)
    SimSetKickerKick ();
  else
    SimSetKickerReleased();
#endif
}

#include <iostream>

void SimClient::update () throw () {
#if USE_ODESIM
 odesimcomm->send();
 odesimcomm->receive();
     
    

  //int x1, y1, phi1;
  timeval ts1;
  gettimeofday(&ts1,NULL);
  
  
  // Roboterposition
  
  robot_position.x=odesimcomm->simstate->pos[1]*1000;
  cout <<"ROBOT POS X"<< robot_position.x<<endl;
  
  robot_position.y=-odesimcomm->simstate->pos[0]*1000;
 // cout << robot_position.y<<endl;
  
  robot_heading.set_rad(odesimcomm->simstate->pos[2]+M_PI);
  timestamp.set (ts1);

  
  
  // Ballposition

  
  ball_position = Vec(odesimcomm->simstate->ball[1]*1000,-odesimcomm->simstate->ball[0]*1000);

  
   
  unsigned int n = odesimcomm->simstate->num_obstacles;
  obstacle_positions.resize (n);
  unsigned int i=0;
  unsigned int p=0;
  while (i<n) {
    
    // eigene Position nicht als Hindernis erkennen
      
    if (
        (fabs(odesimcomm->simstate->pos[0]-odesimcomm->simstate->obstacles[0][i])>0.2)
        ||
        (fabs(odesimcomm->simstate->pos[1]-odesimcomm->simstate->obstacles[1][i])>0.2)
      )

      {
      Vec obstacle_position = Vec(odesimcomm->simstate->obstacles[1][i]*1000,-odesimcomm->simstate->obstacles[0][i]*1000);
      obstacle_positions[p++]=obstacle_position;
            
      }
    
      
   i++   ;
    
  }
  obstacle_positions.resize (p);
#endif
#if USE_SIMULATOR
  SimUpdate();
  int x1, y1, phi1;
  timeval ts1;

  // Roboterposition
  SimGetAbsOdometryPosition (x1,y1,phi1,ts1);  
  robot_position.x=static_cast<double>(-y1);
  robot_position.y=static_cast<double>(x1);
  robot_heading.set_deg (phi1);
  timestamp.set (ts1);

  // Ballposition
  SimGetBall (x1,phi1,ts1);
  Vec rel_ball = Vec(0,static_cast<double>(x1)).rotate (Angle::deg_angle (static_cast<double>(phi1)));
  ball_position = robot_position+rel_ball.rotate(robot_heading);

  // Hindernisse (wirklich wahr, nicht die Feldlinien, siehe tribotClient)
  unsigned int arr [200];
  unsigned int n = SimGetFieldlines(200,arr,ts1);
  unsigned int p = 0;
  obstacle_positions.resize (n/4);
  // arr enthaelt die Roboterpositionen, je Roboter: teamcolor, Sichtwinkel, Entfernung, don't-care
  unsigned int i=0;
  while (i<n) {
    i++;  // teamcolor ignorieren
    int win = arr[i++];
    unsigned int dist = arr[i++];
    Angle phi1 =Angle::deg_angle (win);
    double distance = static_cast<double>(dist);
    i++;  // don't care
    if (distance>15) {
      // eigene Position nicht als Hindernis erkennen
      Vec rel_obstacle = Vec(0,distance).rotate (phi1);
      Vec obstacle_position = robot_position+rel_obstacle.rotate(robot_heading);
      obstacle_positions[p++]=obstacle_position;
    }
  }
  obstacle_positions.resize (p);
#endif
}

