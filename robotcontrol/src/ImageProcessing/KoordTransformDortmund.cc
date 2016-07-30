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

#include "KoordTransformDortmund.h"

#include <fstream>
#include <iostream>

namespace Tribots {

  using namespace std;

  Image2WorldDortmund::Image2WorldDortmund(string filename)
  {
    ifstream file(filename.c_str());
    if (!file) {
      throw TribotsException(__FILE__": Could not open distance file.");
    }

    // read image dimensions and center
    int centerX, centerY;
    file >> width >> height;
    file >> centerX >> centerY;
    middle = Vec(centerX, centerY);

    // read angle shift
    file >> shift;
    
    // read number of markers
    int numMarkers;
    file >> numMarkers;

    // read real world distances of markers (mm)
    for (int i=0; i < numMarkers; i++) {
      double distance;
      file >> distance;
      realDistances.push_back(distance);
    }
    
    // initialize imageDistances ( 360>>shift list of numMarkers entries each )
    imageDistances = vector<vector<double> >(360>>shift, 
					     vector<double>(numMarkers));

    // initialize angleCorrections
    angleCorrections = vector<double> (360>>shift);

    while (!file.eof()) {
      int angle;
      double angleCorrection;
      file >> angle >> angleCorrection;
      if (file.eof()) { // end of file (or error)
	break;
      }

      angleCorrections[angle] = angleCorrection;

      for (int i=0; i < numMarkers; i++) {
	double distance;
	file >> distance;
	if (file.eof()) {
	  throw TribotsException(__FILE__
				 ": File ended although another distance "
				 "entry was expected.");
	}
	imageDistances[angle][i] = distance;
      }
    }
    file.close();
  }

  Image2WorldDortmund::~Image2WorldDortmund()
  {
  }

  Vec
  Image2WorldDortmund::map(const Vec& vec) const
  {
    Vec to = vec-middle;
    double toLength = to.length();
    int degrees = static_cast<int>( to.angle().get_deg() ) >> shift;

    // step through the list of imageDistances and find first entry,
    // that is bigger than the distance of the given vector (look at i+1).
    // if the position is size()-2, than assume last entry (i+1) to be bigger
    // than the length of the given vector (because last entry should be 
    // infinity per definition (biggest entry, that could be queried) ).
    // the first entry is assumed to be the smaller than every query.

    for (unsigned int i = 0; i < imageDistances[degrees].size()-1; i++) {
      if (toLength < imageDistances[degrees][i+1] || // between i and i+1
	  i == imageDistances[degrees].size()-2) {   // last marker:=infinity

	// linear interpolation of length in real world
	double steigung =   // deltaY / deltaX  ; y1,y2:real, x1,x2:image
	  (realDistances[i+1] - realDistances[i]) /
	  (imageDistances[degrees][i+1] - imageDistances[degrees][i]);

	to = to.normalize();
	to *= realDistances[i] + steigung * //y=x1 + steigung * (x-x1)
	  (toLength - imageDistances[degrees][i]);

	// correct orientation
	to = to.rotate(Angle::deg_angle(angleCorrections[degrees]));

	break;
      }
    }
    // assertion: to has been transformed ( if is true for i=size()-2 )
    return to;
  }

  Image2WorldMapping*
  Image2WorldDortmund::convert() const
  {
    Image2WorldMapping* mapping = new Image2WorldMapping(width, height);

    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
	mapping->set(x,y, map(Vec(x,y)));
      }
    }
    return mapping;
  }  

  void Image2WorldDortmund::setMiddle(const Vec& middle)
  {
    this->middle = middle;
  }

  Vec Image2WorldDortmund::getMiddle() const
  {
    return middle;
  }
}
