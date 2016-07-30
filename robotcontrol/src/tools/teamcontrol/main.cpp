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


#include <qapplication.h>
#include <iostream>
#include "RefboxWidget.h"
#include "RemoteRobotWidget.h"
#include "TeamControlWidget.h"
#include "../../Structures/Journal.h"

int main (int argc, char *argv[]) {
  try{
    Tribots::ConfigReader cfg;
    std::string cfg_file = "teamcontrol.cfg";
    if (argc>1) {
      if (std::string(argv[1])=="--help" || std::string(argv[1])=="-h") {
	std::cout << "Aufruf: " << argv[0] << " [Konfigurationsdatei]\n";
	std::cout << "Fehlt die Konfigurationsdatei, wird teamcontrol.cfg verwendet\n";
	return -1;
      } else {
	cfg_file = argv[1];
      }
    }
    cfg.append_from_file (cfg_file.c_str());
    QApplication app (argc,argv);

    Tribots::Journal::the_journal.set_mode (cfg);

    TeamControlWidget tcw (0,"main window");
    app.setMainWidget(&tcw);
    tcw.init (cfg);
    tcw.show();
    
  
    return app.exec ();
  }catch (Tribots::TribotsException& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }catch (std::exception& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }
}



