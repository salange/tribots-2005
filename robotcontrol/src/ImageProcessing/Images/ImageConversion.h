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

#ifndef _imageconversion_h_
#define _imageconversion_h_


#include "../../Structures/TribotsException.h"
#include <map>

namespace Tribots {

  class ImageBuffer; // forward declaration of the image buffer

  /**
   * Schnittstelle f�r Konvertierungsfunktionen. Ein ImageConverter   
   * kann einen ImageBuffer genau eines Formats in einen ImageBuffer 
   * eines bestimmten anderen Formats (m�glichst effizient) umwandeln. 
   * F�r jede m�gliche Formatkombination m�ssen getrennte Konverter 
   * implementiert werden.
   */
  class ImageConverter
  {
  public:
    /**
     * Konstruktor. Die Formate werden von den einzelnen Implementierungen
     * passend gesetzt.
     */
    ImageConverter(int srcFormat, int dstFormat);

    /**
     * Konvertiert und schreibt den ImageBuffer src in den ImageBuffer dst.
     * Ist das Format von src und dst gleich, wird der Puffer nur kopiert.
     * Werden die Formate von src und dst nicht von dem Konverter unterst�tzt
     * wird eine TribotsException geworfen.
     */
    virtual void operator() (const ImageBuffer& src, ImageBuffer& dst) const
      throw (TribotsException)=0;
    /**
     * liefert das Format, das der Konverter auf Seiten der Quelle unterst�tzt
     */
    int getSourceFormat() { return srcFormat; }
    /**
     * liefert das Format, das der Konverter auf Seiten des Ziels unterst�tzt.
     */
    int getDestinationFormat() { return dstFormat; }

  protected:
    int srcFormat;   ///< Unterst�tztes Format der Quelle
    int dstFormat;   ///< Unterst�tztes Format des Ziels
  };

  /**
   * Zentrale, als Singleton implementierte Registrierung, bei der alle 
   * vorhandenen Konvertierungsfunktionen f�r ImageBuffer registriert und
   * unter Angabe des gew�nschten Quellen- und Zielformats nachgeschlagen
   * werden k�nnen.
   */
  class ImageConversionRegistry
  {
  public:
    /**
     * liefert einen Zeiger auf die zentrale Registrierung.
     *
     * \attention Kein delete auf den �bergebenen Zeiger ausf�hren!
     */
    static ImageConversionRegistry* getInstance();
    /**
     * Destruktor.
     */
    ~ImageConversionRegistry();

    /**
     * schl�gt einen Konverter nach. Ist keine passende Implementierung
     * registriert, wird eine Exception geworfen.
     *
     * \param Format der Quelle
     * \param Format des Ziels
     */
    const ImageConverter* getConverter(int srcFormat, int dstFormat) 
      throw (TribotsException);
    /**
     * registriert eine Implementierung. Wurde vorher bereits eine
     * andere Implementierung f�r die gleiche Formatumwandlung registriert,
     * wird diese �berschrieben.
     *
     * \todo Beim �berschreiben muss die alte Implementierung auch 
     *       gel�scht werden!!!
     */
    void registerConverter(ImageConverter* converter);

  protected:
    std::map<int, std::map<int, ImageConverter*> > converterMap;
    static ImageConversionRegistry* singleton;

    /**
     * gesch�tzter Konstruktor, Zugriff auf das Singleton nur mittels 
     * getInstance()
     */
    ImageConversionRegistry();    
  };

  /**
   * Wandelt Puffer von YUV nach RGB um.
   */
  class YUV2RGB : public ImageConverter
  {
  public:
    YUV2RGB();
    virtual void operator() (const ImageBuffer& src, ImageBuffer& dst) const
      throw (TribotsException);
  };

  /**
   * Wandelt Puffer von YUV (wobei die Kan�le nach dem Schema UYV angeordnet 
   * sind) nach RGB um.
   */
  class UYV2RGB : public ImageConverter
  {
  public:
    UYV2RGB();
    virtual void operator() (const ImageBuffer& src, ImageBuffer& dst) const
      throw (TribotsException);
  };

  /**
   * Wandelt Puffer von YUV 4:2:2 nach YUV um.
   */
  class UYVY2YUV : public ImageConverter
  {
  public:
    UYVY2YUV();
    virtual void operator() (const ImageBuffer& src, ImageBuffer& dst) const
      throw (TribotsException);
  };

  /**
   * Wandelt Puffer von YUV 4:2:2 nach RGB um.
   */
  class UYVY2RGB : public ImageConverter
  {
  public:
    UYVY2RGB();
    virtual void operator() (const ImageBuffer& src, ImageBuffer& dst) const
      throw (TribotsException);
  };

}


#endif
