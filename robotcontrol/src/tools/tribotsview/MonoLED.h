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


#ifndef TribotsTools_MonoLED_h
#define TribotsTools_MonoLED_h

#include <qwidget.h>

namespace TribotsTools {

  /** Widget, eine einfarbige LED simuliert */
  class MonoLED : public QWidget {
    Q_OBJECT

   public:
    /** Konstruktor; 
	Argumente wie bei normalen Widgets 
	Arg4: Farbe der LED bei Aktivierung */
    MonoLED ( QWidget* = 0, const char* = 0, WFlags = 0, QColor = Qt::red);
    /** Destruktor */
    ~MonoLED ();
    
    const QColor& color () const throw ();   ///< Farbe abfragen
    bool isOn () const throw ();             ///< Zustand abfragen
    void setColor (QColor) throw ();         ///< Farbe setzen

  public slots:
    void setOn (bool);                ///< an/ausschalten

  protected slots:
    void paintEvent(QPaintEvent *);   ///< Zeichnen des Widgetinhalts

  private:
    QColor col;
    bool on;
 };

}

#endif

