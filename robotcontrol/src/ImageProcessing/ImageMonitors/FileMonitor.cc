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

#include "FileMonitor.h"

#include <sstream>
#include <fstream>

namespace Tribots {

  using namespace std;
  
  FileMonitor::FileMonitor(ImageIO* imageIO, const std::string& filename,
			   int step, bool singleFile)
    : ImageMonitor(), imageIO(imageIO), logOut(0), imgOut(0), 
      filename(filename), singleFile(singleFile), counter(0), step(step)
  {
    logOut = new ofstream((filename+".log").c_str());

    if (singleFile) {
      string ending = imageIO->getDefEnding();
      string imgFilename = filename+ending+"_stream";
      imgOut = new ofstream(imgFilename.c_str());

      (*logOut) << "stream " << imageIO->getTypeId() 
		<< " " << imgFilename << endl;
    }
    else {
      (*logOut) << "single " << imageIO->getTypeId() << " " << endl;
    }    
  }

  FileMonitor::~FileMonitor()
  {
    if (logOut) {
      logOut->close();
      delete logOut;
    }
    if (imgOut) {
      imgOut->close();
      delete imgOut;
    }
    if (imageIO) {
      delete imageIO;
    }
  }

  void FileMonitor::monitor(const Image& image, 
			    const Time& available, const Time& created)
  {
    if (counter % step != 0) {
      counter++;   // dieser Regelschleifenzaeler soll noch in die Architektur
      return;     
    }

    if (! *logOut) {
      throw TribotsException("Could not open image monitor log file.");
    }
    if (singleFile) {
      if (! *imgOut) {
	throw TribotsException("Could not open image monitor output file.");
      }
      (*imgOut) << image.getTimestamp() << endl;
      imageIO->write(image, *imgOut);
      (*logOut) << counter++ << " " << image.getTimestamp() << endl;
    }
    else {
      stringstream base;
      base << filename << image.getTimestamp() << imageIO->getDefEnding();
      imageIO->write(image, base.str());
      (*logOut) << counter++ << " " << base.str() << " " 
		<< image.getTimestamp() << endl;
    }
  }
}
