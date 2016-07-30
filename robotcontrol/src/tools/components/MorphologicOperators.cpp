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

#include "MorphologicOperators.h"

namespace Tribots {

  Dilatation::Dilatation(int size)
    : ImageOperator(), size(size)
  {
    if (size < 3 || size % 2 == 0) {
      throw TribotsException(__FILE__ ": Invalid size argument.");
    }
  }

  Dilatation::~Dilatation()
  {}
  
  Image* 
  Dilatation::operator() (const Image& image) const
  {
    Image* newImage = image.clone();
    
    RGBTuple rgb;
    RGBTuple white = { 255, 255, 255 };

    for (int x = 0; x < image.getWidth(); x++) {
      for (int y = 0; y < image.getHeight(); y++) {
	newImage->getPixelRGB(x,y, &rgb);
	if (rgb.r > 127) {         // ist das Pixel schon weiss?
	  continue;                // weisser als weiss kann's nicht werden ;)
	}

	bool set = false;

	for (int mx = x - size/2; !set && mx <= x + size/2; mx++) {
	  for (int my = y - size/2; !set && my <= y + size/2; my++) {
	    if (mx < 0 || my < 0 || 
		mx >= image.getWidth() || my >= image.getHeight()) {
	      continue;
	    }
	    image.getPixelRGB(mx,my, &rgb);
	    if (rgb.r > 127) {     // Pixel gesetzt
	      newImage->setPixelRGB(x,y, white);
	      set = true;          // Schleife verlassen
	    }
	  }
	}
      }
    }
    return newImage;
  }

  // erosion //////////////////////////////////////////////////////////////

  Erosion::Erosion(int size)
    : ImageOperator(), size(size)
  {
    if (size < 3 || size % 2 == 0) {
      throw TribotsException(__FILE__ ": Invalid size argument.");
    }
  }

  Erosion::~Erosion()
  {}
  
  Image* 
  Erosion::operator() (const Image& image) const
  {
    Image* newImage = image.clone();
    
    RGBTuple rgb;
    RGBTuple black = { 0, 0, 0 };

    for (int x = 0; x < image.getWidth(); x++) {
      for (int y = 0; y < image.getHeight(); y++) {
	newImage->getPixelRGB(x,y, &rgb);
	if (rgb.r <= 127) {        // ist das Pixel schon schwarz?
	  continue;                // schwaerzer kann's nicht werden ;)
	}

	bool set = false;

	for (int mx = x - size/2; (!set) && mx <= x + size/2; mx++) {
	  for (int my = y - size/2; (!set) && my <= y + size/2; my++) {
	    if (mx < 0 || my < 0 || 
		mx >= image.getWidth() || my >= image.getHeight()) {
	      continue;
	    }
	    image.getPixelRGB(mx,my, &rgb);
	    if (rgb.r <= 127) {    // Pixel nicht gesetzt
	      newImage->setPixelRGB(x,y, black);
	      set = true;          // Schleife verlassen
	    }
	  }
	}
      }
    }
    return newImage;
  }


}
