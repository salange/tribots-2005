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


#ifndef Tribots_RobotPositionPredictor_h_
#define Tribots_RobotPositionPredictor_h_

#include "../Structures/RobotLocation.h"
#include "../Fundamental/Time.h"

namespace Tribots {

  /** Klasse, um eine kurzfristige Vorhersage der Roboterposition durchzufuehren */
  class RobotPositionPredictor {
  private:
    RobotLocation current_rloc;       ///< Ausgangssituation
    Time timestamp_current_rloc;    ///< Zeitpunkt der Ausgangsposition
    Time timestamp_calculation;    ///< letzter Sensorintegrationsschritt
  public:
    /** Konstruktor */
    RobotPositionPredictor () throw ();
    /** uebergeben von neuer Information;
	arg1: neue Position, Geschwindigkeit, Kicker und Basisqualitaet der Schaetzung
	arg2: Zeitpunkt, den arg1 repraesentiert
	arg3: letzter Zeitpunkt, zu dem ein Sensorintegrationsschritt erfolgt ist */
    void set (const RobotLocation&, Time, Time) throw ();
    /** berechnen einer Position und Geschwindigkeit zu Zeitpunkt arg1 */
    RobotLocation get (Time) const throw ();
  };

}

#endif
