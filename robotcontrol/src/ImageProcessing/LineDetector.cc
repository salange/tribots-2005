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

#include "LineDetector.h"
#include "LineScanning.h"
#include "ColorClasses.h"
#include "../WorldModel/WorldModel.h"
#include "../Player/WhiteBoard.h"
#include "FieldMapper.h"
#include "Image.h"
#include "Painter.h"


namespace Tribots {

  LineDetector::LineDetector(const CoordinateMapping* mapping, 
			     FieldMapper * fieldmapper,
			     double minLineWidth,
			     double maxLineWidth,
			     bool use2Steps,
			     bool checkForGreen,
			     bool cutOutsideField,
			     bool useMaximalDistance,
			     double maximalDistance,
			     bool useHalfField,
			     bool useFieldMapper) 
    : mapping(mapping),fieldmapper(fieldmapper), minLineWidth(minLineWidth), maxLineWidth(maxLineWidth),
      use2Steps(use2Steps), checkForGreen(checkForGreen), 
      cutOutsideField(cutOutsideField), useHalfField(useHalfField),
      useMaximalDistance(useMaximalDistance), maximalDistance(maximalDistance),useFieldMapper(useFieldMapper), vis(0)
  {
  }
  
  void
  LineDetector::searchLines(const ScanResult* scanResult, Time time, 
			    VisibleObjectList* vol, bool writeWM)
    throw (TribotsException)
  {
    if (scanResult->id != COLOR_LINE) {
      throw TribotsException(__FILE__ 
			     ": Expected results of color class COLOR_LINE "
			     "but received results of another class.");
    }
    
    for (unsigned int i=0; i < scanResult->transitions.size(); i+=2) {
      if (scanResult->transitions.size() <= i+1) { // a pair left?
	throw TribotsException(__FILE__
			       ": The transition list is not organized "
			       "pair-wise. One end transision is missng.");
      }
	
      const Transition& transStart = scanResult->transitions[i];
      const Transition& transEnd   = scanResult->transitions[i+1];

      if (transStart.type != Transition::START ||
	  transEnd.type   != Transition::END) {
	throw TribotsException(__FILE__
			       ": The transition list is not organized "
			       "pair-wise. Each start transtition has to be "
			       "followed by exactly one endtransision!");
      }

      // plausibility check

      if (checkLinePlausibility(transStart, transEnd, 
				time)) { // add to world model?
	Vec middle = (mapping->map(transStart.toPos) +
		      mapping->map(transEnd.fromPos));
	middle *= 0.5;  // Mittelpunkt auf der Linie zwischen den beiden 
	                // Übergängen
	if (vis) {
	  Vec imgMiddle = (transStart.toPos + transEnd.fromPos) * .5;
	  visualize(imgMiddle);
	}
	
	if (vol) {      // VisibleObjectList available?
	  vol->objectlist.push_back(
	    VisibleObject (middle, VisibleObject::white_line)
	  );
	}

	if (writeWM) {  // add to world model only, if writeWM set
	  WorldModel::get_main_world_model().set_visual_information(
	    VisibleObject (middle, VisibleObject::white_line), time 
	  );     
	}
      }
    }
    if (vis) vis = 0;  // visualization finished
  }

  bool 
  LineDetector::checkLinePlausibility(const Transition& transStart,
				      const Transition& transEnd,
				      const Time& time) const
  {
    if (checkForGreen) {  // check if FIELD before and after line
      if (use2Steps) {
	if (transStart.twoStep != COLOR_FIELD ||
	    transEnd.twoStep != COLOR_FIELD) {
	  return false;
	}
      }
      else if (transStart.from != COLOR_FIELD ||
	       transEnd.to != COLOR_FIELD) { 
	return false;
      }
    }
    else {  // check for same color class
      if (use2Steps) {
	if (transStart.twoStep != transEnd.twoStep) {
	  return false;
	}
      }
      else if (transStart.from != transEnd.to) { 
	// same color before and after line
	return false;
      }
    }
    if (useMaximalDistance) {
      Frame2d robot2world = 
	WhiteBoard::getTheWhiteBoard()->getRel2AbsFrame(time);
      Vec globalCoord = robot2world * mapping->map(transStart.toPos);

      if (globalCoord.length()  > maximalDistance) {
	return false;
      }
    }

    if (cutOutsideField) {
      const FieldGeometry& fg = MWM.get_field_geometry ();
      Frame2d robot2world = 
	WhiteBoard::getTheWhiteBoard()->getRel2AbsFrame(time);
      Vec globalCoord = robot2world * mapping->map(transStart.toPos);
      
      double maxX = 0.5 * fg.field_width + 300;
      double maxY = 0.5 * fg.field_length+ 300;

      if (fabs(globalCoord.x) > maxX || fabs(globalCoord.y) > maxY) {
	return false;
      }
    }      

    if (useHalfField) {  // nur halbes Feld vorhanden (Trainingsraum)
      Frame2d robot2world = 
	WhiteBoard::getTheWhiteBoard()->getRel2AbsFrame(time);
      Vec globalCoord = robot2world * mapping->map(transStart.toPos);
      
      globalCoord *= MWM.get_own_half();

      if (globalCoord.y > 300) {// ab 30cm in blauer Hälfte abschneiden
	return false;
      }
    }
    
    if (useFieldMapper) {  // nur halbes Feld vorhanden (Trainingsraum)
      if (! fieldmapper->insideField((int)transStart.fromPos.x,
				     (int)transStart.fromPos.y))
      //std::cout<<"FieldMapper Check For "<<transStart.fromPos <<"\n";
      return false;
      }
    
      

    double width = ( mapping->map(transStart.toPos) - 
		     mapping->map(transEnd.fromPos) ).length();

    if (width < minLineWidth || // check distance between the two transitions
	width > maxLineWidth) {
      return false;
    }

    return true;                // passed all tests
  }

  void LineDetector::visualize(const Vec& line) const
  {
    Painter p(*vis);
    p.setColor(60, 120, 240);
    p.markRect((int)line.x, (int)line.y, 2);
  }
};
