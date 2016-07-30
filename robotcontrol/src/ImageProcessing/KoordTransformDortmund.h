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

#ifndef _KoordTransformDortmund_h_
#define _KoordTransformDortmund_h_

#include <string>
#include <vector>
#include "../../ImageProcessing/Image2WorldMapping.h"

namespace Tribots {

  class Image2WorldDortmund : public CoordinateMapping {
  public:
    Image2WorldDortmund(std::string filename);
    virtual ~Image2WorldDortmund();

    virtual Vec map(const Vec& vec) const;

    Image2WorldMapping* convert() const;

    void setMiddle(const Vec& middle);
    Vec getMiddle() const;

  protected:
    int width, height;
    Vec middle;
    int shift;

    std::vector<double> realDistances;
    std::vector<std::vector< double> > imageDistances;
    std::vector<double> angleCorrections;
  };
  
};

#endif
