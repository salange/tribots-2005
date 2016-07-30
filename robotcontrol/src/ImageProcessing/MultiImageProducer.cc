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

#include "MultiImageProducer.h"

#include <string>
#include <iostream>
#include <libdc1394/dc1394_control.h>

#include "ImageSource.h"
#include "ImageSources/IIDC.h"
#include "ImageSources/FileSource.h"
#include "ImageBuffer.h"
#include "Images/RGBImage.h"
#include "Images/YUVImage.h"
#include "Images/UYVYImage.h"
#include "ColorClassifier.h"
#include "YUVLookupTable.h"
#include "../Fundamental/Time.h"
#include "ImageIO.h"
#include "ImageMonitors/FileMonitor.h"
#include "JPEGIO.h"

namespace Tribots {

  using std::string;

  MultiImageProducer::MultiImageProducer(const ConfigReader& config)
  {
    // get values from MultiImageProducer section  //////////////////////////

    int ret;
    int num_sources;


    ret = config.get("MultiImageProducer::image_sources", num_sources);
    if (ret <= 0) { // specified, but parse failed
      throw InvalidConfigurationException("MultiImageProducer::image_sources");
    }


      // initialize all image producers
    for(int i=0; i<num_sources; i++) {
	char buffer[3];
	sprintf(buffer,"%d",i);
	string suffix(buffer);

	string image_source;
	string image_handler;

	ret = config.get(("MultiImageProducer::image_source_" + suffix).c_str(), image_source);
	if (ret <= 0) { // specified, but parse failed
     		throw InvalidConfigurationException(("MultiImageProducer::image_source_" + suffix).c_str());
    	}
	ret = config.get(("MultiImageProducer::image_handler_" + suffix).c_str(), image_handler);
	if (ret <= 0) { // specified, but parse failed
     		throw InvalidConfigurationException(("MultiImageProducer::image_handler_" + suffix).c_str());
    	}

	producers.push_back(new ImageProducer(config, image_source, image_handler));
    }


  }

  MultiImageProducer::~MultiImageProducer()
  {
     for(unsigned int i=0; i<producers.size(); i++)
        delete producers[i];
  }


  Image*
  MultiImageProducer::nextImage() throw (BadHardwareException)
  {
     return nextImage(0);
  }


  Image*
  MultiImageProducer::nextImage(int cam_id) throw (BadHardwareException)
  {
	if((int)producers.size() > cam_id)
	   return (producers[cam_id])->nextImage();
	else
	   throw new BadHardwareException("MultiImageProducer::nextImage(int) : Tried to aquire an image from an unknown camera id.");

  }
} /* namespace Tribots */

