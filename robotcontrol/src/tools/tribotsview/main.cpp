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
#include "tribotsview.h"


int main (int argc, char *argv[]) {
  try{
    std::string logfile = "wminfo";
    std::string cfgfile = "";
    if (argc>1) {
      if (std::string(argv[1])=="--help" || std::string(argv[1])=="-h") {
	std::cout << "Aufruf: " << argv[0] << " [Logdatei-Praefix] [Konfigurationsdatei]\n";
	std::cout << "wird Logdatei-Praefix weggelassen, wird wminfo gewaehlt\n";
	std::cout << "wird Konfigurationsdatei weggelassen, wird die Feldgeometrie\n";
	std::cout << "aus dem Logfile gelesen, falls vorhanden\n";
	return -1;
      } else {
	logfile = argv[1];
	if (argc>2)
	  cfgfile = argv[2];
      }
    }
  
    QApplication app (argc,argv);

    tribotsview tv (0,"main window");
    tv.init_field_and_streams (logfile.c_str(), cfgfile.c_str());

    tv.setCaption("Tribotsview");
    app.setMainWidget(&tv);
    tv.show();

    return app.exec ();
  }catch(std::exception& e){
    std::cerr << e.what() << std::endl;
    return -1;
  }
}



