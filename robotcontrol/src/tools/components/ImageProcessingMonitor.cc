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

#include "ImageProcessingMonitor.h"
#include <qpainter.h>

namespace Tribots {

  ImageProcessingMonitor::ImageProcessingMonitor(const ConfigReader& config,
						 const Image& image, 
						 QWidget* parent,
						 WFlags f) 
    : CImageWidget(image, parent, f), scanLines(0), scanResults(0)
  {
    colClass = new ColorClassInfoList(config);
  };

  ImageProcessingMonitor::~ImageProcessingMonitor()
  {
    delete colClass;
  }

  void 
  ImageProcessingMonitor::setScanLines(const ScanLines* scanLines)
  {
    this->scanLines = scanLines;    
  }

  void 
  ImageProcessingMonitor::setScanResults(const ScanResultList* scanResults)
  {
    this->scanResults = scanResults;    
  }

  void
  ImageProcessingMonitor::paintEvent(QPaintEvent *ev)
  {
    QPainter p(this);
    p.drawPixmap(0,0,image);
   
    if (scanLines != 0) {
      p.setPen(Qt::white);
      
      for (unsigned int i=0; i < scanLines->scanLines.size(); i++) {
	Vec start = scanLines->scanLines[i].getStart();
	Vec end   = scanLines->scanLines[i].getEnd();

	p.drawLine(QPoint((int)start.x, (int)start.y), 
		   QPoint((int)end.x,   (int)end.y));
      }
    }
    if (scanResults != 0) {
      for (unsigned int c=0; c < colClass->classList.size(); c++) {

	// draw transitions
	for (unsigned int i=0; 
	     i < scanResults->results[c]->transitions.size();
	     i++) {
	  const Transition& trans = scanResults->results[c]->transitions[i];
	  if (trans.type == Transition::START) { // Start: hell, end: dunkel
	    p.setPen(QColor(colClass->classList[c]->color.r,
			    colClass->classList[c]->color.g,
			    colClass->classList[c]->color.b));
	    p.drawEllipse((int)trans.toPos.x-2, (int)trans.toPos.y-2,
			  4,4);
	  }
	  else {
	    p.setPen(QColor(colClass->classList[c]->color.r / 2,
			    colClass->classList[c]->color.g / 2,
			    colClass->classList[c]->color.b / 2));
	    p.drawEllipse((int)trans.fromPos.x-2, (int)trans.fromPos.y-2,
			  4,4);
	  }
	}

	// draw points
	for (unsigned int i=0; 
	     i < scanResults->results[c]->points.size();
	     i++) {
	  
	  const Vec& point = scanResults->results[c]->points[i];
	  p.drawPoint((int)point.x, (int)point.y);
	}

      }
    }
  }
}
