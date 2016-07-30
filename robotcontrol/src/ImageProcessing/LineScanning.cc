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

#include "LineScanning.h"
#include "Painter.h"

namespace Tribots {

  using namespace std;

  LineScanner::LineScanner(ScanLines* lines, 
			   const ColorClassInfoList* colClasses)
    : lines(lines), colClasses(colClasses), vis(0)
  {}

  LineScanner::~LineScanner()
  {
    delete lines;
  }

  void 
  LineScanner::scan(const Image& image, ScanResultList* results) 
    throw (TribotsException)
  {
    if (results->results.size() != colClasses->classList.size()) {
      throw TribotsException(__FILE__ 
			     ": Size of given ScanResultList does not match "
			     "the number of classes defined in the "
			     "ColorClassInfoList.");
    }

    for (unsigned int s=0;            // do for every scan line
	 s < lines->scanLines.size(); 
	 s++) {
      
      const LineSegment& line = lines->scanLines[s];  // current scanline
      Vec step   = (line.getEnd() - line.getStart()); // direction vector
      double totalLength = step.length();
      step = step.normalize();        // add this vector each step
      if (fabs(step.x) > fabs(step.y)) {
	step = step / fabs(step.x);
      }
      else {
	step = step / fabs(step.y);         // 
      }
      int numSteps= static_cast<int>(totalLength / step.length());  // number of steps

      std::vector<bool> seenFirstTrans(colClasses->classList.size());
      
      for (unsigned int i=0; i<colClasses->classList.size(); i++) 
	seenFirstTrans[i] = false;

      Vec currPos = line.getStart();
      int currClass = COLOR_IGNORE;       

      Vec oldPos   = currPos - step;  // start one step before scan line
      int oldClass = COLOR_IGNORE;    // with default color class

      bool first   = true;

      // \todo: Mit diesem Verfahren kann wegen der Rundung auf ganze Zahlen
      //        ein Pixel 2* Mal untersucht werden (Ein Pixel ist in der 
      //        diagonalen wesentlich l�nger als 1.0).

      // Koordinaten runden oder abschneiden, um Pixel zu erhalten? 
      // Beinhaltet Pixel (0,0) den Bereich [-0.5, +0.5) oder den Bereich
      // [0, 1) ?

      for (int i=0; i <= numSteps; i++) {

	currClass = image.getPixelClass(static_cast<int>(currPos.x),
					static_cast<int>(currPos.y));	
	
	// add pixel to the result list?
	if (colClasses->classList[currClass]->findPoints) {
	  results->results[currClass]->points.push_back(currPos);
	}

	// generate transitions? transition structs are stored in each
	// participating class's result vector.
	if (currClass != oldClass) {      // transition detected

	  if (colClasses->classList[oldClass]->findTransitions &&
	      !(seenFirstTrans[oldClass] && 
		colClasses->classList[oldClass]->findOnlyFirst)
	      ) { // store end
	    Vec twoPos = currPos + step;
	    results->results[oldClass]->transitions.push_back(
	      Transition(Transition::END,
			 oldClass, currClass, 
			 image.getPixelClass(static_cast<int>(twoPos.x),
					     static_cast<int>(twoPos.y)),
			 oldPos, currPos, first)); // first is a virtual trans
	    seenFirstTrans[oldClass]=true;
	    
	  }
	  if (colClasses->classList[currClass]->findTransitions &&
	      !(seenFirstTrans[currClass] && 
		colClasses->classList[currClass]->findOnlyFirst)) {//store start
	    Vec twoPos = oldPos - step;

	    results->results[currClass]->transitions.push_back(
	      Transition(Transition::START,
			 oldClass, currClass,
			 image.getPixelClass(static_cast<int>(twoPos.x),
					     static_cast<int>(twoPos.y)),
			 oldPos, currPos, first)); // first is a virtual trans
	  }
	  first = false;
	}	      
	oldClass = currClass;           // store current class and
	oldPos   = currPos;             // position and
	currPos+=step;                  // step one pixel forward  
      }
      // need to store last end transition 
      // (oldPos=line.end() nach currPos=line.end()+step)
      if (colClasses->classList[currClass]->findTransitions &&
	      !(seenFirstTrans[currClass] && 
		colClasses->classList[currClass]->findOnlyFirst)) {//store end
	results->results[currClass]->transitions.push_back(
	  Transition(Transition::END,
		     oldClass, COLOR_IGNORE, COLOR_IGNORE,
		     oldPos, currPos, true));  // last is a virtual transition
      }      
    }    
    if (vis) {
      visualize(results);
      vis = 0;
    }
  }

  void LineScanner::visualize(const ScanResultList* results)
  {
    Painter p(*vis);
    for (unsigned col=0; col < colClasses->classList.size(); col++) {
      p.setColor(colClasses->classList[col]->color);
      ScanResult* result = results->results[col];
      for (unsigned t=0; t < result->transitions.size(); t+=2) {
       	Vec start = result->transitions[t].toPos;
	Vec end = result->transitions[t+1].fromPos;
	p.drawLine((int)start.x, (int)start.y, (int)end.x, (int)end.y);
      }
      for (unsigned i=0; i < result->points.size(); i++) {
	Vec point = result->points[i];
	p.drawPoint((int)point.x, (int)point.y);
      }
    }
  }

  void
  ScanResult::clear() 
  {
    transitions.clear();
    points.clear();
  }

  ScanResultList::ScanResultList(int n)
  {
    for (int i=0; i < n; i++) {
      results.push_back(new ScanResult(i));
    }
  }

  ScanResultList::~ScanResultList()
  {
    for (unsigned int i=0; i < results.size(); i++) {
      delete results[i];
    }
    results.clear();
  }
  
  void
  ScanResultList::clear()
  {
    for (unsigned int i=0; i < results.size(); i++) {
      results[i]->clear();
    }
  }
}
