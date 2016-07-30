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

#ifndef _VALPLOTWIDGET_H_
#define _VALPLOTWIDGET_H_

#include <qframe.h>
#include <qpainter.h>
#include <vector>

class ValPlotWidget : public QFrame
{
Q_OBJECT

 protected:
  virtual void paintEvent(QPaintEvent *ev);
  float min_val, max_val, length;
  std::vector<float> values;
  
    
 public:
  ValPlotWidget(QWidget* parent=0,const char * name=0, WFlags f=0);
  
  void init_widget(float _max_val, float _length);
  void init_widget(float min_val, float _max_val, float _length);
  void push(float val);
};

#endif
