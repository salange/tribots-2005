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

#ifndef _chaincoding_h_
#define _chaincoding_h_

#include <vector>
#include "../Fundamental/Vec.h"

namespace Tribots {

  class Image;

  class Region {
  public:
    int colClass;
    int x, y;
    std::vector<char> chainCode;
    int maxX, maxY, minX, minY;

    inline Region(int colClass, int x, int y);
    inline Region();
      
    int getArea() const;
    Vec getCenterOfGravity() const;
    double getCompactness() const;

  protected:
    int area;
    Vec centerOfGravity;
  };

  /**
   * Schreitet eine Region (Zusammenhängende Pixel mit der gleichen Farbklasse)
   * ab und erzeugt einen Kettencode. Die Schrittrichtungen werden dabei wie
   * folgt kodiert: 0 rechts, 1 "oben" (größere y-Koordinate), 3 links, 4 unten
   *
   *    |1           y ^ 
   * 2  |              |
   * --- ---           |
   *    |  0           |
   *   3|              ------> x
   *
   * Imlpementierung mit 8er Nachbarschaft wäre auch noch möglich...
   */
  class ChainCoder {
  public:
    static int xDir[4];
    static int yDir[4];

    virtual ~ChainCoder() {;}
    virtual int traceBorder(const Image& image, int x, int y, Region* region,
			    char* borderMap = 0);

    void setVisualize(Image* vis) { this->vis = vis; }
  protected:
    Image* vis;

    void visualizeRegion(const Region* region);
  };

  class RegionDetector 
  {
  public: 
    /**
     * Konstruktor, der mit einem Kettenkodierer initialisiert wird, der in 
     * den Besitz des RegionDetector übergeht.
     */
    RegionDetector(ChainCoder* cc = new ChainCoder());
    virtual ~RegionDetector();
    /**
     * findet alle Regionen der Farbe colClass im Bild und fügt sie an
     * die übergebene Liste an. Alle Randpixel des Bildes image müssen dabei 
     * von der Hintergrundklasse (0) sein!
     */
    virtual void findRegions (const Image& image, int colClass, 
			      std::vector<Region>* regions) const;
  protected:
    char* buf;
    int size;
    ChainCoder* cc;
  };
    
  // inlines

  Region::Region(int colClass, int x, int y) 
    : colClass(colClass), x(x), y(y), maxX(x), maxY(y), minX(x), minY(y),
      area(-1)
  {}

  Region::Region()
    : colClass(0), x(0), y(0), maxX(0), maxY(0), minX(0), minY(0),
      area(-1)
  {}


};


#endif
