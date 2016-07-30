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

#include "fieldWidget.h"
#include <iostream>
#include <sys/time.h>
#include <qcursor.h>

FieldWidget::FieldWidget(QWidget* parent,const char * name , WFlags f) : QFrame(parent, name, f), slHintMode(0), xoffset(0), yoffset(0), xscale(1), yscale(1)
{
  setPaletteBackgroundColor(QColor(0,255,0));
  fieldGeometry = 0;
  ballLocation  = 0;
  ownHalf       = 0;
  robotLocation = 0;
  robotProperties = 0;
}

void FieldWidget::init_widget(Tribots::FieldGeometry * _fieldGeometry,
			      int * _ownHalf,
			      Tribots::BallLocation  * _ballLocation,
			      Tribots::RobotLocation * _robotLocation,
			      Tribots::RobotProperties * _robotProperties)
{
  fieldGeometry   = _fieldGeometry;
  ballLocation    = _ballLocation; 
  ownHalf         = _ownHalf;
  robotLocation   = _robotLocation; 
  robotProperties = _robotProperties;

  this->repaint();
}


void FieldWidget::paintEvent(QPaintEvent *ev)
{
  if (slHintMode == 0)
    setCursor(QCursor(Qt::ArrowCursor));
  if (slHintMode == 1)
    setCursor(QCursor(Qt::CrossCursor));
  if (slHintMode == 2)
    setCursor(QCursor(Qt::PointingHandCursor));

  QPainter p(this);
  if (slHintMode > 0) {
    QString s;
    s=s+QString("Pos: ") + QString::number(hintx) + QString(" ") + QString::number(hinty);
    if (slHintMode > 1)
      {
	int h1x,h2x,h1y,h2y;
	world2image(hintx, hinty, h1x, h1y);
	world2image(hintx2, hinty2, h2x, h2y);
	p.drawLine(h1x, h1y, h2x, h2y);

	float dx = hintx2-hintx;
	float dy = hinty2-hinty;
	Tribots::Angle w(-std::atan2(dx,dy));
	s=s+QString("  Heading: ") +QString::number(w.get_deg());
      }
    p.drawText(20,20,s);
  }
  if (fieldGeometry->field_length == 0 || fieldGeometry->field_width == 0)
    {
      p.drawText(10,10,"FieldGeometry not given!");
    }
  else
    {
      xoffset = (fieldGeometry->field_length/2.0) + fieldGeometry->goal_band_width;
      yoffset = (fieldGeometry->field_width/2.0)  + fieldGeometry->side_band_width;

      xscale  = frameGeometry().width() / 
	(fieldGeometry->field_length + 2*fieldGeometry->goal_band_width );
      yscale  = frameGeometry().height() /
	(fieldGeometry->field_width  + 2*fieldGeometry->side_band_width);

      p.scale(xscale, yscale);

      unsigned int border_line_width_hor = 
	(unsigned int) (fieldGeometry->border_line_thickness * yscale);
      if(border_line_width_hor == 0) border_line_width_hor = 1;

      unsigned int border_line_width_vert = 
	(unsigned int) (fieldGeometry->border_line_thickness * xscale);
      if(border_line_width_vert == 0) border_line_width_vert = 1;

      
      unsigned int line_width_hor = 
	(unsigned int) (fieldGeometry->line_thickness * yscale);
      if(line_width_hor == 0) line_width_hor = 1;

      unsigned int line_width_vert = 
	(unsigned int) (fieldGeometry->line_thickness * xscale);
      if(line_width_vert == 0) line_width_vert = 1;


      //draw outer field lines horizontal
      p.setPen(QPen(QColor(255,255,255), border_line_width_hor));
      p.drawLine((int) (fieldGeometry->goal_band_width),
		 (int) (fieldGeometry->side_band_width),
		 (int) (fieldGeometry->goal_band_width + fieldGeometry->field_length),
		 (int) (fieldGeometry->side_band_width));

      p.drawLine((int) (fieldGeometry->goal_band_width),
		 (int) ((fieldGeometry->side_band_width + fieldGeometry->field_width)),
		 (int) ((fieldGeometry->goal_band_width + fieldGeometry->field_length)),
		 (int) ((fieldGeometry->side_band_width + fieldGeometry->field_width)));

      // draw outer field lines vertical
      p.setPen(QPen(QColor(255,255,255), border_line_width_vert));
      p.drawLine((int) (fieldGeometry->goal_band_width),
		 (int) (fieldGeometry->side_band_width),
		 (int) (fieldGeometry->goal_band_width),
		 (int) (fieldGeometry->side_band_width + fieldGeometry->field_width));

      p.drawLine((int) (fieldGeometry->goal_band_width + fieldGeometry->field_length),
		 (int) (fieldGeometry->side_band_width),
		 (int) (fieldGeometry->goal_band_width + fieldGeometry->field_length),
		 (int) (fieldGeometry->side_band_width + fieldGeometry->field_width));
      
      // draw middle line
      p.setPen(QPen(QColor(255,255,255), line_width_vert));
      p.drawLine((int) (xoffset),
		 (int) (fieldGeometry->side_band_width),
		 (int) (xoffset),
		 (int) (fieldGeometry->side_band_width + fieldGeometry->field_width));

      // draw kickoff circle
      if (fieldGeometry->center_circle_radius !=0)
	p.drawEllipse((int) (xoffset - fieldGeometry->center_circle_radius), 
		      (int) (yoffset - fieldGeometry->center_circle_radius),
		      (int) (2*fieldGeometry->center_circle_radius),
		      (int) (2*fieldGeometry->center_circle_radius));
    

      // draw blue goal
       p.setPen(QPen(QColor(0,0,255), 1));
       p.drawLine((int) (fieldGeometry->goal_band_width - fieldGeometry->goal_length),
		  (int) (yoffset - (fieldGeometry->goal_width/2.0)),
		  (int) (fieldGeometry->goal_band_width - fieldGeometry->goal_length),
		  (int) (yoffset + (fieldGeometry->goal_width/2.0)));
       p.drawLine((int) (fieldGeometry->goal_band_width - fieldGeometry->goal_length),
		  (int) (yoffset - (fieldGeometry->goal_width/2.0)),
		  (int) (fieldGeometry->goal_band_width),
		  (int) (yoffset - (fieldGeometry->goal_width/2.0)));
       p.drawLine((int) (fieldGeometry->goal_band_width - fieldGeometry->goal_length),
		  (int) (yoffset + (fieldGeometry->goal_width/2.0)),
		  (int) (fieldGeometry->goal_band_width),
		  (int) (yoffset + (fieldGeometry->goal_width/2.0)));

       // draw yellow goal
       p.setPen(QPen(QColor(255,255,0), 1));
       p.drawLine((int) (xoffset + (fieldGeometry->field_length/2.0) + fieldGeometry->goal_length),
		  (int) (yoffset - (fieldGeometry->goal_width/2.0)),
		  (int) (xoffset + (fieldGeometry->field_length/2.0) + fieldGeometry->goal_length),
		  (int) (yoffset + (fieldGeometry->goal_width/2.0)));
       p.drawLine((int) (xoffset + (fieldGeometry->field_length/2.0) + fieldGeometry->goal_length),
		  (int) (yoffset - (fieldGeometry->goal_width/2.0)),
		  (int) (xoffset + (fieldGeometry->field_length/2.0)),
		  (int) (yoffset - (fieldGeometry->goal_width/2.0)));
       p.drawLine((int) (xoffset + (fieldGeometry->field_length/2.0) + fieldGeometry->goal_length),
		  (int) (yoffset + (fieldGeometry->goal_width/2.0)),
		  (int) (xoffset + (fieldGeometry->field_length/2.0)),
		  (int) (yoffset + (fieldGeometry->goal_width/2.0)));


       // draw Ball
       p.setPen(QPen(QColor(255,0,0), 1));
       p.drawEllipse((int) ((xoffset-ballLocation->pos.y * *ownHalf)-(fieldGeometry->ball_diameter/2.0)),
		     (int) ((yoffset-ballLocation->pos.x * *ownHalf)-(fieldGeometry->ball_diameter/2.0)),
		     (int) (fieldGeometry->ball_diameter),
		     (int) (fieldGeometry->ball_diameter));

       // draw Ball vel
       p.drawLine((int) (xoffset-ballLocation->pos.y * *ownHalf),
		  (int) (yoffset-ballLocation->pos.x * *ownHalf),
		  (int) (xoffset-ballLocation->pos.y * *ownHalf
			 - (ballLocation->velocity.y * *ownHalf * 300.0)),
		  (int) (yoffset-ballLocation->pos.x * *ownHalf
			 - (ballLocation->velocity.x * *ownHalf * 300.0)));

       // draw Robot
       p.setPen(QPen(QColor(0,0,0), 1));
       p.drawPoint((int) (xoffset - robotLocation->pos.y* *ownHalf), 
		   (int) (yoffset - robotLocation->pos.x * *ownHalf));
       p.drawLine((int) (xoffset - robotLocation->pos.y* *ownHalf), 
		  (int) (yoffset - robotLocation->pos.x * *ownHalf),
		  (int) ((xoffset - robotLocation->pos.y* *ownHalf) 
			 - sin(robotLocation->heading.get_rad() + (M_PI / 2.0))* 250  * *ownHalf),
		  (int) ((yoffset - robotLocation->pos.x * *ownHalf)
			 - cos(robotLocation->heading.get_rad() + (M_PI / 2.0))* 250 * *ownHalf));
       
        p.setPen(QPen(QColor(0,0,255), 1));
	p.drawLine((int) (xoffset - robotLocation->pos.y* *ownHalf), 
		   (int) (yoffset - robotLocation->pos.x * *ownHalf),
		   (int) ((xoffset - robotLocation->pos.y* *ownHalf) 
			  - (robotLocation->vtrans.y  *300 * *ownHalf)),
		   (int) ((yoffset - robotLocation->pos.x * *ownHalf)
			  - (robotLocation->vtrans.x * 300 * *ownHalf)));

       // draw Robot max and min Radius
       p.setPen(QPen(QColor(0,0,0), 1));
       p.drawEllipse((int) ((xoffset-robotLocation->pos.y * *ownHalf)-(robotProperties->max_robot_radius / 2.0 )),
		     (int) ((yoffset-robotLocation->pos.x * *ownHalf)-(robotProperties->max_robot_radius / 2.0 )),
		     (int) (robotProperties->max_robot_radius),
		     (int) (robotProperties->max_robot_radius));
       p.drawEllipse((int) ((xoffset-robotLocation->pos.y * *ownHalf)-(robotProperties->min_robot_radius / 2.0 )),
		     (int) ((yoffset-robotLocation->pos.x * *ownHalf)-(robotProperties->min_robot_radius / 2.0 )),
		     (int) (robotProperties->min_robot_radius),
		     (int) (robotProperties->min_robot_radius));

       

    }
}

void FieldWidget::image2world(int image_x, int image_y, float &world_x, float &world_y)
{
  world_y = (-(image_x / xscale) + xoffset) * *ownHalf;
  world_x = (-(image_y / yscale) + yoffset) * *ownHalf;
}

void FieldWidget::world2image(float world_x, float world_y, int &image_x, int &image_y)
{
  image_x = (int) ( (xoffset - world_y * *ownHalf) * xscale);
  image_y = (int) ( (yoffset - world_x * *ownHalf) * yscale);
}

void FieldWidget::setSlHintMode()
{
  if(slHintMode>0)
    {
      setMouseTracking(false);
      slHintMode=0;
    }
  else
    {
      slHintMode=1;
      setMouseTracking(true); 
    }
}


void FieldWidget::mousePressEvent ( QMouseEvent * e)
{
  if (slHintMode==2)
    {
      image2world(e->x(), e->y(), hintx2, hinty2);
      float dx = hintx2-hintx;
      float dy = hinty2-hinty;
      Tribots::Angle w(-std::atan2(dx,dy));
      std::cout << "You set a SL HintHeading: " << w.get_deg() << "\n"; 
      emit SigSetSLHint3(hintx, hinty, (float) w.get_rad());
      setMouseTracking(false);
      slHintMode=0;
    }
  if (slHintMode==1)
    {
      image2world(e->x(), e->y(), hintx, hinty);
      std::cout << "You set a hint at: " << hintx << "  " << hinty << "\n";
      if (e->button() == Qt::RightButton)
	{
	  emit SigSetSLHint2(hintx, hinty);
	  slHintMode=0;
	  setMouseTracking(false);
	}
      else
	slHintMode++;
    }
  
}

void FieldWidget::mouseMoveEvent ( QMouseEvent * e)
{
  if (slHintMode==1)
    {
      image2world(e->x(), e->y(), hintx, hinty);
    }
  if (slHintMode==2)
    {
      image2world(e->x(), e->y(), hintx2, hinty2);
    }
} 
