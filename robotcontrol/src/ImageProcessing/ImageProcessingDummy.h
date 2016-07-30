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


#ifndef tribots_image_processing_dummy_h
#define tribots_image_processing_dummy_h

#include "ImageProcessingType.h"
#include "../WorldModel/WorldModel.h"

namespace Tribots {

  /** Dummy-Implementierung der Bildverarbeitung; tut nichts */
  class ImageProcessingDummy:public ImageProcessingType {
  public:
    void process_image () throw ();
  };

}

#endif

