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

#ifndef _ColorClassifier_h_
#define _ColorClassifier_h_

#include <string>

#include "Image.h"

namespace Tribots {

  using std::string;
  
  /**
   * Ein ColorClassifier klassifiziert übergebene Farbwerte. Die 
   * zurückgegebenen Werte sollten den in der verwendeten ColorClassList
   * enthaltenen Informationen entsprechen.
   *
   * \todo Eventuell Zusammenhang zwischen ColorClassList und
   *       ColorClassifier herstellen (zum Beispiel ColorClassList mit
   *       in der Lookuptabelle abspeichern). Derzeit etwas unschön...
   */
  class ColorClassifier {
  public:
    /**
     * klassifiziere das gegebene RGBTuple
     */
    virtual const unsigned char& lookup(const RGBTuple&  rgb)  const =0;
    /**
     * klassifiziere das gegebene YUVTuple
     */
    virtual const unsigned char& lookup(const YUVTuple&  yuv)  const =0;
    /**
     * klassifiziere das gegebene UYVYTuple
     */
    virtual const unsigned char& lookup(const UYVYTuple& uyvy, 
					int pos=0) const =0;

    /**
     * ändere das Klassenlabel des übergebenen RGBTuple rgb auf Klasse c.
     * Wie die unterschiedlichen Implementierungen mit dieser Information
     * verfahren, ist nicht näher definiert. Eventuell können angrenzende
     * Farben ebenfalls geändert oder der Aufruf gänzlich ignoriert werden.
     *
     * \param rgb Farbwert, dessen Klassifizierung geändert werden soll
     * \param c   neues Klassenlabel
     */
    virtual void set(const RGBTuple&  rgb,  unsigned char c) =0;
    /**
     * ändere das Klassenlabel des übergebenen YUVTuple yuv auf Klasse c.
     * Wie die unterschiedlichen Implementierungen mit dieser Information
     * verfahren, ist nicht näher definiert. Eventuell können angrenzende
     * Farben ebenfalls geändert oder der Aufruf gänzlich ignoriert werden.
     *
     * \param yuv Farbwert, dessen Klassifizierung geändert werden soll
     * \param c   neues Klassenlabel
     */
    virtual void set(const YUVTuple&  yuv,  unsigned char c) =0;
    /**
     * ändere das Klassenlabel des übergebenen UYVYTuple uyvy auf Klasse c.
     * Wie die unterschiedlichen Implementierungen mit dieser Information
     * verfahren, ist nicht näher definiert. Eventuell können angrenzende
     * Farben ebenfalls geändert oder der Aufruf gänzlich ignoriert werden.
     *
     * \param uyvy Farbwert, dessen Klassifizierung geändert werden soll
     * \param c    neues Klassenlabel
     */
    virtual void set(const UYVYTuple& uyvy, unsigned char c, int pos=0) =0;

    /**
     * lädt einen Klassifikator aus einer Datei. Das Dateiformat ist vom
     * Klassifikator abhängig (und dort beschrieben).
     */
    virtual void load(string filename) =0;
    /**
     * schreibt einen Klassifikator in eine Datei. Das Dateiformat ist vom
     * Klassifikator abhängig (und dort beschrieben).
     */
    virtual void save(string filename) const =0;

    /**
     * erzeugt einen neuen Klassifikator (Kopie).
     */
    virtual ColorClassifier* create() const =0;
    /**
     * erzeugt einen neuen Klassifikator, der aus einer Datei eingelesen 
     * wird.
     */
    virtual ColorClassifier* createFromFile(string filename) const;
  };

  /**
   * Standardimplementierung, die für jeden Farbwert 0 (COLOR_IGNORE)
   * zurückliefert.
   */
  class DefaultClassifier : public ColorClassifier {
  public:
    virtual const unsigned char& lookup(const RGBTuple&  rgb)  const;
    virtual const unsigned char& lookup(const YUVTuple&  yuv)  const;
    virtual const unsigned char& lookup(const UYVYTuple& uyvy, 
					int pos=0) const;

    virtual void set(const RGBTuple&  rgb,  unsigned char c);
    virtual void set(const YUVTuple&  yuv,  unsigned char c);
    virtual void set(const UYVYTuple& uyvy, unsigned char c, int pos=0);

    /**
     * hat keinen Effekt
     */
    virtual void load(string filename);
    /**
     * hat keinen Effekt
     */
    virtual void save(string filename) const;

    /**
     * es ist verboten, vom DefaultClassifier eine Kopie anzulegen.
     * create() liefert daher im Falle eines Aufrufs eine Exception.
     */
    virtual ColorClassifier* create() const;

    /**
     * Liefert einen Zeiger auf die einzige Instanz des DefaultClassifier.
     */
    static const DefaultClassifier* getInstance();

  protected:
    /**
     * Da es sich um ein Singleton handelt, soll der Konstrukter nicht 
     * aufgerufen werden.
     */
    DefaultClassifier();               
    static DefaultClassifier* singleton; ///< einzige Instanz
    unsigned char defValue;              ///< default value, 0 für COLOR_IGNORE
  };

  /**
   * Klassifiziert schwarze Pixel (0,0,0) als Klasse 0, weisse Pixel 
   * (255, 255, 255) als Klasse 1. Für andere Farbwerte ist die Klassifizierung
   * undefiniert.
   */
  class WhiteClassifier : public DefaultClassifier {
  public:
    WhiteClassifier();

    virtual const unsigned char& lookup(const RGBTuple&  rgb)  const;
    virtual const unsigned char& lookup(const YUVTuple&  yuv)  const;
    virtual const unsigned char& lookup(const UYVYTuple& uyvy, 
					int pos=0) const;

  protected:
    unsigned char white;
    unsigned char black;
  };

  /**
   * Klassifiziert schwarze Pixel (0,0,0) als Klasse 1, weisse Pixel 
   * (255, 255, 255) als Klasse 0. Für andere Farbwerte ist die Klassifizierung
   * undefiniert.
   */
  class BlackClassifier : public WhiteClassifier {
  public:
    BlackClassifier();
  };

}

#endif
