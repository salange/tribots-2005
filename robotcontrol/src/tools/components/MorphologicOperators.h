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

#ifndef _MorphologicOperators_h_
#define _MorphologicOperators_h_

#include "../../ImageProcessing/Image.h"

namespace Tribots {

  /**
   * Interface f�r Operatoren, die ein Bild bearbeiten und ein neues, 
   * ver�ndertes Bild zur�ckliefern.
   */
  class ImageOperator {
  public:
    /**
     * Virtueller Destrkutor.
     */
    virtual ~ImageOperator() {};
    
    /**
     * Liefert ein neues, ver�ndertes Bild. Der Speicher geht in den Besitz
     * des Aufrufers �ber.
     *
     * \param image Ursprungsbild
     * \return Ein neu erzeugtes Image, das das Ergebnis enth�lt.
     */
    virtual Image* operator() (const Image&) const =0;
  };

  /**
   * Dilatationsoperator. Vergr��ert weisse Fl�chen um die angrenzenden Pixel.
   */
  class Dilatation : public ImageOperator {
  public:
    Dilatation(int size = 3);
    
    virtual ~Dilatation();
    virtual Image* operator() (const Image&) const;

  protected:
    int size;
  };

  /**
   * Erosionsoperator. Verkleinert weisse Fl�chen um Randpixel.
   */
  class Erosion : public ImageOperator {
  public:
    Erosion(int size = 3);
    
    virtual ~Erosion();
    virtual Image* operator() (const Image&) const;

  protected:
    int size;
  };


}

#endif
