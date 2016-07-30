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

#ifndef _PAINTER_H_
#define _PAINTER_H_

#include "Image.h"

namespace Tribots {

  class Painter {
  public:
    
    static const RGBTuple black;
    static const RGBTuple white;

    enum { PEN_SOLID=0, PEN_DOTTED, PEN_STEPPED };

    Painter(Image& image);

    void setColor(const RGBTuple& color);
    void setColor(int r, int g, int b);

    void setPen(int type);

    void drawLine(int x1, int y1, int x2, int y2);
    void drawPoint(int x, int y);
    void drawRect(int x, int y, int width, int height);

    void markCrosshair(int x, int y, int size);   // horizontal, vertical
    void markCrossbar(int x, int y, int size);    // diagonal
    void markRect(int x, int y, int size);
  
  protected:
    Image& image;
    RGBTuple color;
    int pen;
    int counter;

    void usePen(int x, int y);
  };

}

#endif
