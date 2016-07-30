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


#ifndef TribotsTools_DistanceCalibration_h
#define TribotsTools_DistanceCalibration_h

#include <string>
#include <deque>
#include "../../ImageProcessing/ColorClasses.h"
#include "../../ImageProcessing/LineScanning.h"
#include "../../Fundamental/Vec.h"

namespace TribotsTools {

  class DistanceCalibration {
  private:
   // Struktur, um fuer jeden erkannten Marker Winkel, Entfernung in Echt und Entfernung im Bild zu merken
    struct MarkerInfo {
      Tribots::Angle angle;
      unsigned int true_distance;
      double image_distance;
      MarkerInfo () throw () {;}
      MarkerInfo (const MarkerInfo& m) : angle(m.angle), true_distance(m.true_distance), image_distance(m.image_distance) {;}
      const MarkerInfo& operator= (const MarkerInfo& m) { angle=m.angle; true_distance=m.true_distance; image_distance=m.image_distance; return *this; }
    };

    std::deque<MarkerInfo> markers;
    Tribots::Vec image_center;

  public:
    DistanceCalibration (Tribots::Vec);
    ~DistanceCalibration ();
    void search_markers (const Tribots::ScanResultList&, Tribots::Angle);
    void writeMarkerFile (const std::string&);

  private:
    // Hilfsfunktion: in einer Liste von Markern den am naechstliegenden suchen mit Minimalentfernung arg3
    double search_min_transition_index (const std::deque<MarkerInfo>&, double);
  };

}

#endif
