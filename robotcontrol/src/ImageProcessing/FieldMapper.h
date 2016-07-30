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

#ifndef _fieldmapper_h_
#define _fieldmapper_h_

#include "../Structures/TribotsException.h"
#include "../Fundamental/Time.h"
#include "../Fundamental/Frame2D.h"

#include <vector>


namespace Tribots {

  class ScanResult;
  class Transition;
  class Image;

  class FieldMapper {
  public:
    FieldMapper(int x1=0,int x2=640,int y1=0,int y2=480,int w=64,int h=48);
    ~FieldMapper() {;};
 
    bool insideField(int x, int y) const;
    void buildFieldMap(const Image& image);
  void growRegions();
  protected:
    int colClass;
    int x1,x2;
    int y1,y2;
    int w,h;
    bool imagemap[64][48];
    int xstep;
    int ystep;
      
    };

 }
#endif
