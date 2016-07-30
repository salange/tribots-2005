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

#ifndef _RGBImage_h_
#define _RGBImage_h_

#include "../Image.h"

namespace Tribots {

  /**
   * Implementierung des Image Interfaces. RGBImage verwendet den 
   * RGB Farbraum zur Repräsentierung der Bilddaten. Hat der übergebene
   * ImageBuffer ein anderes Format, so wird versucht diesen bei der
   * Konstruktion in das RGB Format zu konvertieren. 
   */
  class RGBImage : public Image
  {
  public:
    /**
     * Wickelt ein Image um den übergebenen ImageBuffer. Verwendet der
     * ImageBuffer ein anderes Format als das RGB Format, wird dieser
     * kopiert und konvertiert. Wird in der zentralen
     * Registrierung für Konvertierungsmethoden (ImageConversionRegistry)
     * keine passende Konvertierungsmethode gefunden, schlägt die 
     * Konstruktion mit einer Exception fehl.
     *
     * \param buffer die Bilddaten
     */
    RGBImage(const ImageBuffer& buffer);
    /**
     * Erzeugt ein neues Bild mit der angegebenen Größe.
     */
    RGBImage(int width, int height);
    virtual ~RGBImage();

    virtual Image* clone() const;

    virtual void getPixelYUV (int x, int y, YUVTuple* yuv) const;
    virtual void getPixelUYVY(int x, int y, UYVYTuple* uyvy) const;
    virtual void getPixelRGB (int x, int y, RGBTuple* rgb) const;

    virtual void setPixelYUV (int x, int y, const YUVTuple& yuv);
    virtual void setPixelUYVY(int x, int y, const UYVYTuple& uyvy);
    virtual void setPixelRGB (int x, int y, const RGBTuple& rgb);

    virtual unsigned char getPixelClass(int x, int y) const;

  protected:
    bool controlsBuffer; ///< true, wenn der interne Puffer der Instanz gehört 
  };

}

#endif
