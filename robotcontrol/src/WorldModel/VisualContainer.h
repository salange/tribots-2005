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


#ifndef Tribots_VisualContainer_h_
#define Tribots_VisualContainer_h_

#include "../Structures/VisibleObject.h"
#include <vector>


namespace Tribots {

  /** Klasse, um Bildinformationen zu sammeln und zu sortieren */
  class VisualContainer {
  private:
    VisibleObjectList lines;        ///< Liste der weisen Linien
    VisibleObjectList goals;       ///< Liste der Torpunkte
    VisibleObjectList balls;       ///< Liste des Balls
    VisibleObjectList obstacles;    ///< Liste der Hindernisse
    VisibleObjectList all;       ///< alle gesehenen Objekte

  public:
    /** Konstruktor */
    VisualContainer () throw (std::bad_alloc);
    /** Destruktor */
    ~VisualContainer () throw ();

    /** Hinzufuegen von Informationen */
    void add (const VisibleObjectList&) throw (std::bad_alloc);
    /** Hinzufuegen von Informationen */
    void add (const VisibleObject&, Time) throw (std::bad_alloc);

    /** Container loeschen */
    void clear () throw ();

    /** liefert die gespeicherten Liniensegmente */
    const VisibleObjectList& get_lines () const throw ();
    /** liefert die gespeicherten Torpunkte */
    const VisibleObjectList& get_goals () const throw ();
    /** liefert die gespeicherten Ballinformation */
    const VisibleObjectList& get_balls () const throw ();
    /** liefert die gespeicherten Hindernisse */
    const VisibleObjectList& get_obstacles () const throw ();
    /** liefert alle gesehenen Objekte */
    const VisibleObjectList& get_all () const throw ();

    /** nur zu Testzwecken, Durcheinander-Mischen der Liniensegmente */
    void shuffle_lines () throw (std::bad_alloc);
    /** nur zu Testzwecken, Entfernen der letzten Liniensegmente */
    void prune_lines (unsigned int) throw ();
  };

}

#endif

