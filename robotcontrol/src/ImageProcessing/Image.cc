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

#include "Image.h"

#include "ImageSource.h"
#include "ColorClassifier.h"

namespace Tribots {
  Image::Image(const ColorClassifier* classifier)
    : classifier(classifier), timestamp(Time())
  {
    if (classifier == 0) {
      this->classifier = DefaultClassifier::getInstance();
    }
  }

  int Image::getWidth() const { return buffer.width; }
  int Image::getHeight() const { return buffer.height; }

  int Image::getNativeFormat() const { return buffer.format; }

  const ColorClassifier*
  Image::getClassifier() const
  {
    return classifier;
  }
  
  void 
  Image::setClassifier(const ColorClassifier* classifier)
  {
    if (classifier != 0) {
      this->classifier = classifier;
    }
    else {
      this->classifier = DefaultClassifier::getInstance();
    }
  }

  void Image::setBlackBorder()
  {
    RGBTuple black = { 0, 0, 0 };
    setBorder(black);
  }

  void Image::setWhiteBorder()
  {
    RGBTuple white = { 255, 255, 255 };
    setBorder(white);
  }

  void Image::setBorder(const RGBTuple& rgb) 
  {
    int w = getWidth()-1;
    int h = getHeight()-1;

    for (int x=w; x >= 0; x--) {
      setPixelRGB(x, 0, rgb);
      setPixelRGB(x, h, rgb);
    }
    for (int y=h; y >= 0; y--) {
      setPixelRGB(0, y, rgb);
      setPixelRGB(w, y, rgb);
    }
  }      
    
}
