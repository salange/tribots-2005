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

#include "YUVLookupTable.h"

#include <string>
#include <fstream>

#include "Image.h"
#include "../Structures/TribotsException.h"


namespace Tribots {

  using namespace std;

  YUVLookupTable::YUVLookupTable(int shiftY, int shiftU, int shiftV) 
    : ColorClassifier(), shiftY(shiftY), shiftU(shiftU), shiftV(shiftV),
      sizeY(256 >> shiftY), sizeU(256 >> shiftU), sizeV(256 >> shiftV),
      size(sizeY*sizeU*sizeV), lut(new unsigned char[size])
  {}

  YUVLookupTable::~YUVLookupTable()
  {
    delete [] lut;
  }
  
  const unsigned char&
  YUVLookupTable::lookup(const RGBTuple& rgb) const
  {
    PixelConversion::convert(rgb, const_cast<YUVTuple*>(&yuvTmp));
    return lookup(yuvTmp);
  }
  const unsigned char&
  YUVLookupTable::lookup(const UYVYTuple& uyvy, int pos) const
  {
    PixelConversion::convert(uyvy, const_cast<YUVTuple*>(&yuvTmp), pos);
    return lookup(yuvTmp);
  }
  const unsigned char&
  YUVLookupTable::lookup(const YUVTuple& yuv) const
  {
    return lut[((yuv.y >> shiftY) * sizeU + 
		(yuv.u >> shiftU)) * sizeV +
	       (yuv.v >> shiftV)];
  }

  void 
  YUVLookupTable::set(const RGBTuple&  rgb,  unsigned char c)
  {
    PixelConversion::convert(rgb, &yuvTmp);
    set(yuvTmp, c);
  }
  void 
  YUVLookupTable::set(const UYVYTuple& uyvy, unsigned char c, int pos)
  {
    PixelConversion::convert(uyvy, &yuvTmp, pos);
    set(yuvTmp, c);
  }
  void
  YUVLookupTable::set(const YUVTuple&  yuv,  unsigned char c)
  {
    lut[((yuv.y >> shiftY) * sizeU + 
	 (yuv.u >> shiftU)) * sizeV +
	(yuv.v >> shiftV)] = c;    
  }

  void 
  YUVLookupTable::load(string filename)
  {
    int oldSize = size;// remember the size, to detect wheter or not it changes

    ifstream lutFile (filename.c_str(), ios::binary);
    if (! lutFile) {
      throw TribotsException((string(__FILE__)+string(": Could not open file ")+filename).c_str());
    }

    lutFile >> shiftY; 
    lutFile >> shiftU;
    lutFile >> shiftV; // this one reads the '\n' that follows the shiftV value

    sizeY = 256 >> shiftY;
    sizeU = 256 >> shiftU;
    sizeV = 256 >> shiftV;

    size = sizeY * sizeU * sizeV;

    if (oldSize != size) {              // size has been changed
      delete [] lut;                    // delete old and
      lut = new unsigned char[size];    // create new lookup table 
    }

    if (! lutFile.read((char*)lut, size)) {
      throw TribotsException((string(__FILE__)+string(": Could not read lookup table from file ")+filename).c_str());
    }
    lutFile.close();
  }

  
  void 
  YUVLookupTable::save(string filename) const
  {
    ofstream lutFile (filename.c_str(), ios::binary);
    if (! lutFile) {
      throw TribotsException((string(__FILE__)+string(": Could not open file ")+filename).c_str());
    }
    lutFile << shiftY << ' ' << shiftU << ' ' << shiftV <<  '\n';

    if (! lutFile.write((char*)lut, size)) {
      throw TribotsException((string(__FILE__)+string(": Could not write lookup table to file ")+filename).c_str());
    }
    lutFile.close();
  }

  ColorClassifier* 
  YUVLookupTable::create() const
  {
    return new YUVLookupTable(shiftY, shiftU, shiftV);
  }

  void 
  YUVLookupTable::fillFromClassifier(const ColorClassifier* cc)
  {
    int classCounter[256];    // 256 classes (unsigned char)
    YUVTuple yuv;
		
    for (int yQ = 0; yQ < sizeY; yQ++) {
      for (int uQ = 0; uQ < sizeU; uQ++) {
	for (int vQ = 0; vQ < sizeV; vQ++) { // for every entry in the lut


	  // check every yuv-tuple, that falls into the present quantizized
	  // entry of the lut

	  memset(classCounter, 0, sizeof(int) * 256);

	  for (int y = yQ << shiftY; y < (yQ+1) << shiftY; y++) {
	    //	    cerr << "Y: " << y << endl;
	    for (int u = uQ << shiftU; u < (uQ+1) << shiftU; u++) {
	      for (int v = vQ << shiftV; v < (vQ+1) << shiftV; v++) {
		yuv.y = y; yuv.u = u; yuv.v = v;
		classCounter[cc->lookup(yuv)]++;
	      }
	    }
	  }
	  int maxID = 0;   // look for the most frequent class 
	  for (int i=1; i < 256; i++) {
	    if (classCounter[maxID] < classCounter[i]) { // more entries?
	      maxID = i; 
	    }
	  }
	  set(yuv, maxID); // since yuv is still _in_ the lut entry, this
                           // should do the necessary update (I hope...)
	}
      }
    }
  } 
}
