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

#include "ColorClassifier.h"

namespace Tribots {

  ColorClassifier* 
  ColorClassifier::createFromFile(string filename) const
  {
    ColorClassifier* cc = create();
    cc->load(filename);
    return cc;
  }

  DefaultClassifier*
  DefaultClassifier::singleton = 0;

  const unsigned char& 
  DefaultClassifier::lookup(const RGBTuple&)  const
  { return defValue; }

  const unsigned char& 
  DefaultClassifier::lookup(const YUVTuple&)  const
  { return defValue; }

  const unsigned char& 
  DefaultClassifier::lookup(const UYVYTuple&, int) const
  { return defValue; }

  void 
  DefaultClassifier::set(const RGBTuple&,  unsigned char)
  { return; }

  void 
  DefaultClassifier::set(const YUVTuple&,  unsigned char)
  { return; }

  void 
  DefaultClassifier::set(const UYVYTuple&, unsigned char, int)
  { return; }

  void 
  DefaultClassifier::load(string)
  { return; }
  
  void 
  DefaultClassifier::save(string) const
  { return; }

  ColorClassifier* 
  DefaultClassifier::create() const
  { 
    throw TribotsException(__FILE__ ": Don't call create() on the default " 
			   "DefaultClassifier! You do not want to copy this.");
  }

  const DefaultClassifier* 
  DefaultClassifier::getInstance()
  {
    if (singleton==0) {
      singleton = new DefaultClassifier();
    }
    return singleton;
  }


  DefaultClassifier::DefaultClassifier()
    : ColorClassifier(), defValue(0)
  {}

  WhiteClassifier::WhiteClassifier()
    : DefaultClassifier(), white(1), black(0)
  {}

  const unsigned char&
  WhiteClassifier::lookup(const RGBTuple& rgb) const
  {
    return rgb.r > 127 && rgb.g > 127 && rgb.b > 127 ? white : black;
  }

  const unsigned char&
  WhiteClassifier::lookup(const YUVTuple& yuv) const
  {
    return yuv.y > 127 ? white : black;
  }

  const unsigned char&
  WhiteClassifier::lookup(const UYVYTuple& uyvy, int pos) const
  {
    return ((pos % 2 == 0) ? uyvy.y1 > 127 : uyvy.y2 > 127) ? white : black;
  }

  BlackClassifier::BlackClassifier()
    : WhiteClassifier()
  {
    white = 0;
    black = 1;
  }


}
