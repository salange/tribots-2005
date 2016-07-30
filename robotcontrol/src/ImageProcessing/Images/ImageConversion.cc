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

#include "ImageConversion.h"
#include "../ImageSource.h"
#include "../Image.h"

namespace Tribots {

  ImageConverter::ImageConverter(int srcFormat, int dstFormat)
    : srcFormat(srcFormat), dstFormat(dstFormat)
  {}

  YUV2RGB::YUV2RGB() : ImageConverter(ImageBuffer::FORMAT_YUV444,
				      ImageBuffer::FORMAT_RGB)
  {}

  void
  YUV2RGB::operator() (const ImageBuffer& src, ImageBuffer& dst) const
    throw (TribotsException)
  {
    if (src.format != ImageBuffer::FORMAT_YUV444 ||
	dst.format != ImageBuffer::FORMAT_RGB) {
      throw TribotsException(__FILE__
			     ": YUV2RGB: Source and target buffers do not "
			     "have the expected foramt");
    }
    register YUVTuple* yuv = 
      reinterpret_cast<YUVTuple*>(src.buffer);
    register RGBTuple* rgb =
      reinterpret_cast<RGBTuple*>(dst.buffer);

    register int y,u,v, r,g,b;

    for(register int i = src.width * src.height; i > 0;	i--) {
      y = yuv->y;
      u = yuv->u-128;
      v = yuv->v-128;

      PIXEL_YUV2RGB(y,u,v, r,g,b);

      rgb->r = static_cast<unsigned char>(r<0 ? 0 : (r>255 ? 255 : r));
      rgb->g = static_cast<unsigned char>(g<0 ? 0 : (g>255 ? 255 : g));
      rgb->b = static_cast<unsigned char>(b<0 ? 0 : (b>255 ? 255 : b));

      ++yuv; ++rgb;
    }
  }

  UYV2RGB::UYV2RGB() : ImageConverter(ImageBuffer::FORMAT_UYV,
				      ImageBuffer::FORMAT_RGB)
  {}

  void
  UYV2RGB::operator() (const ImageBuffer& src, ImageBuffer& dst) const
    throw (TribotsException)
  {
    if (src.format != ImageBuffer::FORMAT_UYV ||
	dst.format != ImageBuffer::FORMAT_RGB) {
      throw TribotsException(__FILE__
			     ": UYV2RGB: Source and target buffers do not "
			     "have the expected foramt");
    }
    register UYVTuple* uyv = 
      reinterpret_cast<UYVTuple*>(src.buffer);
    register RGBTuple* rgb =
      reinterpret_cast<RGBTuple*>(dst.buffer);

    register int y,u,v, r,g,b;

    for(register int i = src.width * src.height; i > 0;	i--) {
      y = uyv->y;
      u = uyv->u-128;
      v = uyv->v-128;

      PIXEL_YUV2RGB(y,u,v, r,g,b);

      rgb->r = static_cast<unsigned char>(r<0 ? 0 : (r>255 ? 255 : r));
      rgb->g = static_cast<unsigned char>(g<0 ? 0 : (g>255 ? 255 : g));
      rgb->b = static_cast<unsigned char>(b<0 ? 0 : (b>255 ? 255 : b));

      ++uyv; ++rgb;
    }
  }

  UYVY2YUV::UYVY2YUV() : ImageConverter(ImageBuffer::FORMAT_YUV422,
					ImageBuffer::FORMAT_YUV444)
  {}

  void
  UYVY2YUV::operator() (const ImageBuffer& src, ImageBuffer& dst) const
    throw (TribotsException)
  {
    if (src.format != ImageBuffer::FORMAT_YUV422 ||
	dst.format != ImageBuffer::FORMAT_YUV444) {
      throw TribotsException(__FILE__
			     ": UYVY2YUV: Source and target buffers do not "
			     "have the expected foramt");
    }
    register UYVYTuple* uyvy = 
      reinterpret_cast<UYVYTuple*>(src.buffer);
    register YUVTuple* yuv =
      reinterpret_cast<YUVTuple*>(dst.buffer);

    for(register int i = src.width * src.height / 2; i > 0; i--) {
      
      yuv->y = uyvy->y1;
      yuv->u = uyvy->u;
      yuv->v = uyvy->v;

      ++yuv;

      yuv->y = uyvy->y2;
      yuv->u = uyvy->u;
      yuv->v = uyvy->v;

      ++uyvy; ++yuv;
    }
  }

  UYVY2RGB::UYVY2RGB() : ImageConverter(ImageBuffer::FORMAT_YUV422,
					ImageBuffer::FORMAT_RGB)
  {}

  void
  UYVY2RGB::operator() (const ImageBuffer& src, ImageBuffer& dst) const
    throw (TribotsException)
  {
    if (src.format != ImageBuffer::FORMAT_YUV422 ||
	dst.format != ImageBuffer::FORMAT_RGB) {
      throw TribotsException(__FILE__
			     ": UYVY2RGB: Source and target buffers do not "
			     "have the expected format");
    }
    register UYVYTuple* uyvy = 
      reinterpret_cast<UYVYTuple*>(src.buffer);
    register RGBTuple* rgb =
      reinterpret_cast<RGBTuple*>(dst.buffer);

    register int y1,y2,u,v, r,g,b;

    for(register int i = src.width * src.height / 2; i > 0; i--) {
      y1 = uyvy->y1;
      y2 = uyvy->y2;
      u  = uyvy->u-128;
      v  = uyvy->v-128;

      PIXEL_YUV2RGB(y1,u,v, r,g,b);

      rgb->r = static_cast<unsigned char>(r<0 ? 0 : (r>255 ? 255 : r));
      rgb->g = static_cast<unsigned char>(g<0 ? 0 : (g>255 ? 255 : g));
      rgb->b = static_cast<unsigned char>(b<0 ? 0 : (b>255 ? 255 : b));

      ++rgb;

      PIXEL_YUV2RGB(y2,u,v, r,g,b);

      rgb->r = static_cast<unsigned char>(r<0 ? 0 : (r>255 ? 255 : r));
      rgb->g = static_cast<unsigned char>(g<0 ? 0 : (g>255 ? 255 : g));
      rgb->b = static_cast<unsigned char>(b<0 ? 0 : (b>255 ? 255 : b));

      ++uyvy; ++rgb;
    }
  }

  ImageConversionRegistry* 
  ImageConversionRegistry::singleton = 0;

  ImageConversionRegistry* 
  ImageConversionRegistry::getInstance()
  {
    if (singleton == 0) {
      singleton = new ImageConversionRegistry();
    }
    return singleton;
  }
  
  ImageConversionRegistry::ImageConversionRegistry()
  {
    // you may add your implementation here or use registerConverter...

    registerConverter(new YUV2RGB());
    registerConverter(new UYV2RGB());
    registerConverter(new UYVY2YUV());
    registerConverter(new UYVY2RGB());
  }

  ImageConversionRegistry::~ImageConversionRegistry()
  {
    for (std::map<int, std::map<int, ImageConverter*> >::iterator it = 
	   converterMap.begin(); 
	 it != converterMap.end(); 
	 it++) {
      
      for (std::map<int, ImageConverter*>::iterator innerIt = 
	     it->second.begin(); 
	   innerIt != it->second.end(); 
	   innerIt++) {

	delete innerIt->second;       // delete ImageConverter object
      }
      it->second.clear();             // empty the inner map(s)
    }
    converterMap.clear();             // empty the outer map
  }

  void
  ImageConversionRegistry::registerConverter(ImageConverter* converter)
  {
    converterMap[converter->getSourceFormat()]
      [converter->getDestinationFormat()] = converter;
  }

  const ImageConverter* 
  ImageConversionRegistry::getConverter(int srcFormat, int dstFormat) 
    throw(TribotsException)
  {
    if (converterMap[srcFormat].find(dstFormat) == 
	converterMap[srcFormat].end()) {
      throw TribotsException(__FILE__ 
			     ": The requested image converter is not "
			     "implemented.");
    }
    return converterMap[srcFormat].find(dstFormat)->second;
  }

}
