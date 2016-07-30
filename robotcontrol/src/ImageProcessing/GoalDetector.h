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

#ifndef _goaldetector_h_
#define _goaldetector_h_

#include "../Structures/TribotsException.h"
#include "Image2WorldMapping.h"
#include "../Fundamental/Time.h"
#include "Image.h"
#include "../Fundamental/Vec.h"

//#define DEBUG_GOAL

namespace Tribots {

  class VisibleObjectList;
  class Image;
  
  class GoalDetector {
  public:
    /**
     * Erzeugt einen Tordektektor, der in einem Fenster der angegebenen
     * Höhe und Breite n zufällig ausgewählte Pixel auf ihre Farbklasse
     * untersucht.
     */
    GoalDetector(int centerX, int centerY, 
		 const CoordinateMapping* mapping = 0,
		 int width = 65, int height = 65, int n = 80);
    virtual ~GoalDetector() { ; }

    /**
     * Benutzt das Weltmodell, um die Position der Tore abzuschätzen und
     * sucht dort in einem Suchfenster nach gelben und blauen Pixeln.
     * Werden von der einen Farbe genügend viele und von der anderen
     * sehr wenige gesehen, gilt ein Tor als gefunden.
     */
    virtual void searchGoals(const Image&, Time time = Time(),
			     VisibleObjectList* vol=0, 
			     bool writeWM = true)
      throw (TribotsException);

    void setVisualize(Image* vis) { this->vis=vis; }

  protected:
    /** Checks the area around the given image position for the dominant
     *  color (blue, yellow or "no dominant color"). Uses some fixed 
     *  thresholds. */
    int getDominantColor(const Image& image, const Vec& position, int n);
    /** Used to draw a bold rectangle of the color of the detected goal in the 
     *  image (vis) */
    void visualizeGoal(const Vec& pos, int col);

    Vec center;
    int width; 
    int height;
    int n;

    const CoordinateMapping* mapping;

    Image* vis;        ///< points to the user-requested debug output image
  };

};


#endif
