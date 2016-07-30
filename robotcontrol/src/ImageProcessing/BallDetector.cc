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

#include "BallDetector.h"
#include "LineScanning.h"
#include "ColorClasses.h"
#include "../WorldModel/WorldModel.h"
#include "Image.h"
#include "Painter.h"
#include "../Structures/Journal.h"
#include "ChainCoding.h"

#include <cstdlib>
#include <cmath>

namespace Tribots {
  
  using namespace std;

  ImageParticleTracker::ImageParticleTracker(int colClass,
					     int centerX, int centerY,
					     int windowWidth,
					     int windowHeight,
					     int n,
					     unsigned int minParticles)
    : colClass(colClass), center(centerX, centerY), 
      windowWidth(windowWidth),
      windowHeight(windowHeight), n(n), minParticles(minParticles),  vis(0)
  {
    const FieldGeometry& fg = MWM.get_field_geometry ();
    max_y = 0.5*fg.field_length+fg.goal_band_width;
    max_x = 0.5*fg.field_width+fg.side_band_width;
  }

  void
  ImageParticleTracker::addParticle(const Vec& p)
  {
    survivingparticles.push_back(p);
  }

  void
  ImageParticleTracker::propagate(const Image& image, 
				  const CoordinateMapping& img2robot, 
				  const Frame2d& robot2world)
  {
    // zunaechst alle Partikel rausfiltern, die ausserhalb des Feldes liegen
    particles.clear();

    for (unsigned int i=0; i < survivingparticles.size(); i++) {
      Vec pworld = robot2world * img2robot.map(survivingparticles[i]); 
      if (fabs(pworld.x) <= max_x && fabs(pworld.y) <= max_y) {      
	particles.push_back(survivingparticles[i]);
      }
    }
  
    // jetzt neue Partikel zufaellig erzeugen usw.
    survivingparticles.clear();

    middle = Vec(0,0);        // reset middle particle

    if (particles.size() > 0) {
      for (int i=0; i < n; i++) {
	Vec p = particles[rand() % particles.size()];
	p = p + Vec( (rand() % windowWidth) - windowWidth / 2,
		     (rand() % windowHeight) - windowHeight / 2 );

	p.x = min(max(p.x, 0.), image.getWidth()-1.);  // clip at boundaries
	p.y = min(max(p.y, 0.), image.getHeight()-1.); // clip at boundaries

	int c = image.getPixelClass((int)p.x, (int)p.y);


	if (c==colClass) {
	  // particle nicht verwenden, wenn es weit (+1m) ausserhalb liegt
	  Vec pworld = robot2world * img2robot.map(p);
	  if (fabs(pworld.x) <= max_x 
	      && fabs(pworld.y) <= max_y) {
	    survivingparticles.push_back(p);

	    // sum up to calculate mean position:
	    middle = middle + p;

	    if (vis) {
	      visualizeSample((int)p.x, (int)p.y, c);
	    }
	  }
	  else if (vis) {
	    visualizeSample((int)p.x, (int)p.y, COLOR_MAGENTA);
	  }
	}
	else {
	  if (vis) {
	    visualizeSample((int)p.x, (int)p.y, c);
	  }
	}
      }
      middle *= 1. / survivingparticles.size(); // calculate mean position
    }    
    if (vis) {
      if (found()) visualizeCenter();
      vis = 0;
    }
  }

  void
  ImageParticleTracker::visualizeCenter()
  {
    Painter p(*vis);
    p.setPen(Painter::PEN_DOTTED);
    p.setColor(Painter::white);
    p.markRect((int)middle.x, (int)middle.y, 2);
  }

  void 
  ImageParticleTracker::visualizeSample(int x, int y, int c)
  {
    Painter p(*vis);
    p.setColor(ColorClassInfoList::getColorClassInfoList()->classList[c]->color);
    p.drawPoint(x,y);
  }
  
  bool
  ImageParticleTracker::found() const
  {
    return survivingparticles.size() >= minParticles; 
  }

  Vec
  ImageParticleTracker::getPositionMean() const
  {
    if (found()) {
      return middle;
    }
    else {
      throw TribotsException(__FILE__ ": No object found.");
    }
  }

  BallDetector::BallDetector(const CoordinateMapping* mapping, 
			     int centerX, int centerY, 
			     bool useRegion, bool useTrig) 
    : mapping(mapping), tracker(COLOR_BALL, centerX, centerY), vis(0),
      centerX(centerX), centerY(centerY), useRegion(useRegion),
      useTrig(useTrig)
  {}
  
  void
  BallDetector::searchBall(const Image& image, 
			   const ScanResult* scanResult, Time time, 
			   VisibleObjectList* vol, bool writeWM)
    throw (TribotsException)
  {
    if (scanResult->id != COLOR_BALL) {
      throw TribotsException(__FILE__ 
			     ": Expected results of color class COLOR_BALL "
			     "but received results of another class.");
    }

    bool useScanlinesOnlyIfBallNotSeen = false; // should not be changed.. \todo: komplett weg damit, wenns ausreichend lange getestet ist

    if (!tracker.found() || !useScanlinesOnlyIfBallNotSeen) { // if the particle tracker has not found the ball, add hypothesis from scanlines
      for (unsigned int i=0; i < scanResult->points.size(); i++) {
        tracker.addParticle(scanResult->points[i]);
      }
    }
    RobotLocation rloc = MWM.get_robot_location (time, false);
    Frame2d homtrans (rloc.pos, rloc.heading);

    if (vis) {
      tracker.setVisualize(vis);
    }
    tracker.propagate(image, *mapping, homtrans);

    if (tracker.found()) {
      Vec ballPosition = tracker.getPositionMean();
      Vec ballPositionWorld;
      
      if (useRegion) {
	LookForColorInSurrounding colorLookup(image);// try to find a pixel
	ballPosition = 
	  colorLookup.search(ballPosition, COLOR_BALL, 3); // on the ball
	
	if (image.getPixelClass((int)ballPosition.x, 
				(int)ballPosition.y) == COLOR_BALL) {
	  ChainCoder cc;
	  Region region;
	  cc.traceBorder(image, (int)ballPosition.x, (int)ballPosition.y,
			 &region);
	  ballPosition = region.getCenterOfGravity();
	  if (vis) {
	    visualizeRegion(region);
	  }
	}
	else {  // have not found a pixel on the ball
	/*
	  JWARNING("The mean center of the image particle tracker is NOT "
		   "on the ball. Therefore "
		   "I was not able to start the ChainCoding algorithm to "
		   "trace the region's border. I will use the mean center "
		   "of the particle tracker - it's my best "
		   "available estimation.");
		   */
	}	
	if (useTrig) {  // use intercept theorem to correct center o. grav.
	  InterceptTheoremCorrection itc ( 710. ); // height of mirror
	  ballPositionWorld = 
	    itc.correct(mapping->map(ballPosition), 110.); 
	}
	else {          // do not correct but find the nearest pixel
	  NearestPixelFinder npf(image, Vec(centerX, centerY));
	  ballPosition = npf.find(ballPosition, COLOR_BALL);
	  ballPositionWorld = mapping->map(ballPosition);
	}
      }
      else { //suche auf der Scanlinie center<->CoG nach dem ersten pixel
	NearestPixelFinder npf(image, Vec(centerX, centerY));
	ballPosition = npf.find(ballPosition, COLOR_BALL);
	ballPositionWorld = mapping->map(ballPosition);
	if (useTrig) {
	  InterceptTheoremCorrection itc ( 710. ); // height of mirror
	  ballPositionWorld = itc.correct(ballPositionWorld, 80.); 
	  // estimated height of intercept point
	}	    
      }      

      if (vis) {
	visualizeBall(ballPosition);
      }
      if (vol) {
	vol->objectlist.push_back(
	  VisibleObject (ballPositionWorld, VisibleObject::ball)
	  );
      }
      if (writeWM) {
	WorldModel::get_main_world_model().set_visual_information(
	  VisibleObject (ballPositionWorld, VisibleObject::ball), time
	  );
      }
    }
    if (vis) {
      vis = 0;
    }
  }

  InterceptTheoremCorrection
  ::InterceptTheoremCorrection(double cameraHeight)
    : cameraHeight(cameraHeight)
  {}
  
  Vec
  InterceptTheoremCorrection::correct(const Vec& point, double height) const
  {
    double d = point.length();
    double u = (height / cameraHeight) * d; // intercept theorems: b/h = u/d
    
    return point.normalize() * (d-u);	
  }

  LookForColorInSurrounding::LookForColorInSurrounding(const Image& image)
    : image(image)
  {}

  Vec
  LookForColorInSurrounding::search(const Vec& start, int color, 
				    int maxDist) const
  {
    int x = (int) start.x;
    int y = (int) start.y;
    
    if (image.getPixelClass(x,y) == color) {
      return start;
    }

    for (int i=1; i <= maxDist; i++) {
      for (int d=0; d < 4; d++) {
	
	int lx = x+i*ChainCoder::xDir[d];
	int ly = y+i*ChainCoder::yDir[d];

	if (lx < 0 || lx >= image.getWidth() ||  // auﬂerhalb?
	    ly < 0 || ly >= image.getHeight()) {
	  continue;
	}

	if (image.getPixelClass(lx, ly) == color) {
	  return Vec(lx, ly);
	}
      }
    }
/*    
    JWARNING("Have not found a pixel of the requested color in the "
	     "surrounding of the start point.");
    */
    return start;
  }

  NearestPixelFinder::NearestPixelFinder(const Image& image,
					 const Vec& imageCenter)
    : image(image), imageCenter(imageCenter)
  {}

  Vec
  NearestPixelFinder::find(const Vec& pos, int color)
  {
    const ColorClassInfoList* colClasses = 
      ColorClassInfoList::getColorClassInfoList();
    ScanResultList results(colClasses->classList.size());

    LineScanner scanner(new ScanLines(imageCenter, pos),
			colClasses);
    scanner.scan(image, &results);
    ScanResult* result = results.results[color];
    if (result->points.size() > 0) {
      return result->points[0];
    }
    else {
      JERROR("I was looking for the nearest pixel on the scan line from "
	     "image center to a given point, but have not found "
	     "a single pixel of the color I was looking for. I'll "
	     "use the given point instead - it's my best "
	     "available estimation.");
      return pos;
    }
  }
  
  void BallDetector::visualizeBall(const Vec& pos)
  {
    Painter p(*vis);
    p.setColor(ColorClassInfoList::getColorClassInfoList()->classList[COLOR_BALL]->color);
    p.markCrosshair((int)pos.x, (int)pos.y, 10);
    p.setColor(168, 128, 128);
    p.setPen(Painter::PEN_DOTTED);
    p.drawLine((int)pos.x, (int)pos.y, centerX, centerY);
  }

  void BallDetector::visualizeRegion(const Region& region)
  {
    Painter p(*vis);
    p.setColor(Painter::white);
    Vec point(region.x, region.y);
    for (unsigned int i=0; i < region.chainCode.size(); i++) {
      p.drawPoint((int)point.x, (int)point.y);
      point += Vec(ChainCoder::xDir[(int)region.chainCode[i]],
		   ChainCoder::yDir[(int)region.chainCode[i]]);
    }
    p.markCrossbar((int)region.getCenterOfGravity().x,
		   (int)region.getCenterOfGravity().y, 2);
  }

};
