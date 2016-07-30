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

#include "ChainCoding.h"
#include "Image.h"

#include <stdlib.h>
#include <iostream>

namespace Tribots {
  
  using namespace std;

  int ChainCoder::xDir[4] = {  1,  0, -1,  0 };
  int ChainCoder::yDir[4] = {  0,  1,  0, -1 };  

  int 
  ChainCoder::traceBorder(const Image& image, int x, int y, Region* region,
			  char* borderMap)
  {
    int dir;                      // Richtung des letzten Schritts
    int colClass =
      image.getPixelClass(x,y);   // Farbe der abzuschreitenden Region

    int width  = image.getWidth();

    *region = Region(colClass, x, y);

    for (dir=0; dir < 4; dir++) { // check, dass kein einzelnes Pixel
      if (image.getPixelClass(x+xDir[dir], y+yDir[dir]) == colClass) {
	break;
      }
    }
    if (dir == 4) {               // es ist ein einzelnes Pixel!
      if (vis) vis = 0;
      return 1;
    }

    // Startpunkt bestimmen: es muss sichergestellt werden, dass die Region
    // immer gegen den Uhrzeigersinn abgeschritten wird, egal an welcher 
    // Stelle sie von der Scanlinie getroffen wurde.

    do {
      for (dir=0; dir < 4; dir++) {// suche ein andersfarbiges Pixel
        if (image.getPixelClass(x+xDir[dir], y+yDir[dir]) != colClass) {
	  break;
        }
      }

      if (dir == 4) {             // Punkt ist kein Randpunkt!
        while (image.getPixelClass(x-1, y) == colClass) {
          x--;
	}
      }
    } while (dir == 4);

    region->x = x;

    if (borderMap && borderMap[x+y*width] > 0) {
      if (vis) vis = 0;
      return 0;                   // dieser Rand wurde schon einmal bearbeitet
    }
    
    dir++;                        // in der schleife wird einen zurückgedreht

    // gegen den Uhrzeigersinn nach nächstem Pixel suchen (einen Schritt
    // vor der letzten Schrittrichtung beginnen. Also bei Schritt nach unten ->
    // links mit der Suche anfangen), gegen den Uhrzeigersinn abschreiten, 
    // bis am Ausgangspunkt (in richtiger Richtung) angekommen. 
    // Test: Aktuelle Position auf zweitem Punkt der Kette, vorige Position
    // (-dir) ist Ausgangspunkt region->x, region->y

    do {
      int sDir;
      for (sDir = (dir+3) % 4; ; sDir = (++sDir) % 4) {
	if (image.getPixelClass(x+xDir[sDir], y+yDir[sDir]) == colClass) {
	  break;                  // nächstes Pixel auf Rand gefunden
	}
      }
      x+=xDir[sDir];
      y+=yDir[sDir];
      dir = sDir;

      if (borderMap) {
	borderMap[x+y*width] = 1; // Punkt markieren
      }

      region->chainCode.push_back(dir);

      region->minX = min(region->minX, x);
      region->maxX = max(region->maxX, x);
      region->minY = min(region->minY, y);
      region->maxY = max(region->maxY, y);
    } while (region->chainCode.size() < 2 ||  // Kann noch nicht rum sein
	     x != region->x+xDir[static_cast<int>(region->chainCode[0])] ||
	     y != region->y+yDir[static_cast<int>(region->chainCode[0])] ||
	     x-xDir[dir] != region->x ||
	     y-yDir[dir] != region->y);
    
    region->chainCode.pop_back(); // letzter Schritt war mit erstem identisch

    if (vis) {
      visualizeRegion(region);
      vis = 0;
    }

    return 1;
  }

  void
  ChainCoder::visualizeRegion(const Region* region)
  {}

  int
  Region::getArea() const
  {
    if (area < 0) {               // have to calculate it first
      int a = 0;

      int vertPos=0;
      int horiPos=0;

      int cX = 0;
      int cY = 0;      
   
      // walk through the chaincode and integrate (see Horn for a desc.)
      for (unsigned int i=0; i < chainCode.size(); i++) {
	switch(chainCode[i]) {
	case 0: a-=vertPos; cX-=vertPos*horiPos; break;
	case 1: cY+=vertPos*horiPos; break;
	case 2: a+=vertPos; cX+=vertPos*horiPos; break;
	case 3: cY-=horiPos*vertPos; break;
	}
	vertPos += ChainCoder::yDir[(int)chainCode[i]];
	horiPos += ChainCoder::xDir[(int)chainCode[i]];
      }      
      const_cast<Region*>(this)->area=a;
      const_cast<Region*>(this)->centerOfGravity=
	Vec(cX/(double)a, cY/(double)a) + Vec(x,y);
    }
    return area;   
  }

  double
  Region::getCompactness() const
  {
    return ((double)chainCode.size() * chainCode.size()) / getArea();
  }
  
  Vec 
  Region::getCenterOfGravity() const
  {
    if (area < 0) {
      getArea();               // berechnet auch das Zentrum
    }
    return centerOfGravity;
  }


  RegionDetector::RegionDetector(ChainCoder* cc)
    : buf(0), size(0), cc(cc)
  {}

  RegionDetector::~RegionDetector()
  {
    if (buf) delete [] buf;
    if (cc) delete cc;
  }

  void
  RegionDetector::findRegions (const Image& image, int colClass,
			       vector<Region>* regions) const
  {
    int w = image.getWidth();
    int h = image.getHeight();

    if (size != w*h) {         // puffer neu anlegen
      if (buf) {
	delete [] const_cast<RegionDetector*>(this)->buf;
      }
      const_cast<RegionDetector*>(this)->buf = new char[w*h];
    }
    else {
      memset(buf, 0, sizeof(char)); // puffer leeren
    }

    int actClass = 0;          // aktuelle Farbklasse
    Region region;

    for (int x=1; x < w-1; x++) {
      for (int y=1; y < h-1; y++) {
	int newClass = image.getPixelClass(x,y);
	if (actClass != colClass &&      // nicht in farbe gewesen
	    newClass == colClass &&      // jetzt aber in farbe eingetreten
	    buf[x+y*w] == 0) {           // und dieser rand noch nicht verfolgt
	  if (cc->traceBorder(image, x,y, &region, buf)) { // verfolge
	    if (region.getArea() >= 0) {  // nur merken, wenn area größer 0,
	      regions->push_back(region); // denn bei area<0 handelt es sich
	    }                             // um ein loch!
	  }
	}
	actClass = newClass;
      }
    }    
  }

};

