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

#ifndef _filemonitor_h_
#define _filemonitor_h_

#include "../ImageMonitor.h"
#include "../ImageIO.h"
#include "../Image.h"
#include <string>
#include <fstream>

namespace Tribots {

  class FileMonitor : public ImageMonitor {
  public:
    FileMonitor(ImageIO* imageIO, const std::string& filename, 
		int step=1, bool singleFile=true);
    virtual ~FileMonitor();
    virtual void monitor(const Image& image, 
			 const Time& available, const Time& created);

  protected:
    ImageIO* imageIO;
    std::ofstream* logOut;
    std::ofstream* imgOut;
    std::string filename;
    bool singleFile;

    int counter;
    int step;
  };

}

#endif
