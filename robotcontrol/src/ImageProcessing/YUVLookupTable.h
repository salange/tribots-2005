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

#ifndef _yuvlut_h_
#define _yuvlut_h_

#include "ColorClassifier.h"
#include "Image.h"

namespace Tribots {
  
  /**
   * Implementiert die Farbklassifizierung mittels einer Nachschlagetabelle
   * für YUV Werte. Andere Farbwerte werden zuerst in YUV umgerechnet und 
   * dann nachgeschlagen. Um die Tabelle möglichst klein zu halten, kann
   * man für jeden Kanal (Y, U, V) einen eigenen Shift Wert angeben, um den
   * die Werte dieses Kanals nach rechts (>>) verschoben werden. Für jeden
   * Shift eines einzelnen Kanals halbiert sich die Anzahl der benötigten 
   * Einträge.
   *
   * Dateiformat (ios::binary):
   *
   * HEADER DATA
   *
   * HEADER:
   *
   * shift_y SPACE shift_u SPACE shift_v NEW_LINE
   *
   * SPACE:    ' '
   *
   * NEW_LINE: '\\n'
   *
   * schift_y, shift_u, shift_v sind ints
   * 
   * DATA:
   * 255 >> shift_y * 255 >> shift_u * 255 >> shift_v char 's (Farbklassen)
   *
   * Die Farbklasse für das YUV Triple (x,y,z) steht im Datenbereich an der 
   * Stelle ((x >> shift_y) * (255 >> shift_u) + (y >> shift_u)) 
   *        * (255 >> shift_v) + (z >> shift_v) .
   *
   * Di Anordnung des Datenbereichs sieht also folgendermaßen aus:
   * (0,0,0) (0,0,1) ... (0,0,255) (0, 1, 0) .. (0, 1, 255) .. 
   * (0, 255, 255) (1, 0, 0) .. (1, 255, 255) .. (255, 255, 255)
   */
  class YUVLookupTable : public ColorClassifier {
  public:
    /**
     * Kontruiert eine neue, leere Nachschlagetabelle mit den angegebenen
     * Shiftwerten. In der Standardeinstellung werden der Y-Kanal um ein Bit,
     * der U-Kanal und der V-Kanal um zwei Bit nach rechts geschiftet. Somit
     * werden nur 128*64*64 = 114.688 statt 256^3 = 16.777.216 Einträge
     * benötigt.
     *
     * \param shiftY Anzahl der Bits, um den der Y-Kanal nach rechts verschoben
     *               wird.
     * \param shiftU Anzahl der Bits, um den der U-Kanal nach rechts verschoben
     *               wird.
     * \param shiftV Anzahl der Bits, um den der V-Kanal nach rechts verschoben
     *               wird.
     */
    YUVLookupTable(int shiftY = 1,
		   int shiftU = 2,
		   int shiftV = 2);		  
    /**
     * Destruktor, löscht die interne Nachschlagetabelle.
     */
    virtual ~YUVLookupTable();

    /**
     * rechnet rgb in ein YUVTuple um und liefert dessen Farbklasse zurück
     */
    virtual const unsigned char& lookup(const RGBTuple&  rgb)  const;
    /**
     * schlägt die Klasse von yuv in der Tabelle nach
     */
    virtual const unsigned char& lookup(const YUVTuple&  yuv)  const;
    /**
     * rechnet uyvy in ein YUVTuple um und liefert dessen Farbklasse zurück
     */
    virtual const unsigned char& lookup(const UYVYTuple& uyvy, int pos=0)const;

    /**
     * Setzt die Klasse für den angebenen Farbwert auf das angegebene Label
     * c. Hierbei werden Farben, die im gleichen "Block" (entstehen durch das
     * Shiften der Werte der einzelnen Kanäle) wie rgb liegen, ebenfalls
     * der Klasse c zugeordnet.
     */
    virtual void set(const RGBTuple&  rgb,  unsigned char c);
    /**
     * Setzt die Klasse für den angebenen Farbwert auf das angegebene Label
     * c. Hierbei werden Farben, die im gleichen "Block" (entstehen durch das
     * Shiften der Werte der einzelnen Kanäle) wie yuv liegen, ebenfalls
     * der Klasse c zugeordnet.
     */
    virtual void set(const YUVTuple&  yuv,  unsigned char c);
    /**
     * Setzt die Klasse für den angebenen Farbwert auf das angegebene Label
     * c. Hierbei werden Farben, die im gleichen "Block" (entstehen durch das
     * Shiften der Werte der einzelnen Kanäle) wie uyvy liegen, ebenfalls
     * der Klasse c zugeordnet.
     */
    virtual void set(const UYVYTuple& uyvy, unsigned char c, int pos=0);

    /**
     * lädt die Nachschlagetabelle aus einer Datei.
     */
    virtual void load(string filename);
    /**
     * speichert die Nachschlagetabelle in einer Datei
     */
    virtual void save(string filename) const;

    /**
     * Erzeugt eine neue, leere Lookuptabelle mit den gleichen Schiftwerten
     */
    virtual ColorClassifier* create() const;

    /**
     * Füllt diese Lookuptabelle entsprechend den Vorgaben des übergebenen
     * Classifiers. Die alten Farbzuordnungen werden dabei überschrieben.
     */
    virtual void fillFromClassifier(const ColorClassifier* cc);

  protected:
    int shiftY;   ///< Anzahl der Bits, um den die Y-Werte geschiftet werden
    int shiftU;   ///< Anzahl der Bits, um den die Y-Werte geschiftet werden
    int shiftV;   ///< Anzahl der Bits, um den die Y-Werte geschiftet werden

    int sizeY;    ///< sizeY = 255 >> shiftY
    int sizeU;    ///< sizeU = 255 >> shiftU
    int sizeV;    ///< sizeV = 255 >> shiftV
    int size;     ///< Gesamtzahl der Einträge in der Nachschlagetabelle

    unsigned char* lut;///< Zeiger auf die Nachschlagetabelle
    
  private:
    YUVTuple yuvTmp;   ///< Zwischenspeicher für die Transfomierung nach YUV
  };

}

#endif
