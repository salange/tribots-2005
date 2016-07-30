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

#include "ImageBuffer.h"
#include "Images/ImageConversion.h"

namespace Tribots {

  ImageBuffer::ImageBuffer(int width, int height, int format, 
			   unsigned char* buffer, int size)
    : width(width), height(height), format(format), buffer(buffer), size(size)
  {}

  void 
  ImageBuffer::convert(const ImageBuffer& src, ImageBuffer& dst)
    throw (TribotsException)
  {
    if (src.width != dst.width ||
	src.height != dst.height) {
      throw TribotsException(__FILE__
			     ": Source and target image buffer don't match");
    }

    if (src.format == dst.format) {
      if (src.size != dst.size) {
	throw TribotsException(__FILE__
			       ": Source and target image buffer have same "
			       "dimensions and format, but the size of their "
			       "buffers differ");
      }
      memcpy(dst.buffer, src.buffer, src.size * sizeof(src.buffer[0]));
    }
    else {
      // lookup the implementation and convert source to target format
      const ImageConverter* convert =    
	ImageConversionRegistry::getInstance()->getConverter(src.format,
							     dst.format);
      (*convert)(src, dst);    
    }
  }

};
