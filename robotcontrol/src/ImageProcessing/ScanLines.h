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

#ifndef _scanlines_h_
#define _scanlines_h_

#include <vector>

#include "../Fundamental/Vec.h"
#include "../Fundamental/geometry.h"


namespace Tribots {

  /**
   * Struktur enthält eine Liste von Scanlinien, die einem LineScanner 
   * übergeben werden können.
   */
  class ScanLines {
  public:
    /**
     * Konstruktor, der eine Menge von Scanlinien erzeugt, die Radial
     * um den angegebenen Mittelpunkt angeordnet sind.
     *
     * \param middle Koordinaten des Mittelpunktes, um den die Linien 
     *               angeordnet werden sollen
     * \param innerRadius Der innere Radius gibt an, in welchem Abstand vom 
     *                    Mittelpunkt die Scanlinien beginnen sollen.
     * \param outerRaius  Der äußere Radius gibt an, in welchem Abstand vom
     *                    Mittelpunkt die Scanlinien enden sollen.
     * \param width Gesamtbreite des Bildes, an der die Scanlinien 
     *              abgeschnitten (clipping) werden sollen.
     * \param height Gesamthöhe des Bildes, an der die Scanlinien abgeschnitten
     *               (clipping) werden sollen.
     * \param n Anzahl der "Scanliniensegmente". Jedes Scanliniensegment 
     *          besteht aus mehreren, auf eine bestimmte Weise 
     *          angeordneten Scanlinien, die n-Mal wiederholt werden.
     */
    ScanLines(const Vec& middle, int innerRadius, int outerRadius, 
	      int width, int height, int n = 12);

    /** 
     * Konstruktor, der eine einzelne Scanlinie erzeugt.
     */
    ScanLines(const Vec& start, const Vec& end, int width=0, int height=0);
    
    std::vector<LineSegment> scanLines; ///< Liste von Scanlinien

  protected:
    void insertScanLine(const LineSegment& line, int width, int height);
  };

}

#endif
