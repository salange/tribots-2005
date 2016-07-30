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


#include "MonoLED.h"
#include <qpainter.h>

using namespace std;
using namespace TribotsTools;

MonoLED::MonoLED ( QWidget* p, const char* c, WFlags f, QColor color) : QWidget (p,c,f), col(color), on (false) {;}

MonoLED::~MonoLED () {;}

const QColor& MonoLED::color () const throw () {
  return col;
}

bool MonoLED::isOn () const throw () {
  return on;
}

void MonoLED::setColor (QColor c) throw () {
  col=c;
}

void MonoLED::setOn (bool b) {
  on=b;
  update ();
}

void MonoLED::paintEvent(QPaintEvent *) {
  QPainter paint (this);
  int w = width();
  int h = height();
  int r = ((h>w ? w : h )-5)/2;
  QWMatrix mapping (1,0,0,1,w/2, h/2);
  paint.setWorldMatrix (mapping);

  paint.setPen (QPen (Qt::black,0));
  if (on)
    paint.setBrush (QBrush (col, Qt::SolidPattern));
  else
    paint.setBrush (QBrush (col, Qt::Dense5Pattern));
  paint.drawChord (-r,-r,2*r,2*r,0,5760);
  paint.setPen (QPen (Qt::black,1));
  paint.drawArc (-r,-r,2*r,2*r,0,5760);
}
