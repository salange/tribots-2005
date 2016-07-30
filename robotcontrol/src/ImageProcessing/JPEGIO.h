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

#ifndef _JPEGIO_h_
#define _JPEGIO_h_

#include "ImageIO.h"

namespace Tribots {

  /** encapsulates libjpeg data structs to remove libjpeg headers from this
   *  public header. */
  class LibJPEGData; 

  class JPEGIO : public ImageIO {
  public:
    JPEGIO(int quality=50);
    virtual ~JPEGIO();

    virtual void write(const Image& image, ostream& out) const
      throw(TribotsException);
    
    virtual Image* read(Image* image, istream& in) const
      throw(TribotsException);

    virtual std::string getDefEnding() const { return ".jpg"; }
    virtual std::string getTypeId() const { return "JPEG"; }
  protected:
    LibJPEGData *lj;
  };

}

#endif
