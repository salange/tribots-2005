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

#ifndef _balldetector_h_
#define _balldetector_h_

#include "../Structures/TribotsException.h"
#include "Image2WorldMapping.h"
#include "../Fundamental/Time.h"
#include "../Structures/VisibleObject.h"
#include "../Fundamental/Vec.h"
#include "../Fundamental/Frame2D.h"

#include <vector>

namespace Tribots {

  class ScanResult;
  class Transition;
  class Image;
  class Region;

  class ImageParticleTracker {
  public:
    ImageParticleTracker(int colClass, int centerX, int centerY,
			 int windowWidth  = 40,
			 int windowHeight = 40,
			 int n = 400, 
			 unsigned int minParticles = 4);
    ~ImageParticleTracker() {;};

    void addParticle(const Vec& p);
    void propagate(const Image& image, const CoordinateMapping& img2robot, 
		   const Frame2d& robot2world);

    Vec getPositionMean() const;

    bool found() const;

    void setVisualize(Image* vis) { this->vis=vis; }

  protected:
    int colClass;
    Vec center;
    int windowWidth;
    int windowHeight;
    int n;
    unsigned int minParticles;

    std::vector<Vec> particles;
    std::vector<Vec> survivingparticles;

    Vec middle;      ///< mean of all positive particles
    double max_x, max_y;
    Image* vis;

    void visualizeSample(int x, int y, int c);
    void visualizeCenter();
  };

  class NearestPixelFinder {
  public:
    NearestPixelFinder(const Image& image, const Vec& imageCenter);
    Vec find(const Vec& pos, int color);
  protected:
    const Image& image;
    const Vec& imageCenter;
  };

  class BallDetector {
  public:
    /**
     * Erzeugt und initialisiert einen Balldetektoren mit der übergebenen
     * Koordinatentransformation (Bild->Welt).
     *
     * \param mapping Koordinatentransformation von Bild zu egozentrischen
     *                Weltkoordinaten
     */
    BallDetector(const CoordinateMapping* mapping, 
		 int centerX, int centerY, 
		 bool useRegion=true, bool useTrig=true);

    /**
     * Findet Linien in den übergebenen Resultaten des Linienscanners und
     * übergibt die gefundenen Punkte als VisibleObject an das Weltmodell.
     * Eine Linie besteht hier immer aus zwei Übergängen (zur und von der
     * Line). Für jede Paarung wird die Methode checkLinePlausibility 
     * aufgerufen, die verschiedene Plausibilitätstest durchführt und so
     * unwahrscheinliche Übergänge herausfiltert.
     *
     * \attention Ändert über einen Seiteneffekt das Weltmodell.
     *
     * \param scanResults ScanResult Struktur der Farbklasse COLOR_LINE
     * \param vol Wird hier eine VisibleObjectList übergeben, werden die
     *            Objekte nicht nur ins Weltmodell sondern auch an diese 
     *            Liste angehängt.
     * \param writeWM gibt an, ob die gefundenen Objekte ins Weltmodell
     *                geschrieben werden sollen
     */
    void searchBall(const Image& image,
		    const ScanResult* scanResult, Time time = Time(),
		    VisibleObjectList* vol=0, bool writeWM = true)
      throw (TribotsException);

    void setVisualize(Image* vis) { this->vis=vis; }
    
  protected:
    const CoordinateMapping* mapping;  ///< Koordinatentransformation
    ImageParticleTracker tracker;
    Image* vis;
    int centerX, centerY;
    bool useRegion;  ///< use chaincoder to analyze ball-cluster
    bool useTrig;    ///< use projection theorems to correct ball position

    void visualizeBall(const Vec& pos);
    void visualizeRegion(const Region& region);
  };

  class InterceptTheoremCorrection {
  public:
    InterceptTheoremCorrection(double cameraHeight);

    /** corrects a point given in real world coordinates according to the 
     *  given height of the measured point with the help of the projection
     * theorems */
    Vec correct(const Vec& point, double height) const;

  protected:
    double cameraHeight;
  };
  
  class LookForColorInSurrounding {
  public:
    LookForColorInSurrounding(const Image& image);
    Vec search(const Vec& start, int color, int maxDist) const;    
  protected:
    const Image& image;
  };

};


#endif
