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

#include "Painter.h"


namespace Tribots {

  Painter::Painter(Image& image) 
    : image(image), color(black), pen(PEN_SOLID), counter(0) 
  {}
  void Painter::setColor(const RGBTuple& color)
  {
    this->color = color;
  }
  void Painter::setColor(int r, int g, int b)
  {
    color.r = r;
    color.g = g;
    color.b = b;
  }

  void Painter::setPen(int type)
  {
    pen = type;    
  }

  void Painter::usePen(int x, int y)
  {
    switch (pen) {
    case PEN_DOTTED: 
      if (counter++ % 2 == 0) { 
	image.setPixelRGB(x,y,color); 
      } 
      break;
    case PEN_STEPPED:
      if (counter++ % 7 < 4) {
	image.setPixelRGB(x,y,color);
      }
      break;
    case PEN_SOLID: 
    default: 
      image.setPixelRGB(x,y,color); break;
    }      
  }

  void Painter::drawRect(int x, int y, int width, int height)
  {
    drawLine(x,y, x+width, y);
    drawLine(x+width, y, x+width, y+height);
    drawLine(x+width, y+height, x, y+height);
    drawLine(x, y+height, x, y);
  }

  void Painter::markRect(int x, int y, int size)
  {
    drawRect(x-size, y-size, size*2, size*2);
  }

  void Painter::markCrosshair(int x, int y, int size)
  {
    drawLine(x, y-size, x, y+size);
    drawLine(x-size, y, x+size, y);
  }

  void Painter::markCrossbar(int x, int y, int size)
  {
    drawLine(x-size, y-size, x+size, y+size);
    drawLine(x+size, y-size, x-size, y+size);
  }

  void Painter::drawPoint(int x, int y)
  {
    image.setPixelRGB(x, y, color);
  }

  // Implementierung des Bresenham-Algorithmus mit Integern
  void Painter::drawLine(int x1, int y1, int x2, int y2)
  {
    int x=x1,y=y1, error, delta, schwelle, dx, dy, inc_x, inc_y;
    int width = image.getWidth();
    int height= image.getHeight();

    dx = x2-x;
    dy = y2-y;

    if (dx>0) inc_x=1; else inc_x=-1;
    if (dy>0) inc_y=1; else inc_y=-1;
    
    if (abs(dy) < abs(dx)) {
      error = -abs(dx);
      delta = 2* abs(dy);
      schwelle = 2*error;
      while (x != x2) {
	if (y<0 || x<0 || y>=height || x>=width)
	  ;             // don't draw pixels, that aren't in the visual's area
	else 
	  usePen(x,y);
	x+=inc_x;
	error+=delta;
	if (error>0) { y+=inc_y; error+=schwelle; }
      }
    }
    else {
      error = -abs(dy);
      delta = 2* abs(dx);
      schwelle = 2*error;
      while (y != y2) {
	if (y<0 || x<0 || y>=height || x>=width)
	  ;
	else
	  usePen(x,y);
	y+=inc_y;
	error+=delta;
	if (error>0) { x+=inc_x; error+=schwelle; }
      }
    }

    if (y<0 || x<0 || y>=height || x>=width)
      return;
    
    usePen(x,y);
  }

  const RGBTuple Painter::black = {0,0,0};
  const RGBTuple Painter::white = {255,255,255};


}
