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

/* TribotsUdpCommunication class
 *
 * Class for communication of special tags and information of the robot's
 * world information.
 * The class can be used bidirectional for communication of remote control
 * signals of the robot and the state of the robot.
 *
 */

#ifndef _TRIBOTS_UDPCOMMUNICATION_H_
#define _TRIBOTS_UDPCOMMUNICATION_H_

// Versionsnummer vom Protokoll
#define _VERSION_UDPCOMMUNICATION_HAUPT_ 12


#include "udpsocket.h"
#include "../Structures/DriveVector.h"
#include "../Structures/FieldGeometry.h"
#include "../Structures/BallLocation.h"
#include "../Structures/RobotProperties.h"
#include "../Structures/RobotLocation.h"
#include "../Structures/RobotData.h"
#include "../Structures/GameState.h"
#include "../Structures/ObstacleLocation.h"
#include "../Structures/VisibleObject.h"
#include "tribotsUdpCommunicationBuffer.h"

#include <vector>
#include <string>

using namespace std;

namespace Tribots {

class TribotsUdpCommunication {

 protected:
  // some tags for communication
  static const char PingTag            = 'a';
  static const char RequestSettingsTag = 'b';
  static const char RemoteCtrTag       = 'c';
  static const char GameStateTag       = 'd';
  static const char OwnHalfTag         = 'e';
  static const char FieldGeometryTag   = 'f';
  static const char BallLocationTag    = 'g';
  static const char RobotPropertiesTag = 'h';
  static const char RobotLocationTag   = 'i';
  static const char PlayerTypeTag      = 'j';
  static const char RobotDataTag       = 'k';
  static const char XmlStringTag       = 'x';
 
 // define some indexes and structs for storing things
  static const int PingIdx            = 0;
  static const int RequestSettingsIdx = 1;
  static const int RemoteCtrIdx       = 2;
  static const int GameStateIdx       = 3;
  static const int OwnHalfIdx         = 4;
  static const int FieldGeometryIdx   = 5;
  static const int BallLocationIdx    = 6;
  static const int RobotPropertiesIdx = 7;
  static const int RobotLocationIdx   = 8;
  static const int PlayerTypeIdx      = 9;
  static const int RobotDataIdx       = 10;
  static const int XmlStringIdx       = 11;
  static const int NumIdx             = 12;
  int  received[NumIdx];

  vector<string> ReceivedXMLStrings;
  vector<string> ReceivedXMLCmds;
  vector<string> ReceivedXMLRequests;
  vector<string> ReceivedXMLData;

  // the socket for communication
  UDPsocket sock;

  // buffer for communication
  static const int BUFFER_MAX_LEN= 16384;  
  //static const int BUFFER_MAX_LEN= 8000;
   // fehlerhafte pakete auf 8096 wurden empfangen, mal versuchen was passiert wenn man drunter bleibt. 
   // Da muss es ne grenze geben bei der uebertragung, mit 8000 wurden keine pakete abgeschnitte.
  int buffer_len;
  char buffer[BUFFER_MAX_LEN];
  
  bool is_blocking;

  //protected functions
  bool put_to_buffer(void * src, int size, char tag,bool rueckschein,unsigned char prio);
  bool get_from_buffer(void * obj, void * src, int size);
  
  enum RequestState { never_send, send_once};   // maybe later , send_reqular };
  struct RequestSettings {
    RequestSettings();
    RequestState request[NumIdx];
    void clear();
    bool set_all(RequestState state);
    bool set_request(int MsgIdx, RequestState state);
  };

  
  RequestSettings * requestSettings;
  DriveVector     * RemoteCtr;
  
  // some pointers to read from
  GameState       * gameState;
  int             * ownHalf;
  FieldGeometry   * fieldGeometry;
  BallLocation    * ballLocation;
  RobotProperties * robotProperties;
  RobotLocation   * robotLocation;
  char            * playerType;
  RobotData       * robotData;

  // Die neue Puffer klasse
  TribotsUdpCommunicationBuffer * com_Buffer;
     
  bool use_as_server(int port);
  bool use_as_client(char const* addr, int port);
  
  bool use_nonblock();
 
  bool udp_receive();
  bool udp_receive_all(int timeout_sec=1);

  bool processXmlStrings();

  public:
  TribotsUdpCommunication(bool server=true, int port=6012, char const* addr=0);
  ~TribotsUdpCommunication();
   
  bool clear_to_send();
  bool send();

  bool receive_all();

  bool putRequests(bool rueckschein=false,unsigned char prio=1);// the request set will be put to buffer for sending client will use this
  bool clearRequests();                  // old requests will be cleared client will use this
  bool setGameStateRequest();            // set the requests for information client will use this
  bool setOwnHalfRequest();              // ...
  bool setFieldGeometryRequest();
  bool setBallLocationRequest();
  bool setRobotPropertiesRequest();
  bool setRobotLocationRequest();
  bool setPlayerTypeRequest();
  bool setRobotDataRequest();
  bool setObstacleLocationRequest();
  bool setVisibleObjectListRequest();
  // No Request for Messages ... tested and sent every cycle

  bool setPlayerTypeListRequest();

  bool putInGame (bool& ig,bool rueckschein=true,unsigned char prio=0);
  bool getInGame (bool& ig);

  bool putRefboxSignal (RefboxSignal& rs,bool rueckschein=true,unsigned char prio=0); 
  bool getRefboxSignal (RefboxSignal& rs); 

  bool getRemoteCtr(DriveVector &drv);
  bool putRemoteCtr(DriveVector &drv,bool rueckschein=false,unsigned char prio=1);

  bool getGameState(GameState &gs);
  bool putGameState(GameState &gs,bool rueckschein=false,unsigned char prio=0);

  bool getOwnHalf(int &oh);
  bool putOwnHalf(int &oh,bool rueckschein=false,unsigned char prio=0);

  bool getFieldGeometry(FieldGeometry& fg);
  bool putFieldGeometry(FieldGeometry& fg,bool rueckschein=false,unsigned char prio=1);

  bool getBallLocation(BallLocation& bl);
  bool putBallLocation(BallLocation& bl,bool rueckschein=false,unsigned char prio=1);

  bool getRobotProperties(RobotProperties& rp);
  bool putRobotProperties(RobotProperties& rp,bool rueckschein=false,unsigned char prio=1);

  bool getRobotLocation(RobotLocation& rl);
  bool putRobotLocation(RobotLocation& rl,bool rueckschein=false,unsigned char prio=1);

  bool getPlayerType(char * pt);
  bool getPlayerType(std::string &pt);
  bool putPlayerType(char const * pt,bool rueckschein=false,unsigned char prio=1);

  bool getPlayerTypeList(vector<string>& ptl);
  bool putPlayerTypeList(const vector<string>& ptl,bool rueckschein=true,unsigned char prio=1);

  bool getRobotData(RobotData& rd);
  bool putRobotData(const RobotData& rd,bool rueckschein=false,unsigned char prio=1);

  bool putMessage(const string& msg,bool rueckschein=false,unsigned char prio=1);
  bool putMessageList(const vector<string>& msgList,bool rueckschein=false,unsigned char prio=1);
  bool getMessageList(vector<string>& msgList);

  bool putObstacleLocation(const ObstacleLocation & ol,bool rueckschein=false,unsigned char prio=2);
  bool getObstacleLocation(ObstacleLocation & ol);

  bool putVisibleObjectList(const VisibleObjectList &ol,bool rueckschein=false,unsigned char prio=2);
  bool getVisibleObjectList(VisibleObjectList &ol);

  // Some Xml func to try com, use for new commands
  bool putXmlString(const char * s,bool rueckschein=false,unsigned char prio=1);

  bool putSLHint(float x_mm, float y_mm, float heading_rad,bool rueckschein=false,unsigned char prio=1);
  bool putSLHint(float x_mm, float y_mm,bool rueckschein=false,unsigned char prio=1);

  bool getSLHint(float &x_mm, float &y_mm, float &heading_rad);
  bool getSLHint(float &x_mm, float &y_mm);

  void getXmlSubstringList(std::string tag, std::string src, std::vector<std::string> &target);
  
  unsigned int getVersion();
};
}
#endif
