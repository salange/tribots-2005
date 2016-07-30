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


#ifndef Tribots_EMAObstacleFilter_h
#define Tribots_EMAObstacleFilter_h

#include <deque>
#include "../Structures/VisibleObject.h"
#include "../Structures/ObstacleLocation.h"
#include "../Structures/RobotLocation.h"
#include "../Structures/BallLocation.h"
#include "../Structures/FieldGeometry.h"
#include "../Fundamental/ConfigReader.h"


namespace Tribots {

  /** Hindernis-Filter auf Grundlage einer exponentiellen Glaettung;
      filtert kurzfristige Stoerungen in der Hinderniserkennung;
      (bis maximal 3 Frames Stoerung) */
  class EMAObstacleFilter {
  public:
    /** Konstruktor */
    EMAObstacleFilter (const ConfigReader&, const FieldGeometry&) throw ();
    /** Destruktor */
    ~EMAObstacleFilter () throw ();

    /** liefere Position der Hindernisse*/
    ObstacleLocation get () const throw ();
    /** liefere Position der Hindernisse einschliesslich der Eckpfosten */
    ObstacleLocation get_with_poles () const throw ();
    /** liefere Position der Hindernisse einschliesslich der Eckpfosten 
	und der Positionen, an denen der Roboter blockiert war */
    ObstacleLocation get_with_poles_and_stuck () const throw ();
    /** analysiere die Hindernisse anhand der visuellen Information;
	uebergeben wird die Liste erkannter Hindernisse und die Roboterposition zum Zeitpunkt
	der Bildinformation */
    void update (const VisibleObjectList&, const RobotLocation&, const BallLocation&) throw ();

  private:
    /** Struktur, um Hindernisse intern zu verwalten */
    struct ObstacleProperties {
      Vec pos;
      double width;
      double probability;
      bool active;
    };
    struct AssignmentProperties {
      Vec pos;
      double width;
      double nearest_dist;
      int nearest_index;
    };
    std::deque<ObstacleProperties> obstacles;       ///< Liste vorhandener Hindernisse
    std::vector<AssignmentProperties> assignments;  ///< Assignment-Container fuer interne Berechnungen
    double ema;                                     ///< EMA-Glaettungsparameter (je kleiner, desto kurzfristiger)
    double hysterese_lower;                         ///< unterer Schwellwert der Hysterese
    double hysterese_higher;                        ///< oberer Schwellwert der Hysterese
    ObstacleLocation poles;                         ///< Liste der Eckpfosten
    unsigned int stuck_obstacle_delay;              ///< Zeit in ms, die Stuck-Hindernisse vorhanden sein sollen
    bool remove_ball_obstacles;                     ///< (scheinbare) Hindernisse vor dem Ball entfernen
  };

}

#endif
