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


#include "../../Structures/BallLocationReadWriter.h"
#include "../../Structures/DriveVectorReadWriter.h"
#include "../../Structures/GameStateReadWriter.h"
#include "../../Structures/ObstacleLocationReadWriter.h"
#include "../../Structures/RobotLocationReadWriter.h"
#include "../../Structures/VisibleObjectReadWriter.h"
#include "../../Fundamental/stringconvert.h"
#include <fstream>

using namespace Tribots;
using namespace std;

int main (int argc, char** argv) {
  if (argc<4) {
    cerr << "Aufruf: " << argv[0] << " Zieldatei-Praefix Quelldatei-Praefix Zeitraum(ms)\n";
    cerr << "schneidet einen gegebenen Zeitraum aus Logfiles aus\n";
    cerr << "Formatierung des Zeitraums: von-bis\n";
    cerr << "Beruecksichtigt Robot- Ball-, ObstacleLocation, Odometrie, Fahrtvektroen, Bildinformation, .log-Datei\n";
    return -1;
  }
  try{

    string src_praefix = argv[2];
    string dest_praefix = argv[1];
    string interval = argv[3];
    unsigned int idash = interval.find ('-');
    if (idash>interval.length())
      throw invalid_argument ("Zeitraum nicht korrekt angegeben");
    unsigned long int from=0;
    unsigned long int to=999999999;
    if (idash>0) string2ulint (from, interval.substr(0,idash));
    if (idash+1<interval.length()) string2ulint (to, interval.substr(idash+1,interval.length()));

    // RobotLocation:
    {
      ifstream src ((src_praefix+".rpos").c_str());
      RobotLocationReader reader (src);
      ofstream dest ((dest_praefix+".rpos").c_str());
      RobotLocationWriter writer (dest);
      unsigned long int tt;
      bool neof = reader.next_timestamp (tt);
      while (neof) {
	unsigned long int t1, t2;
	RobotLocation info1, info2;
	reader.read_until (tt, t1, info1, t2, info2, tt);
	if (from<=tt && tt<=to)
	  writer.write (tt-from, t1-from, info1, t2-from, info2);
	neof = reader.next_timestamp (tt);
      }
      dest << flush;
    }

    // BallLocation:
    {
      ifstream src ((src_praefix+".bpos").c_str());
      BallLocationReader reader (src);
      ofstream dest ((dest_praefix+".bpos").c_str());
      BallLocationWriter writer (dest);
      unsigned long int tt;
      bool neof = reader.next_timestamp (tt);
      while (neof) {
	unsigned long int t1, t2;
	BallLocation info1, info2;
	reader.read_until (tt, t1, info1, t2, info2, tt);
	if (from<=tt && tt<=to)
	  writer.write (tt-from, t1-from, info1, t2-from, info2);
	neof = reader.next_timestamp (tt);
      }
      dest << flush;
    }

    // ObstacleLocation:
    {
      ifstream src ((src_praefix+".opos").c_str());
      ObstacleLocationReader reader (src);
      ofstream dest ((dest_praefix+".opos").c_str());
      ObstacleLocationWriter writer (dest);
      unsigned long int tt;
      bool neof = reader.next_timestamp (tt);
      while (neof) {
	ObstacleLocation info1;
	reader.read_until (tt, info1, tt);
	if (from<=tt && tt<=to)
	  writer.write (tt-from, info1);
	neof = reader.next_timestamp (tt);
      }
      dest << flush;
    }

    // DriveVector:
    {
      ifstream src ((src_praefix+".drv").c_str());
      DriveVectorReader reader (src);
      ofstream dest ((dest_praefix+".drv").c_str());
      DriveVectorWriter writer (dest);
      unsigned long int tt;
      bool neof = reader.next_timestamp (tt);
      while (neof) {
	unsigned long int t1;
	DriveVector info1;
	reader.read_until (tt, t1, info1, tt);
	if (from<=tt && tt<=to)
	  writer.write (tt-from, t1-from, info1);
	neof = reader.next_timestamp (tt);
      }
      dest << flush;
    }

    // Odometrie:
    {
      ifstream src ((src_praefix+".odo").c_str());
      DriveVectorReader reader (src);
      ofstream dest ((dest_praefix+".odo").c_str());
      DriveVectorWriter writer (dest);
      unsigned long int tt;
      bool neof = reader.next_timestamp (tt);
      while (neof) {
	unsigned long int t1;
	DriveVector info1;
	reader.read_until (tt, t1, info1, tt);
	if (from<=tt && tt<=to)
	  writer.write (tt-from, t1-from, info1);
	neof = reader.next_timestamp (tt);
      }
      dest << flush;
    }

    // VisibleObject:
    {
      ifstream src ((src_praefix+".vis").c_str());
      VisibleObjectReader reader (src);
      ofstream dest ((dest_praefix+".vis").c_str());
      VisibleObjectWriter writer (dest);
      unsigned long int tt;
      bool neof = reader.next_timestamp (tt);
      while (neof) {
	unsigned long int t1;
	VisibleObjectList info1;
	reader.read_until (tt, t1, info1, tt);
	if (from<=tt && tt<=to)
	  writer.write (tt-from, t1-from, info1);
	neof = reader.next_timestamp (tt);
      }
      dest << flush;
    }

    // GameState:
    {
      ifstream src ((src_praefix+".gs").c_str());
      GameStateReader reader (src);
      ofstream dest ((dest_praefix+".gs").c_str());
      GameStateWriter writer (dest);
      unsigned long int tt;
      bool neof = reader.next_timestamp (tt);
      while (neof) {
	GameState info1;
	reader.read_until (tt, info1, tt);
	if (from<=tt && tt<=to)
	  writer.write (tt-from, info1);
	neof = reader.next_timestamp (tt);
      }
      dest << flush;
    }

    // .log-Datei:
    {
      ifstream src ((src_praefix+".log").c_str());
      ofstream dest ((dest_praefix+".log").c_str());
      bool is_active=false;
      while (!src.eof()) {
	string line;
	getline (src, line);
	if (src.eof())
	  break;
	if (prefix (string("\%\%\%\%cycle"), line)) {
	  unsigned int idx = line.find ('\t');
	  unsigned long int tt;
	  string2ulint (tt, line.substr (10, idx-10));
	  if (from<=tt && tt<=to) {
	    dest << line.substr(0,10) << tt-from << line.substr (idx, line.length()) << '\n';
	    is_active=true;
	  } else
	    is_active=false;
	} else if (is_active) {
	  dest << line << '\n';
	}
      }
      dest << flush;
    }



  }catch(std::exception& e){
    cerr << e.what() << endl;
    return -1;
  }
  return 0;
}
