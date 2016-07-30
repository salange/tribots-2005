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


#ifndef _Tribots_SimulatorImageProcessing_h_
#define _Tribots_SimulatorImageProcessing_h_

#include "../ImageProcessing/ImageProcessingType.h"
#include "../Fundamental/geometry.h"
#include "SimClient.h"
#include "../Fundamental/ConfigReader.h"
#include <deque>

namespace Tribots {

  /** Klasse SimulatorImageProcessing erzeugt eine virtuelle Bildverarbeitung,
      die ihre Eingaben aus dem Simulator liest und Tore sowie weiﬂe Linien liefert */
  class SimulatorImageProcessing : public ImageProcessingType {
  private:
   SimClient* the_sim_client;         ///< pointer auf den SimClient
   double noise_level;                ///< Standardabweichung fuer Rauschen
   double mis_probability;            ///< Wahrscheinlichkeit, dass ein Sensorwert fehlt
   double goal_mis_probability;       ///< Wahrscheinlichkeit, dass ein Torsensorwert fehlt
   double ball_mis_probability;       ///< Wahrscheinlichkeit, dass Ball nicht gefunden wird
   double line_vision_radius;              ///< Radius, bis zu dem Linien erkannt werden
   std::deque<LineSegment> lines;     ///< Spielfeldlinien
   std::deque<Arc> arcs;              ///< Spielfeldlinien
  public:
   /** Konstruktor */
   SimulatorImageProcessing (const ConfigReader&) throw (std::bad_alloc, Tribots::InvalidConfigurationException);
   /** Destruktor */
   ~SimulatorImageProcessing () throw ();
   /** virtuelle Sensorinformationen liefern */
   void process_image () throw ();
  };

}

#endif
