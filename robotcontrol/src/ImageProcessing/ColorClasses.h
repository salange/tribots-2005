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

#ifndef _colorclasses_h_
#define _colorclasses_h_

#include <vector>
#include <string>

#include "../Fundamental/ConfigReader.h"
#include "Image.h"

namespace Tribots {

  /**
   * Robocup spezifische Liste von Farben
   */
  enum { COLOR_IGNORE = 0,
	 COLOR_BALL,
	 COLOR_BLUE,
	 COLOR_YELLOW,
	 COLOR_OBSTACLE,      ///< black
	 COLOR_LINE,          ///< white
	 COLOR_FIELD,         ///< green
	 COLOR_CYAN,          ///< unused
	 COLOR_MAGENTA        ///< unused
  };
  /**
   * Informationen zu einer bestimmten Farbklasse.
   */
  struct ColorClassInfo {
    int id;               ///< Nummer der Farbklasse (COLOR_IGNORE,...)
    std::string name;     ///< Name der Farbklasse (für mögliche Ausgaben)
    RGBTuple color;       ///< "Prototypfarbe" (für mögliche Visualisierungen)
    bool findTransitions; ///< Bei dieser Farbklasse Übergänge im Bild merken?
    bool findOnlyFirst;   ///< Sascha das waren wir!
    bool findPoints;      ///< Bei dieser Farbklasse alle Pixel merken?
    
    /**
     * Konstruktor.
     *
     * \param id Nummer der Farbklasse
     * \param name (kurzer) Name der Farbklasse
     * \param color "Protoypfarbe", die zum Beispiel zur Visualisierung dieser
     *              Klasse verwendet werden kann
     * \param findTransitions gibt an, ob bei dieser Farbklasse im Bild
     *                        gefundene Übergänge von Interesse sind
     * \param findPoints gibt an, ob bei dieser Farbklasse im Bild gefundene
     *                   Pixel von Interesse sind
     */
    ColorClassInfo(int id, std::string name, const RGBTuple& color,
		   bool findTransitions=true, bool findPoints=false, 
		   bool findOnlyFirst=false);
  };

  /**
   * Enthält Informationen über alle dem System bekannten Farbklassen. 
   */
  class ColorClassInfoList {
  public:
    std::vector<ColorClassInfo*> classList;

    /**
     * Constructs empty class list (only class is the background class with 
     * id 0.
     */
    ColorClassInfoList();
    /**
     * Constructs default class list as needed by robocup and fills it with 
     * additional values set in the configuration files.
     */
    ColorClassInfoList(const ConfigReader& config);
    /**
     * Destruktor.
     */
    ~ColorClassInfoList();

    static const ColorClassInfoList* 
      getColorClassInfoList(const ConfigReader* config = 0);

  protected:
    static ColorClassInfoList* theColorClassInfoList;

  private:
  void std_init ();  // wird von den Konstruktoren aufgerufen
  };

};

#endif
