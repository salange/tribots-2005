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


#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

int main (int argc, char** argv) {
  if (argc<5) {
    cerr << "Auswahl einer Teilsequenz aus einer Sammlung von Logfiles\n";
    cerr << argv[0] << " Zieldatei-Praefix Quelldatei-Praefix Startzeitpunkt Endzeitpunkt\n";
    return -1;
  }

  string dest_praefix = argv[1];
  string src_praefix = argv[2];
  unsigned long int msec_begin, msec_end;
  char* end_char;
  msec_begin = strtoul (argv[3], &end_char, 0);
  msec_end = strtoul (argv[4], &end_char, 0);

  unsigned int num_files = 7;
  string suffixe [num_files];
  suffixe[0]="bpos";
  suffixe[1]="rpos";
  suffixe[2]="opos";
  suffixe[3]="ppos";
  suffixe[4]="drv";
  suffixe[5]="odo";
  suffixe[6]="vis";
  int num_timestamps [] = { 1,1,1,1,2,2,2 };
  
  // zuerst die "einfachen" Dateien mit Format "Zeitstempel Rest"
  for (unsigned int i=0; i<num_files; i++) {
    string src_name = src_praefix+string(".")+suffixe[i];
    string dest_name = dest_praefix+string(".")+suffixe[i];

    ifstream src (src_name.c_str());
    ofstream dest (dest_name.c_str());
    if (!src || !dest) {
      cerr << "error while opening " << src_name << " and " << dest_name << "; skip\n";
      continue;
    }
    cout << "converting " << src_name << " to " << dest_name << '\n';
    string line, part1, part2, part3, part4;
    unsigned int j;
    while (!src.eof()) {
      getline (src, line);
      j=0;
      while (j<line.length()) {
	if (isspace (line[j])) {
	  part1=line.substr(0,j);
	  part2=line.substr(j,line.length());
	  break;
	}
	j++;
      }
      if (j==line.length())
	continue;   // Zeile ohne whitespace???
      
      unsigned long int tt;
      tt = strtoul (part1.c_str(), &end_char, 0);
      if (tt>=msec_begin && tt<=msec_end) {
	if (num_timestamps[i]==1)
	  dest << tt-msec_begin << part2 << '\n';
	else {
	  bool scnd=false;
	  j=0;
	  while (j<part2.length()) {
	    if (isspace (part2[j])) {
	      if (scnd) {
		part3=part2.substr(0,j);
		part4=part2.substr(j,part2.length());
		break;
	      } else
		j++;
	    } else {
	      scnd=true;
	      j++;
	    }
	  }
	  unsigned long int tt2;
	  tt2 = strtoul (part3.c_str(), &end_char, 0);
	  if (tt2>msec_begin)
	    dest << tt-msec_begin << '\t' << tt2-msec_begin << part4 << '\n';
	}
      }
    }
  }

  // jetzt noch die .log-Datei
  {
    string src_name = src_praefix+string(".log");
    string dest_name = dest_praefix+string(".log");

    ifstream src (src_name.c_str());
    ofstream dest (dest_name.c_str());
    if (!src || !dest) {
      cerr << "error while opening " << src_name << " and " << dest_name << "; skip\n";
      return 0;
    }
    cout << "converting " << src_name << " to " << dest_name << '\n';
    string line, part1, part2;
    bool mode = false;
    while (!src.eof()) {
      getline (src, line);
      if (line.substr (0,10)==string("%%%%cycle ")) {
	unsigned int tt = strtoul (line.substr(10,line.length()).c_str(), &end_char, 0);
	if (tt>=msec_begin && tt<=msec_end) {
	  dest << "%%%%cycle " << tt-msec_begin << '\n';
	  mode=true;
	} else
	  mode=false;
      } else if (mode)
	dest << line << '\n';
    }	
  }

  return 0;
}
