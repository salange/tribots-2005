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


#include "../ControlLoop/ControlLoop.h"
#include "../Structures/Journal.h"
#include "../Fundamental/random.h"
#include <string>

int main (int argc, char** argv) {
  std::string configfile = ("../config_files/robotcontrol.cfg");
  if (argc>1) {
    std::string arg1 = argv[1];
    if (arg1=="--help" || arg1=="-h") {
      std::cerr << "Aufruf: " << argv[0] << " [Konfigurationsdatei]\n";
      std::cerr << "wird keine Konfigurationsdatei angegeben, wird automatisch\n";
      std::cerr << "../config_files/robotcontrol.cfg verwendet\n";
      return -1;
    } else 
      configfile = argv[1];
  }
  try{

    Tribots::ConfigReader vread (0);
    bool success = vread.append_from_file (configfile.c_str());
    if (!success) {
      std::cerr << "Fehler: konnte Konfigurationsdateien nicht vollstaendig lesen\n";
      std::cerr <<"Konfigurationsdatei war: " << configfile << '\n';
      return -1;
    }
    Tribots::Journal::the_journal.set_mode (vread);
    JMESSAGE((std::string("use config-file ")+configfile).c_str());
    
    Tribots::ControlLoop the_control_loop (vread);
    the_control_loop.loop();

    unsigned int ui;
    if (vread.get ("random_seed", ui)>0)
      Tribots::random_seed (ui);

  }catch(Tribots::TribotsException& e){
    std::cerr << e.what() << "\n\r" << std::flush;
    Tribots::Journal::the_journal.error (__FILE__, __LINE__, e.what());
    return -1;
  }catch(std::exception& e){ 
    std::cerr << e.what() << "\n\r" << std::flush;
    Tribots::Journal::the_journal.error (__FILE__, __LINE__, e.what());
    return -1;
  }
  return 0;
}

