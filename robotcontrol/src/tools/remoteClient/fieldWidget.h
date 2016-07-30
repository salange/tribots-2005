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

#ifndef _YUV422VIEWER_H_
#define _YUV422VIEWER_H_

#include <qframe.h>
#include <qpainter.h>

#include "../Structures/FieldGeometry.h"
#include "../Structures/BallLocation.h"
#include "../Structures/RobotProperties.h"
#include "../Structures/RobotLocation.h"
#include "../Fundamental/Angle.h"

class FieldWidget : public QFrame
{
Q_OBJECT

 protected:
  Tribots::FieldGeometry   * fieldGeometry;
  Tribots::BallLocation    * ballLocation;
  int                      * ownHalf;
  Tribots::RobotLocation   * robotLocation;
  Tribots::RobotProperties * robotProperties;

  virtual void paintEvent(QPaintEvent *ev);

  int slHintMode;
  float hintx, hinty;
  float hintx2, hinty2;
    
  virtual void mousePressEvent ( QMouseEvent * );
  virtual void mouseMoveEvent ( QMouseEvent * );

  void image2world(int image_x, int image_y, float &world_x, float &world_y);
  void world2image(float world_x, float world_y, int &image_x, int &image_y);

  double xoffset, yoffset, xscale, yscale;

 public:
  FieldWidget(QWidget* parent=0,const char * name=0, WFlags f=0);
  
  void init_widget(Tribots::FieldGeometry* _fieldGeometry,
		   int * _ownHalf,
		   Tribots::BallLocation* _ballLocation,
		   Tribots::RobotLocation* _robotLocation,
		   Tribots::RobotProperties* _robotProperties); 
  
  public slots:
    void setSlHintMode();

  signals:
  void SigSetSLHint3(float x_mm, float y_mm, float heading_rad);
  void SigSetSLHint2(float x_mm, float y_mm);

};

#endif
