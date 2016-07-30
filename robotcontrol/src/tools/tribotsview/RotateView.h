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


#ifndef TribotsTools_RotateView_h
#define TribotsTools_RotateView_h

#include <qwidget.h>
#include "FieldOfPlay.h"

namespace TribotsTools {

  /** Widget, das eine runde Scheibe mit Angabe der Roboterausrichtung und 
      -Rotationsgeschwindigkeit darstellt */
  class RotateView : public QWidget {
    Q_OBJECT

   public:
    /** Konstruktor; Achtung: vor Verwendung muss init() aufgerufen werden 
	Argumente wie bei normalen Widgets */
    RotateView ( QWidget* = 0, const char* = 0, WFlags = 0 );
    /** Destruktor */
    ~RotateView ();

    /** Initialisierung 
	RotateView wird an ein FieldOfPlay angebunden, das hier uebergeben wird. 
	RotateView aktualisiert sich automatisch bei Veraenderungen im referenzierten FieldOfPlay */
    void init (const FieldOfPlay*);

  protected slots:
    void paintEvent(QPaintEvent *);   ///< Zeichnen des Widgetinhalts

  private:
    const FieldOfPlay* fop;           ///< das FieldOfPlay, aus dem die Informationen genommen werden
 };

}

#endif

