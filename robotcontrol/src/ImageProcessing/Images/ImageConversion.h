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
   * Schnittstelle für Konvertierungsfunktionen. Ein ImageConverter   
   * kann einen ImageBuffer genau eines Formats in einen ImageBuffer 
   * eines bestimmten anderen Formats (möglichst effizient) umwandeln. 
   * Für jede mögliche Formatkombination müssen getrennte Konverter 
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
     * Werden die Formate von src und dst nicht von dem Konverter unterstützt
     * wird eine TribotsException geworfen.
     */
    virtual void operator() (const ImageBuffer& src, ImageBuffer& dst) const
      throw (TribotsException)=0;
    /**
     * liefert das Format, das der Konverter auf Seiten der Quelle unterstützt
     */
    int getSourceFormat() { return srcFormat; }
    /**
     * liefert das Format, das der Konverter auf Seiten des Ziels unterstützt.
     */
    int getDestinationFormat() { return dstFormat; }

  protected:
    int srcFormat;   ///< Unterstütztes Format der Quelle
    int dstFormat;   ///< Unterstütztes Format des Ziels
  };

  /**
   * Zentrale, als Singleton implementierte Registrierung, bei der alle 
   * vorhandenen Konvertierungsfunktionen für ImageBuffer registriert und
   * unter Angabe des gewünschten Quellen- und Zielformats nachgeschlagen
   * werden können.
   */
  class ImageConversionRegistry
  {
  public:
    /**
     * liefert einen Zeiger auf die zentrale Registrierung.
     *
     * \attention Kein delete auf den übergebenen Zeiger ausführen!
     */
    static ImageConversionRegistry* getInstance();
    /**
     * Destruktor.
     */
    ~ImageConversionRegistry();

    /**
     * schlägt einen Konverter nach. Ist keine passende Implementierung
     * registriert, wird eine Exception geworfen.
     *
     * \param Format der Quelle
     * \param Format des Ziels
     */
    const ImageConverter* getConverter(int srcFormat, int dstFormat) 
      throw (TribotsException);
    /**
     * registriert eine Implementierung. Wurde vorher bereits eine
     * andere Implementierung für die gleiche Formatumwandlung registriert,
     * wird diese überschrieben.
     *
     * \todo Beim Überschreiben muss die alte Implementierung auch 
     *       gelöscht werden!!!
     */
    void registerConverter(ImageConverter* converter);

  protected:
    std::map<int, std::map<int, ImageConverter*> > converterMap;
    static ImageConversionRegistry* singleton;

    /**
     * geschützter Konstruktor, Zugriff auf das Singleton nur mittels 
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
   * Wandelt Puffer von YUV (wobei die Kanäle nach dem Schema UYV angeordnet 
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
