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

#include "valplotwidget.h"

ValPlotWidget::ValPlotWidget(QWidget* parent, const char * name, WFlags f) 
  : QFrame(parent, name, f)
{
  max_val = 1;
  min_val = 0;
  length = 10;
}


void ValPlotWidget::init_widget(float _max_val, float _length)
{
  max_val = _max_val;
  length  = _length;
  min_val = 0;
}

void ValPlotWidget::init_widget(float _min_val, float _max_val, float _length)
{
  max_val = _max_val;
  min_val = _min_val;
  length  = _length;
}

void ValPlotWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setPen(QPen(QColor(0,0,0), 1));
  float range = max_val - min_val;
  p.scale(this->width()/length, (this->height()-1)/100.0) ;
  
  p.setPen(QPen(QColor(0,255,0), 1));
  p.drawText(0,100,QString::number(min_val));
  p.drawLine(0,
	     100,
	     values.size(),
	     100);
  p.setPen(QPen(QColor(255,0,0), 1));
  p.drawText(0,10,QString::number(max_val));
  p.drawLine(0,
	     0,
	     values.size(),
	     0);
  p.setPen(QPen(QColor(0,0,0), 1));
 

  if (min_val <= 0 && max_val >= 0) {
    p.setPen(QPen(QColor(0,255,0), 1));
    p.drawLine(0,
	       (int) (100.0 - (( (- min_val) / range) * 100.0 )),
	       values.size(),
	       (int) (100.0 - (( (- min_val) / range) * 100.0 )));
    p.setPen(QPen(QColor(0,0,0), 1));
  }
  
    
  for (int i=0; i<((int) values.size()); i++)
    {
      //int scalval=(int) (100.0 - ((values[i] / max_val) * 100.0));
      int scalval = (int) (100.0 - (( (values[i] - min_val) / range) * 100.0 ));
      p.drawPoint(i,scalval);
    }
  
}

void  ValPlotWidget::push(float val)
{
  if (values.size() >= length)
    values.erase(values.begin());
  values.push_back(val);
}
