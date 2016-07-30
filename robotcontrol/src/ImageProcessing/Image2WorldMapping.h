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

#ifndef _image2worldmapping_h_
#define _image2worldmapping_h_

#include <string>

#include "../Fundamental/Vec.h"
#include "../Fundamental/geometry.h"
#include "../Structures/TribotsException.h"

namespace Tribots {

  class CoordinateMapping {
  public:
    virtual ~CoordinateMapping() {};

    virtual Vec map(const Vec& vec) const =0;
    virtual Line map(const Line& line) const;
    virtual LineSegment map(const LineSegment& line) const;
  };

  /**
   * Bildet ganzzahlige Bildkoordinaten auf relative Weltkoordinaten ab.
   * Die Abbildung wird durch eine Nachschlagetabelle realisiert, in der die
   * korrekten Weltkoordinaten für jedes einzelne Pixel gespeichert sind.
   *
   * Die Nachschlagetabelle wird aus einer externen Datei eingelesen, die 
   * mit Hilfe eines Kalibrierungstools erzeugt wurde. Dabei hat
   * die Datei ein binäres Format. Vor dem Start der eigentlichen Daten
   * steht die Höhe und die Breite der Tabelle: "HÖHE BREITE\n". Die beiden int
   * werden durch EINE Leerstelle getrennt. Direkt nach der BREITE folgt
   * ein Zeilenunbruchzeichen '\n'. Hieran schließt sich der Puffer der 
   * Nachschlagetabelle an (Bildzeilenweise organisiert, ios::binary). 
   *
   * Die Struktur der binären Daten ist von der Implementierung des Vektors
   * (Vec.h) abhängig. Derzeit bilden je 16 Byte einen Block. Ein Block
   * entspricht hierbei einem Eintrag in der Nachschlagetabelle, bzw.
   * einem Object vom Typ Vec. Jeder Block besteht aus zwei double Werten
   * für x- (die ersten 8 Byte) und y-Koordiante (die letzten 8 Byte).
   */
  class Image2WorldMapping : public CoordinateMapping {
  public:
    Image2WorldMapping(std::string filename);
    Image2WorldMapping(int width, int height);
    virtual ~Image2WorldMapping();
    
    /**
     * Bildet einen Vektor mit Bildkoordinaten in relative Weltkoordinaten ab.
     *
     * \param  vec Vektor mit Bildkoordinaten (Pixel)
     * \returns    Vektor mit relativen Weltkoordinaten
     */
    virtual Vec map(const Vec& vec) const;
    /**
     * Bildet x-y-Bildkoordinaten in relative Weltkoordinaten ab.
     *
     * \param  x X-Koordinate des Pixels
     * \param  y Y-Koordinate des Pixels
     * \returns  Vektor mit relativen Weltkoordinaten
     */
    virtual Vec map(int x, int y) const;

    /**
     * Lädt eine Nachschlagetabelle aus einer Datei. Die Datei enthält
     * Informationen über die robozentrischen Weltkoordinaten für jedes 
     * einzelne Pixel. Daher muss die Größe der in der Datei enthaltenen
     * Nachschlagetabelle (siehe getWidth und getHeight Methoden) zu der 
     * aktuell verwendeten Bildgröße passen!
     */
    virtual void load(std::string filename);
    /**
     * Speichert die aktuelle Nachschlagetabelle in einer Datei.
     */
    virtual void save(std::string filename) const;

    /**
     * Setzt den relativen Weltkoordinatenvektor für das Pixel an der 
     * Stell (x,y). Wird zum erzeugen der Nachschlagetabelle in der 
     * Kalibrierungsphase benötigt.
     */
    virtual void set(int x, int y, const Vec& vec);
    
    virtual int getWidth() const { return width; };
    virtual int getHeight() const { return height; }

  protected:
    Vec* lut;   ///< holds the relative world coords for every pixel
    int width;
    int height;
  };

  /**
   * Bildet egozentrische Weltkoordinaten in Bildkoorinaten ab.
   */
  class World2Image : public CoordinateMapping {
  public:
    World2Image(std::string filename);
    virtual ~World2Image();

    virtual Vec map(const Vec& vec) const;

    void setMiddle(const Vec& middle);
    Vec getMiddle() const;

  protected:
    int width, height;
    Vec middle;
    int shift;

    std::vector<double> realDistances;
    std::vector<std::vector< double> > imageDistances;
    std::vector<double> angleCorrections;
  };

  
}

#endif /* _image2worldmapping_h_ */
