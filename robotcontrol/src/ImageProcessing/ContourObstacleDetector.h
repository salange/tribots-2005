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

#ifndef _contourobstacledetector_h_
#define _contourobstacledetector_h_

#include "../Structures/TribotsException.h"
#include "Image2WorldMapping.h"
#include "../Fundamental/Time.h"
#include "../Fundamental/Vec.h"
#include "ChainCoding.h"
#include "ObstacleDetector.h"

#include <vector>

// #define DEBUG_OBSTACLE
#define STATUS_OBSTACLE

namespace Tribots {

  class VisibleObjectList;
  class ScanResult;
  class Transition;
  class Image;

  class ContourObstacleDetector : public ObstacleDetector {
  public:
    /**
     * Erzeugt und initialisiert einen Obstacledetektor mit der übergebenen
     * Koordinatentransformation (Bild->Welt).
     *
     * \param mapping Koordinatentransformation von Bild zu egozentrischen
     *                Weltkoordinaten
     */
    ContourObstacleDetector(const CoordinateMapping* mapping,
			    ChainCoder* cc,
			    double minTransitionSize = 4);

    virtual ~ContourObstacleDetector();

    /**
     *
     * \attention Ändert über einen Seiteneffekt das Weltmodell.
     *
     * \param image Aktuelles Bild
     * \param scanResults ScanResult Struktur der Farbklasse COLOR_LINE
     * \param vol Wird hier eine VisibleObjectList übergeben, werden die
     *            Objekte nicht nur ins Weltmodell sondern auch an diese Liste 
     *            angehängt.
     * \param writeWM gibt an, ob die gefundenen Objekte ins Weltmodell
     *                geschrieben werden sollen
     */
    virtual void searchObstacles(const Image& image,
				 const ScanResult* scanResult, 
				 Time time = Time(),
				 VisibleObjectList* vol=0, 
				 bool writeWM = true)
      throw (TribotsException);
    
  protected:
    const CoordinateMapping* mapping;  ///< Koordinatentransformation
    ChainCoder* cc;

    char* borderMap;
    int borderMapSize;

    double minTransitionSize;

  };
};


#endif
