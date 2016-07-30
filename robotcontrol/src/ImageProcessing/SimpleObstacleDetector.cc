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

#include "SimpleObstacleDetector.h"
#include "LineScanning.h"
#include "ColorClasses.h"
#include "../WorldModel/WorldModel.h"
#include "Image.h"
#include "../Structures/VisibleObject.h"
#include "Painter.h"

#include <stdlib.h>

namespace Tribots {
  
  using namespace std;

  SimpleObstacleDetector
  ::SimpleObstacleDetector(const ConfigReader& config,
			   const CoordinateMapping* mapping, 
			   const CoordinateMapping* rel2img,
			   double minTransitionSize) 
    : ObstacleDetector(), mapping(mapping), rel2img(rel2img),
      minTransitionSize(minTransitionSize), vis(0)
  {
    min_width = 50;
    clustering_thresh = 300;
    config.get("SimpleObstacleDetector::clustering_thresh", clustering_thresh);
    config.get("SimpleObstacleDetector::min_width", min_width);
  } 

  SimpleObstacleDetector::~SimpleObstacleDetector()
  {}
  
  void
  SimpleObstacleDetector
  ::searchObstacles(const Image& image, 
		    const ScanResult* scanResult, Time time, 
		    VisibleObjectList* vol, bool writeWM)
    throw (TribotsException)
  {
    if (scanResult->id != COLOR_OBSTACLE) 
      {
	throw TribotsException(__FILE__ 
			       ": Expected results of color class "
			       "COLOR_OBSTACLE but received results of another "
			       "class.");
      }
    
    
    std::vector<Vec> opoints;
    Vec obstaclePosition;
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
      
      //if (transStart.virt == true || transEnd.virt == true) { // skip virtual
      //continue;
      //}
      
      // plausibilitätstest, minimale Breite erforderlich
      if ((transStart.toPos - transEnd.fromPos).length() < minTransitionSize) {
	continue;
      }

      // Plausibilitätstest ... angesetzte Scanlnien können im Hindernis beginnen ... nicht ganz sauber
      //if ((mapping->map(transStart.toPos)).length() > 1000.0 && transStart.virt == true) continue;

      // Position berechnen; \todo nicht einfach den ersten Punkt nehmen
      obstaclePosition = mapping->map(transStart.toPos);
      
      opoints.push_back(obstaclePosition);
    }


    SimpleVecCluster clust(clustering_thresh);
    for (unsigned int h=0; h < opoints.size(); h++) {

      clust.add(opoints[h]);

#if 0      
      if (vol) {
	vol->objectlist.push_back(VisibleObject (opoints[h], VisibleObject::obstacle) );
      }
      if (writeWM) {
	WorldModel::get_main_world_model().set_visual_information( VisibleObject (opoints[h], VisibleObject::obstacle), time );
      }
#endif
    }
    
    //clust.prune(); //funktioniert nicht ... noch nicht
    for (unsigned int i=0;i<clust.cluster.size(); i++)
      {
	if (clust.cluster[i].width < min_width) continue;
	if (vol) {
	  vol->objectlist.push_back(VisibleObject (clust.cluster[i].support, VisibleObject::obstacle, clust.cluster[i].width) );
	}
	if (writeWM) {
	  WorldModel::get_main_world_model().set_visual_information( VisibleObject (clust.cluster[i].support, VisibleObject::obstacle, clust.cluster[i].width), time );
	}
      } 
    

    if (vis) {
      paintResults(clust);
      vis = 0;
    }
  }


  void
  SimpleObstacleDetector::paintResults(SimpleVecCluster& cluster)
  {
    Painter p(*vis);
    p.setColor(200, 200, 200);

    for (unsigned int cl=0; cl < cluster.cluster.size(); cl++) {
      if (cluster.cluster[cl].width < min_width) continue;
      p.setPen(cl%2);
      p.setColor(((cl/2)%2) * 50 + 150, 
		 ((cl/2)%2) * 50 + 150, 
		 ((cl/2)%2) * 50 + 150);
      for (unsigned int s=0; s < cluster.cluster[cl].samples.size(); s++) {
	Vec point = rel2img->map(cluster.cluster[cl].samples[s]);
	p.markCrosshair((int)point.x, (int)point.y, 3);
      }
      p.setPen(Painter::PEN_SOLID);
      Vec point = rel2img->map(cluster.cluster[cl].support);
      p.markRect((int)point.x, (int)point.y, 3);
      p.markRect((int)point.x, (int)point.y, 5);
    }   
  }
};


