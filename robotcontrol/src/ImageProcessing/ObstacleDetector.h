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

#ifndef _obstacledetector_h_
#define _obstacledetector_h_

#include "../Structures/TribotsException.h"
#include "../Fundamental/Time.h"

namespace Tribots {

  class VisibleObjectList;
  class ScanResult;
  class Transition;
  class Image;

  class ObstacleDetector {
  public:
    virtual ~ObstacleDetector();

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
      throw (TribotsException) =0;

    virtual void setVisualize(Image* vis) {;}
  };
};


#endif
