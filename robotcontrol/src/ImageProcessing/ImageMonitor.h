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

#ifndef _imagemonitor_h_
#define _imagemonitor_h_

#include "../Fundamental/Time.h"
#include "Image.h"

namespace Tribots {


  class ImageMonitor {
  public:
    virtual ~ImageMonitor() {};
    virtual void monitor(const Image& image, 
			 const Time& available, const Time& created)=0;
  };

}

#endif
