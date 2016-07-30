/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

/*
test Simulation fr robocup Tribots

Stefan Welker

*/


#include <ode/ode.h>
#include "structures.h"
#include <math.h>
#include <time.h>
#include "joystick.h"
#include "v2d.h"
#include "frame2d.h"
#include <iostream>
#include <iomanip>
#include "odesimcomm.h"
#include <stdio.h>

#include <drawstuff/drawstuff.h>

#ifdef MSVC
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

// select correct drawing functions

#ifdef dDOUBLE
#define dsDrawBox dsDrawBoxD
#define dsDrawSphere dsDrawSphereD
#define dsDrawCylinder dsDrawCylinderD
#define dsDrawCappedCylinder dsDrawCappedCylinderD
#endif


// some constants

#define BALL_RADIUS 0.14	// ball radius
#define LENGTH 0.36	// box length & width
#define HEIGHT 0.4	// box height
#define MASS 10		// mass of box[i][j] = (i+1) * MASS
#define FORCE 0.1	// force applied to box[i][j] = (j+1) * FORCE
#define MU 0.1		// the global mu to use
#define GRAVITY 0.9	// the global gravity to use
#define N 16		// number of robots
#define MAX_CONTACTS 3		// maximum number of contact points per body
#define FIELD_LENGTH 12		// maximum number of contact points per body
#define FIELD_WIDTH 8		// maximum number of contact points per body

using namespace OdeServer;

// dynamics and collision objects

struct BodyState{
dReal pos[3];
dReal vel[3];
dQuaternion q;

};

struct OdeSimState{
BodyState robbies[N];
BodyState ball;

};

static dWorldID world;
static dSpaceID space;
static dBodyID body[N];
static dBodyID sphere;
static dBodyID bodygoal;

static dJointGroupID contactgroup;
static dGeomID ground;
static dGeomID box[N];
static dGeomID immovablebox[6];
static dGeomID ball;
static dGeomID geomgoal;

OdeSimState savesimstate[3];
static double savedBallPos[3];
static dReal savedBallVel[3];
static dReal savedBallAngVel[3];
int goaldelay;

int simloopCount=0;
int viewmode=0;
float* camsmooth;
float* camlooksmooth;

bool in_field;
Joystick joy;
int activerobot;
bool move_ball=false;

double shootspeed=1;
static Robot robbies[N];

static OdeSimComm serverobject;



/*robots[0].steer[0] =1;
robots[0].steer[1] =1;
robots[0].steer[2] =1;
*/
//robbies[1].steer[0]=0;


// this is called by dSpaceCollide when two objects in space are
// potentially colliding.
/*
static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
  int i;

  // only collide things with the ground
  int g1 = (o1 == ground);
  int g2 = (o2 == ground);
  if (!(g1 ^ g2)) return;

  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);

  dContact contact[3];		// up to 3 contacts per box
  for (i=0; i<3; i++) {
    contact[i].surface.mode = dContactSoftCFM | dContactApprox1;
    contact[i].surface.mu = MU;
    contact[i].surface.soft_cfm = 0.01;
  }
  if (int numc = dCollide (o1,o2,3,&contact[0].geom,sizeof(dContact))) {
    for (i=0; i<numc; i++) {
      dJointID c = dJointCreateContact (world,contactgroup,contact+i);
      dJointAttach (c,b1,b2);
    }
  }
}
*/



static void followRules()    // FEHLER DRIN ???
{
 dReal *dpos; 
 dpos =(dReal*)dBodyGetPosition(sphere);
double up,down,left,right;
up=FIELD_WIDTH/2;
down=-FIELD_WIDTH/2;
left=-FIELD_LENGTH/2;
right=FIELD_LENGTH/2;

if ((dpos[0]>left)&&(dpos[0]<right) &&
	dpos[1]>up+0.10)
{

  dBodySetPosition (sphere,dpos[0]+((rand()%10)-5)*1.0f/10,up,1.0);
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere, 0,0,0);
         dBodySetAngularVel  (sphere, 0,0,0	);
cout<<"OUT !! THROW IN!!! 0"<<endl;
	  

}


if ((dpos[0]>left)&&(dpos[0]<right) &&
	dpos[1]<down-0.10)
{

  dBodySetPosition (sphere,dpos[0]+((rand()%10)-5)*1.0f/10,down,1.0);
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere, 0,0,0);
  
         dBodySetAngularVel  (sphere, 0,0,0	);
cout<<"OUT !! THROW IN!!! 1"<<endl;
	  

}





if ((dpos[0]<left) && (dpos[0]>left-50) &&  fabs(dpos[1])<1)
{
cout<<"GOAL_BLUE!!!! 2"<<endl;
  goaldelay++;
  	 

}

if ((dpos[0]>right) && (dpos[0]<right+50) &&  fabs(dpos[1])<1)
{
cout<<"GOAL_YELLOW 3"<<endl;
goaldelay++;

}
if ((dpos[0]>right)&&  dpos[1]>1)

{

  dBodySetPosition (sphere,right-0.5,up,0.4);
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere, 0,0,0);
         dBodySetAngularVel  (sphere, 0,0,0	);
cout<<"OUT !! CORNER!!! 4"<<endl;
	  

}
if ((dpos[0]>right)&&  dpos[1]<-1)

{

  dBodySetPosition (sphere,right-0.5,down,0.4);
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere, 0,0,0);
         dBodySetAngularVel  (sphere, 0,0,0	);
cout<<"OUT !! CORNER!!! 5"<<endl;
	  

}
if ((dpos[0]<left)&&  dpos[1]>1)

{

  dBodySetPosition (sphere,left+0.5,up,0.4);
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere, 0,0,0);
         dBodySetAngularVel  (sphere, 0,0,0	);
cout<<"OUT !! CORNER!!! 6"<<endl;
	  

}
if ((dpos[0]<left)&&  dpos[1]<-1)

{

  dBodySetPosition (sphere,left+0.5,down,0.4);
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere, 0,0,0);
         dBodySetAngularVel  (sphere, 0,0,0	);
cout<<"OUT !! CORNER!!! 7"<<endl;
	  

}



  }


  static void setFormation()    // FEHLER DRIN ???
{
  for (int i=0; i<N; i++) {
      if (i==0)dBodySetPosition (body[i],1,0,0.5);
      if (i==1)dBodySetPosition (body[i],2,2,0.5);
      if (i==2)dBodySetPosition (body[i],2,-2,0.5);
      if (i==3)dBodySetPosition (body[i],FIELD_LENGTH/2-0.5,0,0.5);
      
      
      if (i==8)dBodySetPosition (body[i],1,FIELD_WIDTH/2+1,0.5);
      if (i==9)dBodySetPosition (body[i],2,FIELD_WIDTH/2+1,0.5);
      if (i==10)dBodySetPosition (body[i],3,FIELD_WIDTH/2+1,0.5);
      if (i==11)dBodySetPosition (body[i],4,FIELD_WIDTH/2+1,0.5);
      
      if (i==4)dBodySetPosition (body[i],-1,0,0.5);
      if (i==5)dBodySetPosition (body[i],-2,2,0.5);
      if (i==6)dBodySetPosition (body[i],-2,-2,0.5);
      if (i==7)dBodySetPosition (body[i],-FIELD_LENGTH/2+0.5,0,0.5);
      
      
      if (i==12)dBodySetPosition (body[i],-1,FIELD_WIDTH/2+1,0.5);
      if (i==13)dBodySetPosition (body[i],-2,FIELD_WIDTH/2+1,0.5);
      if (i==14)dBodySetPosition (body[i],-3,FIELD_WIDTH/2+1,0.5);
      if (i==15)dBodySetPosition (body[i],-4,FIELD_WIDTH/2+1,0.5);
      
      
  }


}

static void saveState(int state){

//Bodies
const dReal* dreal;


for (int i=0; i<N; i++) {
      
      dreal=dBodyGetPosition(body[i]);
      savesimstate[state].robbies[i].pos[0]=dreal[0];
      savesimstate[state].robbies[i].pos[1]=dreal[1];
      savesimstate[state].robbies[i].pos[2]=dreal[2];
      dreal=dBodyGetLinearVel(body[i]);
      savesimstate[state].robbies[i].vel[0]=dreal[0];
      savesimstate[state].robbies[i].vel[1]=dreal[1];
      savesimstate[state].robbies[i].vel[2]=dreal[2];
      dreal=dBodyGetQuaternion(body[i]);
      savesimstate[state].robbies[i].q[0]=dreal[0];
      savesimstate[state].robbies[i].q[1]=dreal[1];
      savesimstate[state].robbies[i].q[2]=dreal[2];
      savesimstate[state].robbies[i].q[3]=dreal[3];
  }

//Ball

      dreal=dBodyGetPosition(sphere);
      savesimstate[state].ball.pos[0]=dreal[0];
      savesimstate[state].ball.pos[1]=dreal[1];
      savesimstate[state].ball.pos[2]=dreal[2];
      dreal=dBodyGetLinearVel(sphere);
      savesimstate[state].ball.vel[0]=dreal[0];
      savesimstate[state].ball.vel[1]=dreal[1];
      savesimstate[state].ball.vel[2]=dreal[2];
      dreal=dBodyGetQuaternion(sphere);
      savesimstate[state].ball.q[0]=dreal[0];
      savesimstate[state].ball.q[1]=dreal[1];
      savesimstate[state].ball.q[2]=dreal[2];
      savesimstate[state].ball.q[3]=dreal[3];


}

static void recallSavedState(int state){


for (int i=0; i<N; i++) {
      dBodySetPosition(body[i],savesimstate[state].robbies[i].pos[0],savesimstate[state].robbies[i].pos[1],savesimstate[state].robbies[i].pos[2]);
      dBodySetLinearVel(body[i],savesimstate[state].robbies[i].vel[0],savesimstate[state].robbies[i].vel[1],savesimstate[state].robbies[i].vel[2]);
      dBodySetQuaternion(body[i],savesimstate[state].robbies[i].q);      
  }

      dBodySetPosition(sphere,savesimstate[state].ball.pos[0],savesimstate[state].ball.pos[1],savesimstate[state].ball.pos[2]);
      dBodySetLinearVel(sphere,savesimstate[state].ball.vel[0],savesimstate[state].ball.vel[1],savesimstate[state].ball.vel[2]);
      dBodySetQuaternion(sphere,savesimstate[state].ball.q);      




}
  
static void saveStatesToDisk(){
FILE * outfile;
outfile= fopen("savestate.bin","w");
fwrite(&savesimstate,sizeof(savesimstate),3,outfile);
fclose(outfile);
cout<<"saving"<<endl;



}
  
  
static void loadStatesFromDisk(){
FILE * infile;
infile= fopen("savestate.bin","r");
fread(&savesimstate,sizeof(savesimstate),3,infile);
fclose(infile);
cout<<"Falls kein Segmentation fault = >Loading ready"<<endl;



}
  
  

static void drawField()
{
  dsSetColor (1,1,1);
  float field_length=FIELD_LENGTH;
  float field_width=FIELD_WIDTH;
  float flh=field_length/2;
  float fwh=field_width/2;
  float lh=0.01;
  double a[3];
  double b[3];
  a[0]=-flh; a[1]=fwh;a[2]=lh; 
  b[0]=+flh; b[1]=fwh;b[2]=lh;
  dsDrawLineD(a,b);
  b[0]=-flh; b[1]=-fwh;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=flh; a[1]=-fwh;a[2]=lh; 
  dsDrawLineD(a,b);
  b[0]=+flh; b[1]=fwh;b[2]=lh;
  dsDrawLineD(a,b);
  
  a[0]=0; a[1]=-fwh;a[2]=lh; 
  b[0]=0; b[1]=fwh;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=0; a[1]=-1;a[2]=lh; 
  b[0]=-0.7; b[1]=-0.7;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=-1; a[1]=0;a[2]=lh; 
  dsDrawLineD(a,b);
  b[0]=-0.7; b[1]=+0.7;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=0; a[1]=+1;a[2]=lh; 
  dsDrawLineD(a,b);
  a[0]=0; a[1]=-1;a[2]=lh; 
  b[0]=0.7; b[1]=-0.7;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=1; a[1]=0;a[2]=lh; 
  dsDrawLineD(a,b);
  b[0]=0.7; b[1]=+0.7;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=0; a[1]=+1;a[2]=lh; 
  dsDrawLineD(a,b);
  
  a[0]=-flh; a[1]=-1.5;a[2]=lh; 
  b[0]=-flh+.5; b[1]=-1.5;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=-flh+.5; a[1]=+1.5;a[2]=lh; 
  dsDrawLineD(a,b);
  b[0]=-flh; b[1]=+1.5;b[2]=lh; 
  dsDrawLineD(a,b);
  
  a[0]=-flh; a[1]=-2.5;a[2]=lh; 
  b[0]=-flh+1.5; b[1]=-2.5;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=-flh+1.5; a[1]=+2.5;a[2]=lh; 
  dsDrawLineD(a,b);
  b[0]=-flh; b[1]=+2.5;b[2]=lh; 
  dsDrawLineD(a,b);
  
  
  a[0]=+flh; a[1]=-1.5;a[2]=lh; 
  b[0]=+flh-.5; b[1]=-1.5;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=+flh-.5; a[1]=+1.5;a[2]=lh; 
  dsDrawLineD(a,b);
  b[0]=+flh; b[1]=+1.5;b[2]=lh; 
  dsDrawLineD(a,b);
  
  a[0]=+flh; a[1]=-2.5;a[2]=lh; 
  b[0]=+flh-1.5; b[1]=-2.5;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=+flh-1.5; a[1]=+2.5;a[2]=lh; 
  dsDrawLineD(a,b);
  b[0]=+flh; b[1]=+2.5;b[2]=lh; 
  dsDrawLineD(a,b);
  
  
  a[0]=+flh; a[1]=-fwh+.5;a[2]=lh; 
  b[0]=+flh-.3; b[1]=-fwh+.3;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=+flh-.5; a[1]=-fwh;a[2]=lh; 
  dsDrawLineD(a,b);
   
  a[0]=+flh; a[1]=+fwh-.5;a[2]=lh; 
  b[0]=+flh-.3; b[1]=fwh-.3;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=+flh-.5; a[1]=fwh;a[2]=lh; 
  dsDrawLineD(a,b);

  a[0]=-flh; a[1]=-fwh+.5;a[2]=lh; 
  b[0]=-flh+.3; b[1]=-fwh+.3;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=-flh+.5; a[1]=-fwh;a[2]=lh; 
  dsDrawLineD(a,b);
   
  a[0]=-flh; a[1]=+fwh-.5;a[2]=lh; 
  b[0]=-flh+.3; b[1]=fwh-.3;b[2]=lh;
  dsDrawLineD(a,b);
  a[0]=-flh+.5; a[1]=fwh;a[2]=lh; 
  dsDrawLineD(a,b);

  
    
  
  
}






static void createField(float *lines,int *lines_num)
{
  
  /*float field_length=1;
  float field_width=2;
  float line_height=1.1;
  
  lines[0+3*(*lines_num)]=-field_length/2;lines[1+3*(*lines_num)]= field_width/2;lines[2+3*(*lines_num)]=line_height; *lines_num++;
  lines[0+3*(*lines_num)]= field_length/2;lines[1+3*(*lines_num)]= field_width/2;lines[2+3*(*lines_num)]=line_height; *lines_num++;
  lines[0+3*(*lines_num)]= field_length/2;lines[1+3*(*lines_num)]= field_width/2;lines[2+3*(*lines_num)]=line_height; *lines_num++;
  lines[0+3*(*lines_num)]= field_length/2;lines[1+3*(*lines_num)]= field_width/2;lines[2+3*(*lines_num)]=line_height; *lines_num++;
  lines[0+3*(*lines_num)]= field_length/2;lines[1+3*(*lines_num)]= -field_width/2;lines[2+3*(*lines_num)]=line_height; *lines_num++;
  lines[0+3*(*lines_num)]= -field_length/2;lines[1+3*(*lines_num)]= -field_width/2;lines[2+3*(*lines_num)]=line_height; *lines_num++;
  lines[0+3*(*lines_num)]= -field_length/2;lines[1+3*(*lines_num)]= -field_width/2;lines[2+3*(*lines_num)]=line_height; *lines_num++;
  lines[0+3*(*lines_num)]= -field_length/2;lines[1+3*(*lines_num)]= field_width/2;lines[2+3*(*lines_num)]=line_height; *lines_num++;
  */
  }


static void nearCallback (void *data, dGeomID o1, dGeomID o2) // Kollisionen zwischen allen objekten und dem boden
{
  int i;
  // if (o1->body && o2->body) return;

  // exit without doing anything if the two bodies are connected by a joint
  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);
  if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;

  dContact contact[MAX_CONTACTS];   // up to MAX_CONTACTS contacts per box-box
  for (i=0; i<MAX_CONTACTS; i++) {
    contact[i].surface.mode = dContactBounce | dContactSoftCFM;
    contact[i].surface.mu = MU;
    //contact[i].surface.mu2 = 0;
    contact[i].surface.bounce = 0.7;
    contact[i].surface.bounce_vel = 0.2;
    contact[i].surface.soft_cfm = 0.03;
  }
  if (int numc = dCollide (o1,o2,MAX_CONTACTS,&contact[0].geom,
			   sizeof(dContact))) {
    dMatrix3 RI;
    dRSetIdentity (RI);
    const dReal ss[3] = {0.02,0.02,0.02};
    for (i=0; i<numc; i++) {
      dJointID c = dJointCreateContact (world,contactgroup,contact+i);
      dJointAttach (c,b1,b2);
    }
  }
}


// simulation - Kamera setzen

static void start()  
{
  printf ("benutze den joystick um roboter 0 zu steuern");
  printf ("   oder .\n");
  printf ("   a links\n");
  printf ("   d rechts\n");
  printf ("   w vorwärts.\n");
  printf ("   s rückwärts.\n");
  printf ("   k links drehen.\n");
  printf ("   l rechts drehen.\n");
  printf ("   r ball reset.\n");
  printf ("   t viewport toggeln 1.person/ von oben.\n");
  printf ("   v ball aus einiger entfernung anrollen lassen.\n");
  printf ("   f Beide Teams Formation einnehmen.\n");
  printf ("   n/N Aktiven Roboter vor/zurückcycelnn");
  printf ("   c Capture Ball Position/Velocity");
  printf ("   p Play Ball Position/Velocity");
  printf ("   P Place Ball at ActiveRobot and shoot");
  printf ("   b Move Ball toggle");
}


static void setCamera()  
{
  start();
  
}



static void communicate()  
{
    
    // receive and try to map the client to a robbi and send back appropriate Worldmodel data
    int howmanyreceived;
    howmanyreceived=0;
    serverobject.serverdata.check_for_inactivity();
    
    
    while (serverobject.receive()){   // Receive all client data

    
    // this is robbi steering information for the server
     robbies[serverobject.last_client].steer[0]=serverobject.simstate->steer[0]; 
     robbies[serverobject.last_client].steer[1]=serverobject.simstate->steer[1]; 
     robbies[serverobject.last_client].steer[2]=serverobject.simstate->steer[2]; 
     robbies[serverobject.last_client].kick=serverobject.simstate->kick;
     
     howmanyreceived++;
     
     }
     //cout <<"Number of Received Infos: "<<howmanyreceived<<endl;
     
     
     
     for (int i=0;i<MAX_CLIENTS;i++){
     
     if (serverobject.serverdata.client_active[i]){
      // this is worldmodel information for the client
     //cout <<"last_client"<<serverobject.last_client<<endl;
     dReal *dpos;
     dReal *dlook;
    // dpos=new double[3];
     dpos =(dReal*)dGeomGetPosition(box[i]);
     V2d robotPos,robotVel;
     robotPos.setPoint(dpos[0],dpos[1]);
     dlook=(dReal*)dGeomGetRotation(box[i]);
     V2d heading;
     heading.setPoint(dlook[0],-dlook[1]);
     //Set Pos in Communication object
     serverobject.simstate->pos[0]=dpos[0];
     serverobject.simstate->pos[1]=dpos[1];
     serverobject.simstate->pos[2]=heading.angle();
     
     dpos =(dReal*)dBodyGetLinearVel(body[i]);
     robotVel.setPoint(dpos[0],dpos[1]);
     robotVel=robotVel.rotate(-heading.angle());
     serverobject.simstate->robvel[0]=robotVel.x;
     serverobject.simstate->robvel[1]=robotVel.y;
          
     dlook =(dReal*)dBodyGetAngularVel(body[i]);
          //Robvel
     serverobject.simstate->robvel[2]=dlook[2];
     
     
     
        
     dpos =(dReal*)dGeomGetPosition(ball);
     V2d absBall(dpos[0],dpos[1]);
     //cout <<"absball "<<absBall<<endl;
     serverobject.simstate->ball[0]=dpos[0];
     serverobject.simstate->ball[1]=dpos[1];
     for(int j=0;j<16;j++){
     
     dpos =(dReal*)dGeomGetPosition(box[j]);
     serverobject.simstate->obstacles[0][j]=dpos[0];
     serverobject.simstate->obstacles[1][j]=dpos[1];
     
     }
     serverobject.simstate->num_obstacles=16;
     
     
     serverobject.sock.serv_addr=serverobject.serverdata.serv_addr[i];
     serverobject.send();
     }
     else{// when client inactive, reset the steering
     robbies[i].steer[0]=0; 
     robbies[i].steer[1]=0; 
     robbies[i].steer[2]=0; 
     robbies[i].kick=false;

     
     }

    
        
    }
    

    
    
        

     
       
}





static void getinput ()
{
// es werden nur krï¿½te gesetzt, keine Geschwindigkeiten 
// zusï¿½zlich gibt es atmosphï¿½ische reibung Dampening, weil sonst keine rollreibung 
  
  joy.read_all_events(); // read joystick input
  double a0, a1, a2, a3;
  a0 =  pow ((double (joy.axis[0]) / double (joy.MAX_AXIS_VALUE)), 1);
  a1 =  pow ((double (joy.axis[1]) / double (joy.MAX_AXIS_VALUE)), 1);
  a2 =  pow ((double (joy.axis[2]) / double (joy.MAX_AXIS_VALUE)), 1);
  a3 =  pow ((double (joy.axis[3]) / double (joy.MAX_AXIS_VALUE)), 1);

  
  robbies[activerobot].steer[0]=-a1;
  robbies[activerobot].steer[1]=-a0;
  robbies[activerobot].steer[2]=-a2;
  
  
     dBodyAddRelForce (body[activerobot],-a1*20,-a0*20,0);
     dBodyAddRelTorque (body[activerobot], 0, 0,-a2*5);
     if (joy.button[0]==1 && joy.button_chg[0])
     dBodyAddRelForce(body[activerobot],0,0,610);

}  


// simulation loop 1 player im moment Joystick

static void simLoop (int pause)
{
// es werden nur krï¿½te gesetzt, keine Geschwindigkeiten 
// zusï¿½zlich gibt es atmosphï¿½ische reibung Dampening, weil sonst keine rollreibung 
 
 
  followRules();  
  joy.read_all_events(); // read joystick input
  double a0, a1, a2, a3;
  a0 =  pow ((double (joy.axis[0]) / double (joy.MAX_AXIS_VALUE)), 1);
  a1 =  pow ((double (joy.axis[1]) / double (joy.MAX_AXIS_VALUE)), 1);
  a2 =  pow ((double (joy.axis[2]) / double (joy.MAX_AXIS_VALUE)), 1);
  a3 =  pow ((double (joy.axis[3]) / double (joy.MAX_AXIS_VALUE)), 1);

  if (joy.button[1]==1 && joy.button_chg[1]) 
  {
  dBodySetPosition (sphere,0,0,0.5);
  dReal* ballvel;
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere,  (rand()%20+10)*0.1,(rand()%10-5)*0.1f,0);
}
    
  
  int i;
  const dReal * angularv;
  const dReal * linearv;
  const dReal * bangularv;
  const dReal * blinearv;
  double drag_linear;
  double drag_angular;
  double balldrag_linear;
  double balldrag_angular;
  drag_linear=40;
  drag_angular=1.2;
  balldrag_linear=-0.005101f;
  balldrag_angular=-0.005101f;
     
  blinearv =dBodyGetLinearVel (sphere);
  bangularv =dBodyGetAngularVel (sphere);     
  dBodyAddForce(sphere,linearv[0]*balldrag_linear,blinearv[1]*balldrag_linear,blinearv[2]*balldrag_linear);
  dBodyAddTorque(sphere,balldrag_angular*bangularv[0],balldrag_angular*bangularv[1],balldrag_angular*bangularv[2]);
                  
  
  dReal * speed;
  dReal * stpos;
     
  
  if (!pause) {
	
    
    for (i=0; i<N; i++) { 
    // apply forces to all bodies
    if (activerobot==i){
    
     dBodyAddRelForce (body[i],1.35*-a1*60,1.35*-a0*60,0);
     dBodyAddRelTorque (body[i], 0, 0,-a2*6);
      //linearv =dBodyGetLinearVel (body[i]);
      // stpos =(dReal*)dBodyGetPosition(body[i]);
      //dBodyGetPointVel (body[i], stpos[0],stpos[1],stpos[2],speed);
    // dBodyVectorFromWorld (body[i], speed[0],speed[1],speed[2],speed);
    // cout <<fixed<<a0<<" "<<setw(17)<<speed[0]<< " y "  <<setw(17)<<speed[1]<<endl; 
         
    
    }
    
     
     dBodyAddRelForce (body[i],robbies[i].steer[0]*30*1.35,robbies[i].steer[1]*30*1.35,0);
     dBodyAddRelTorque (body[i], 0, 0,robbies[i].steer[2]);
     
     
     // Apply drag forces
        linearv =dBodyGetLinearVel (body[i]);
	angularv =dBodyGetAngularVel (body[i]);
	dBodyAddForce(body[i], -linearv[0]*drag_linear, -linearv[1]*drag_linear,-linearv[2]*drag_linear);
	dBodyAddTorque(body[i],-angularv[0]*drag_angular,-angularv[1]*drag_angular,-angularv[2]*drag_angular);
	//dBodyAddTorque(body[i][j],0,0,-angularv[2]*drag_angular);
      
	dReal *dpos;
     dReal *dlook;
     //dpos=new double[3];
     dpos =(dReal*)dGeomGetPosition(box[i]);
     V2d robotPos;
     //cout<<"activerobot"<<activerobot<<endl;
     robotPos.setPoint(dpos[0],dpos[1]);
     //cout <<robotPos;
     dlook=(dReal*)dGeomGetRotation(box[i]);
     V2d heading;
     heading.setPoint(dlook[0],-dlook[1]);
     Frame2d framerobot;
     framerobot.set_position(robotPos);
     framerobot.set_angle(heading);
     framerobot.invert();
     dpos =(dReal*)dGeomGetPosition(ball);
     V2d absBall(dpos[0],dpos[1]);
      V2d relBall=framerobot*absBall;
      //cout <<relBall<<endl;
      if ( (fabs(relBall.angle_DEG()) < 20)
          && (relBall.length()<0.43)
          )
          {
      V2d glob=((robotPos+heading.normalize().scale(0.30))-absBall).normalize().scale(0.2);
      
      
       if ( (robbies[i].kick)||
       (activerobot==i&&joy.button[3]==1 && joy.button_chg[3]) )
       {   // apply kick
           glob=(absBall-robotPos).normalize().scale(3.1);
           dBodySetLinearVel(sphere,glob.x,glob.y,0.7);
           //dBodyAddForce(sphere,glob.x,glob.y,0.7);
             cout<< "KICKING!!! with"<<glob.x<<" "<<glob.y<<endl;
	   
	     }
else {	
	 dBodyAddForce(sphere,glob.x*3.5,glob.y*3.5 ,-0.50);
      dBodyAddRelForce(sphere, blinearv[0]*balldrag_linear,blinearv[1]*balldrag_linear,blinearv[2]*balldrag_linear);
      }

           }
  
	
	
	
      
      
      }
   
     
    
     
     /*
     force = - drag_coefficient * mass * velocity^2
torque = - angular_drag_coefficient * inertia_matrix * angular_velocity
     */

     /*
     wo ist der Ball vom Roboter aus ?

     */
        
    dSpaceCollide (space,0,&nearCallback);
    dWorldStep (world,0.033);
  
    // remove all contact joints
    dJointGroupEmpty (contactgroup);
  }

  dsSetColor (1,0,1);
  
  
  
  float camspeedpos=0.2;
  float camspeedlook=0.2;
  static float pos[3];
  static float look[4];
  if (viewmode==0)
  {
  pos[0]=0;
  pos[1]=0;
  pos[2]=10;
  look[0]=90.0000;
  look[1]=-90.0000;
  look[2]=0.0000;
  look[3]=0.0000;
  }
  if (viewmode==1)
  {
  dReal* dpos;
  dReal* dlook;
  dpos =(dReal*)dGeomGetPosition(box[activerobot]);
  dlook=(dReal*)dGeomGetRotation(box[activerobot]);
  pos[0]=(float)dpos[0];
  pos[1]=(float)dpos[1];
  pos[2]=(float)dpos[2]+0.4;
  look[0]=(float)dlook[0];
  look[1]=(float)dlook[1];
  look[2]=(float)dlook[2];
  
  look[3]=(float)dlook[3];
  
//  cout<<"  "<<look[0];
//  cout<<"  "<<look[1];
//  cout<<"  "<<look[2];
//  cout<<"  "<<look[3]<<endl;
  //look[0]=atan2(look[0],look[1])*180/M_PI;
   look[0]=(atan2(look[0],look[1])-M_PI/2)*180/M_PI;

  look[1]=-25;
  //pos[2]=pos[2]-look[2];
  //look=(float*)dGeomGetRotation(box[0][0]);
  //pos[2]=*pos[2]+1.5;
  camspeedpos=0.4;
  camspeedlook=0.9;
  }

  if (viewmode==2)
  {
  dReal* dpos;
  dReal* dlook;
  dpos =(dReal*)dGeomGetPosition(ball);
  dlook=(dReal*)dGeomGetRotation(ball);
  
  pos[0]=(float)dpos[0];
  pos[1]=(float)dpos[1];
  pos[2]=(float)+4.4;
  look[0]=(float)90;
  look[1]=(float)0;
  look[2]=(float)0;
  
  look[3]=(float)0;
  look[1]=-90;
  camspeedpos=0.05;
  camspeedlook=0.05;
  }
 
  for (int i=0;i<3;i++){
  camsmooth[i]=camsmooth[i]*(1-camspeedpos)+pos[i]*camspeedpos;
  camlooksmooth[i]=camlooksmooth[i]*(1-camspeedlook)+look[i]*camspeedlook;
  }
  
  dsSetViewpoint (camsmooth,camlooksmooth);
 
   
  
  dReal sides[3] = {LENGTH,LENGTH,HEIGHT};
  dReal headsides[3] = {LENGTH*0.9,LENGTH*0.3f,HEIGHT*1.1f};
  for (i=0; i<N; i++) {
      dsSetColor (1,0,1);
      if ( (i/4)%2 )
      dsSetColor (1,0,1);
      else 
      dsSetColor (0,1,0);
  
      
      dsDrawBox (dGeomGetPosition(box[i]),dGeomGetRotation(box[i]),
		 sides);
      
      dsSetColor (1,1,0);
      // Heading von den Robotern zeichnen 
      if(i==activerobot) dsSetColor(1,0,0);
      dsDrawBox (dGeomGetPosition(box[i]),dGeomGetRotation(box[i]),
		 headsides);

    }


  if (goaldelay>0)goaldelay++;
  if (goaldelay>60){
  dBodySetPosition (sphere,0,0,0);
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere, 0,0,0);
  dBodySetAngularVel  (sphere, 0,0,0	);
  goaldelay=0;
}



/*  //immovablebox
  immovablebox[0] = dCreateBox (space,0.1,2,1);//goal_backside
  immovablebox[1] = dCreateBox (space,0.1,2,1);//goal_backside
  immovablebox[2] = dCreateBox (space,0.5,0.1,1);//goal_backside
  immovablebox[3] = dCreateBox (space,0.5,0.1,1);//goal_backside
  immovablebox[4] = dCreateBox (space,0.5,0.1,1);//goal_backside
  immovablebox[5] = dCreateBox (space,0.5,0.1,1);//goal_backside
*/
  dReal sides0[3] = {0.1,2.2,1};
  dReal sides1[3] = {0.6,0.1,1};
  dsSetColor (1,1,0);
  dsDrawBox (dGeomGetPosition(immovablebox[0]),dGeomGetRotation(immovablebox[0]),sides0);
  dsDrawBox (dGeomGetPosition(immovablebox[2]),dGeomGetRotation(immovablebox[2]),sides1);
  dsDrawBox (dGeomGetPosition(immovablebox[4]),dGeomGetRotation(immovablebox[4]),sides1);
  dsSetColor (0,0,1);
  
  dsDrawBox (dGeomGetPosition(immovablebox[1]),dGeomGetRotation(immovablebox[1]),sides0);
  dsDrawBox (dGeomGetPosition(immovablebox[3]),dGeomGetRotation(immovablebox[3]),sides1);
  dsDrawBox (dGeomGetPosition(immovablebox[5]),dGeomGetRotation(immovablebox[5]),sides1);
  
  dsSetColor (1,0.4,0);
  dsDrawSphere (dGeomGetPosition(ball),dGeomGetRotation(ball),
		 BALL_RADIUS);
  drawField();  
  dsSetColor (1,1,0);
  /*
  dReal goalsides[3] = {0.5,2,1};
  dsDrawBox (dGeomGetPosition(geomgoal),dGeomGetRotation(geomgoal),goalsides);
  */
  simloopCount++; 
  //cout<<"Yeah!!  "<<robots[0].steer[0];

  communicate();  
  
  usleep(50000);

}

static void command (int cmd)
{

//  cout <<"Taste:"<<cmd<<endl;
  size_t i;
  int j,k;
 const dReal* bpos;
 bpos=dBodyGetPosition(sphere);
 
  if (cmd == 'b') {
     move_ball=!move_ball;
     cout<< "BAll Move is"<<move_ball<<endl;  
  }
  if (cmd == 'w') {
     if (move_ball) {
     dBodySetPosition (sphere,bpos[0],bpos[1]+.5,bpos[2]);
     dBodySetLinearVel  (sphere,  0,0,0);
     
     }
     else dBodyAddRelForce (body[activerobot],80,0,0);
    }
  if (cmd == 's') {
     if (move_ball)
        {
         dBodySetPosition (sphere,bpos[0],bpos[1]-.5,bpos[2]);
         dBodySetLinearVel  (sphere,  0,0,0);
        }
  else dBodyAddRelForce (body[activerobot],-80,0,0);
    }
  if (cmd == 'd') {
     if (move_ball) 
     {
        dBodySetPosition (sphere,bpos[0]+.5,bpos[1],bpos[2]);
              dBodySetLinearVel  (sphere,  0,0,0);
        }
  
     else dBodyAddRelForce (body[activerobot],0,-80,0);
    }
  if (cmd == 'a') {
     if (move_ball) {
     dBodySetPosition (sphere,bpos[0]-.5,bpos[1],bpos[2]);
    dBodySetLinearVel  (sphere,  0,0,0);
        }
  
     else dBodyAddRelForce (body[activerobot],0,80,0);
    }
  if (cmd == 'k') {
     dBodyAddRelTorque (body[activerobot], 0, 0,60.0f/10);
    }
  if (cmd == 'l') {
     dBodyAddRelTorque (body[activerobot], 0, 0,-60.0f/10);
    }
  if (cmd == 'r') {
  dBodySetPosition (sphere,0,-1,1);
    }
  if (cmd == 't') {
  viewmode=(viewmode+1)%3;
    }
  if (cmd == 'v') {
  dBodySetPosition (sphere,0,0,0.5);
  dReal* ballvel;
  //(dReal*)dBodyGetLinearVel (sphere);
  dBodySetLinearVel  (sphere,  (rand()%20+10)*0.1,(rand()%10-5)*0.1f,0);
         dBodySetAngularVel  (sphere, 0,0,0	);

	 
	  
    }
  if (cmd == 'f') {
  setFormation();
  cout<<"Setting Formation!!"<<endl;;
  
 
    }
  
  if (cmd == 'n') {
  activerobot=(activerobot+1)%N;
  cout<<"activating RObot no "<<activerobot<<endl;;
 
    }
  if (cmd == 'N') {
  activerobot=(activerobot-1);
  if (activerobot<0)activerobot=N-1;
  cout<<"activating RObot no "<<activerobot<<endl;;
 
    }
  
   if (cmd == 'c') {
    cout<<"saving ball data"<<endl;;
     dReal *d;
     d =(dReal*)dBodyGetPosition (sphere);
     savedBallPos[0]=d[0];  savedBallPos[1]=d[1];  savedBallPos[2]=d[2];
     d =(dReal*)dBodyGetLinearVel (sphere);
     savedBallVel[0]=d[0];  savedBallVel[1]=d[1];  savedBallVel[2]=d[2];
     d =(dReal*)dBodyGetAngularVel (sphere);
     savedBallAngVel[0]=d[0];  savedBallAngVel[1]=d[1];  savedBallAngVel[2]=d[2];
   
    }
  if (cmd == 'p') {
    cout<<"recalling ball data"<<endl;;
     dBodySetPosition (sphere,savedBallPos[0],savedBallPos[1],savedBallPos[2]);
     dBodySetLinearVel  (sphere,  savedBallVel[0],savedBallVel[1],savedBallVel[2]);
     dBodySetAngularVel  (sphere, savedBallAngVel[0],savedBallAngVel[1],savedBallAngVel[2]);
 
    }
  if (cmd == 'P') {
    cout<<"Replace Ball and shoot"<<endl;;
    dReal* dpos;
    dReal* dlook;
    dpos =(dReal*)dGeomGetPosition(box[activerobot]);
    dlook=(dReal*)dGeomGetRotation(box[activerobot]);
       dBodySetPosition (sphere,dpos[0]+dlook[0]/2,dpos[1]-dlook[1]/2,0.2);
     dBodySetLinearVel  (sphere,  dlook[0]*shootspeed,  -dlook[1]*shootspeed,  0);
     dBodySetAngularVel  (sphere, 0,0,0);
 
    }
  if (cmd == '+') {
    shootspeed+=0.1;
    cout<<"Increase Ball Replacement  Shoot Speed to "<<shootspeed<< endl;;
    }
  if (cmd == '-') {
    shootspeed-=0.1;
    cout<<"Decreasing Ball Replacement  Shoot Speed to "<<shootspeed<<endl;;
    }
  
  if (cmd == '1') {
    cout<<"Recalling Saved State of Simulation !!!!"<<shootspeed<< endl;;
    recallSavedState(0);
    }
  if (cmd == '!') {
    saveState(0);
    cout<<"Saving State of Simulation"<<shootspeed<<endl;;
    }
  
  if (cmd == '2') {
    cout<<"Recalling Saved State of Simulation !!!!"<<shootspeed<< endl;;
    recallSavedState(1);
    }
  if (cmd == '"') {
    saveState(1);
    cout<<"Saving State of Simulation"<<shootspeed<<endl;;
    }
    
  if (cmd == 'S') {
    saveStatesToDisk();
    cout<<"Saving States to Disk"<<shootspeed<<endl;;
    }
    
  if (cmd == 'L') {
    loadStatesFromDisk();
    cout<<"Load States from Disk"<<shootspeed<<endl;;
    }
      
  
  }



int main (int argc, char **argv)
{
  serverobject.use_as_server(30001);
  serverobject.use_nonblock();

  
camsmooth=new float[3];
camlooksmooth=new float[3];
camsmooth[0]=0;camlooksmooth[0]=0;
camsmooth[1]=0;camlooksmooth[1]=0;
camsmooth[2]=0;camlooksmooth[1]=0;

  goaldelay=0;
  
    
  activerobot=0;
  int i,j;
  dMass m;
  joy.init("/dev/input/js0");
  // setup pointers to drawstuff callback functions
  dsFunctions fn;
  fn.version = DS_VERSION;
  fn.start = &start;
  fn.step = &simLoop;
  fn.command = &command;

  fn.stop = 0;
  fn.path_to_textures = "../../drawstuff/textures";

  // create world
//      dsSetColor (0,.8,0);
  
  world = dWorldCreate();
  space = dHashSpaceCreate (0);
  contactgroup = dJointGroupCreate (0);
  dWorldSetGravity (world,0,0,-GRAVITY);
  
  ground = dCreatePlane (space,0,0,1,0);

  //robbies[1].steer[0]=0;

  // bodies
  for (i=0; i<N; i++) {
      body[i] = dBodyCreate (world);
      dMassSetBox (&m,1,LENGTH,LENGTH,HEIGHT);
      dMassAdjust (&m,MASS);
      dBodySetMass (body[i],&m);
      dBodySetPosition (body[i],i*2.2*LENGTH+1,i/4*2.2*LENGTH+1,HEIGHT*0.5);

      box[i] = dCreateBox (space,LENGTH,LENGTH,HEIGHT);
      dGeomSetBody (box[i],body[i]);

    
  }
      
  immovablebox[0] = dCreateBox (space,0.1,2.2,1);//goal_backside
  immovablebox[1] = dCreateBox (space,0.1,2.2,1);//goal_backside
  immovablebox[2] = dCreateBox (space,0.6,0.1,1);//goal_backside
  immovablebox[3] = dCreateBox (space,0.6,0.1,1);//goal_backside
  immovablebox[4] = dCreateBox (space,0.6,0.1,1);//goal_backside
  immovablebox[5] = dCreateBox (space,0.6,0.1,1);//goal_backside
  dGeomSetBody (immovablebox[0],0);
  dGeomSetBody (immovablebox[1],0);
  dGeomSetBody (immovablebox[2],0);
  dGeomSetBody (immovablebox[3],0);
  dGeomSetBody (immovablebox[4],0);
  dGeomSetBody (immovablebox[5],0);
  dGeomSetPosition (immovablebox[0], -FIELD_LENGTH/2-0.6, 0, 0.3);
  dGeomSetPosition (immovablebox[1], FIELD_LENGTH/2+0.6, 0, 0.3);
  dGeomSetPosition (immovablebox[2], -FIELD_LENGTH/2-0.3, -1.1, 0.3);
  dGeomSetPosition (immovablebox[3], FIELD_LENGTH/2+0.3, -1.1, 0.3);
  dGeomSetPosition (immovablebox[4], -FIELD_LENGTH/2-0.3, 1.1, 0.3);
  dGeomSetPosition (immovablebox[5], FIELD_LENGTH/2+0.3, 1.1, 0.3);
  
  //void dGeomSetRotation (dGeomID, const dMatrix3 R);
  
setFormation();

        
  /*    
      bodygoal = dBodyCreate (world);
      dMassSetBox (&m,1,0.5,2,1);
      dMassAdjust (&m,MASS);
      dBodySetMass (bodygoal,&m);
      dBodySetPosition (bodygoal,-FIELD_LENGTH/2,0,1);

      geomgoal = dCreateBox (space,0.5,2,1);
      dGeomSetBody (geomgoal,bodygoal);
    */   
  
  
  dMassAdjust (&m,0.4);
  sphere =dBodyCreate(world);
  dMassSetSphere (&m, 10.4, BALL_RADIUS);
  dBodySetMass (sphere,&m);
  dBodySetPosition (sphere,0,0,1);
  ball=dCreateSphere (space,BALL_RADIUS);
  dGeomSetBody (ball,sphere);
  
saveState(0);
saveState(1);
      
        
  // run simulation
  dsSimulationLoop (argc,argv,352,288,&fn);

  dJointGroupDestroy (contactgroup);
  dSpaceDestroy (space);
  dWorldDestroy (world);

  return 0;
}


