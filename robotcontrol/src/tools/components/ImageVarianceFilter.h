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

#ifndef _ImageVarianceFilter_h_
#define _ImageVarianceFilter_h_

namespace Tribots {

  class Image;

  class ImageVarianceFilter {
  public:
    virtual ~ImageVarianceFilter() {;}

    virtual void initialize(int w, int h) = 0;
    virtual void initialize(const Image& image);

    virtual void add(const Image& image) =0;
    
    /**
     * Erzeugt eine Maske für alle Bereiche, deren Fluktuation unter dem
     * Schwellwert lag. Weiß (RGBTuple(255,255,255)) bedeutet "kaum Bewegung",
     * Schwarz (RGBTuple(255,255,255)) bedeutet "viel Bewegung".
     *
     * \param threshold Schwellwert, bis zu dem Pixel als "wenig Bewegung"
     *                  maskiert werden sollen. Der Wertebereich ist von der
     *                  Implementatin abhängig, sollte aber zwischen 0 und
     *                  255 liegen.
     */
    virtual Image* createMask(double threshold) =0;

    virtual int getNumImagesAdded() const =0;    
  };


  class ImageDistanceFilter : public ImageVarianceFilter {
  public:
    ImageDistanceFilter();
    virtual ~ImageDistanceFilter();

    virtual void initialize(int w, int h) ;

    virtual void add(const Image& image);
    
    virtual Image* createMask(double threshold);

    virtual int getNumImagesAdded() const;    

  protected:
    bool initialized;      ///< Bereits initialisiert?
    int numImagesAdded;    ///< Anzahl der verarbeiteten Bilder
    double* diff;          ///< Akkumulierte Differenzmatrix (Euklidisch)
    double* tmp;           ///< Matrix für Zwischenrechnungen
    Image* lastImage;      ///< letztes Bild, gegen das Differenz gebildet wird
    int w;
    int h;
  };

  class ImageMaxDistanceFilter : public ImageVarianceFilter {
  public:
    ImageMaxDistanceFilter();
    virtual ~ImageMaxDistanceFilter();

    virtual void initialize(int w, int h) ;

    virtual void add(const Image& image);
    
    virtual Image* createMask(double threshold);

    virtual int getNumImagesAdded() const;    

  protected:
    bool initialized;      ///< Bereits initialisiert?
    int numImagesAdded;    ///< Anzahl der verarbeiteten Bilder
    double* maxDiff;       ///< Maximale Differenz-Matrix (Euklidisch)
    Image* lastImage;      ///< letztes Bild, gegen das Differenz gebildet wird
    int w;
    int h;
  };


};  

#endif
