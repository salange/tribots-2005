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

#include "ImageProducer.h"

#include <string>
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

#ifndef INFO_OUT
#define INFO_OUT cerr
#endif

namespace Tribots {
  
  using namespace std;

  typedef struct {
    string key;
    int value;
  } KeyValueEntry;

  static const int image_buffer_types_size = 3;
  static KeyValueEntry image_buffer_types[image_buffer_types_size] = 
    { { "YUV",  ImageProducer::TYPE_YUVImage  },
      { "RGB",  ImageProducer::TYPE_RGBImage  },
      { "UYVY", ImageProducer::TYPE_UYVYImage } };

  static const int framerate_types_size = 5;
  static KeyValueEntry framerate_types[framerate_types_size] = 
    { { "FRAMERATE_1_875", FRAMERATE_1_875 },
      { "FRAMERATE_3_75",  FRAMERATE_3_75  },
      { "FRAMERATE_7_5",   FRAMERATE_7_5   },
      { "FRAMERATE_15",    FRAMERATE_15    },
      { "FRAMERATE_30",    FRAMERATE_30    } };

  static const int mode_types_size = 7;
  static KeyValueEntry mode_types[mode_types_size] = 
    { { "MODE_160x120_YUV444", MODE_160x120_YUV444 },
      { "MODE_320x240_YUV422", MODE_320x240_YUV422 },
      { "MODE_640x480_YUV411", MODE_640x480_YUV411 },
      { "MODE_640x480_YUV422", MODE_640x480_YUV422 },
      { "MODE_640x480_RGB",    MODE_640x480_RGB    },
      { "MODE_640x480_MONO",   MODE_640x480_MONO   },
      { "MODE_640x480_MONO16", MODE_640x480_MONO16 } };

  ImageProducer::ImageProducer(const ConfigReader& config) 
    throw(TribotsException)
    : imgSource(0), classifier(0), monitor(0), failCount(0), retriesMax(10), 
      imageType(TYPE_YUVImage), camera_delay (0)
  {
    ImageProducer::init(config, "Camera_default", "ImageProducer_default");    
  }


  ImageProducer::ImageProducer(const ConfigReader& config, 
			       const std::string& image_source, 
			       const std::string& image_handler)
    throw(TribotsException)
    : imgSource(0), classifier(0), monitor(0), failCount(0), retriesMax(10),
      imageType(TYPE_YUVImage), camera_delay (0)
  {
  	ImageProducer::init(config, image_source, image_handler);
  }
  
  void ImageProducer::init(const ConfigReader& config, 
			   const string& image_source, 
			   const string& image_handler) {
    fromFile = false;

    // get values from ImageProducer section  //////////////////////////
    int iTmp;
    string sTmp;
    string imgSourceType;
    string classifierType;
    int ret;

    ret = config.get((image_handler+"::max_retries").c_str() , iTmp);
    if (ret < 0) { // specified, but parse failed
      throw InvalidConfigurationException((
	image_handler+"::max_retries").c_str());
    }
    if (ret > 0) { // value specified?
      retriesMax = iTmp;
    }
    // TODO: Journal entry

    ret = config.get((image_handler+"::image_buffer_type").c_str(), sTmp);
    if (ret <= 0) { // specified, but parse failed
      throw InvalidConfigurationException((
        image_handler+"::image_buffer_type").c_str());
    }
    if (ret > 0) { // value specified?
      for (int i=0; i < image_buffer_types_size; i++) {
	if (image_buffer_types[i].key == sTmp) {
	  imageType = image_buffer_types[i].value;
	}
      }
    }
    // TODO: Journal entry

    if (config.get((image_handler+"::image_source_type").c_str(), 
		   imgSourceType) <= 0) {
      throw InvalidConfigurationException((
        image_handler+"::image_source_type").c_str());
    }

    // choose image source and initialize it ////////////////////////////


    if (imgSourceType == "CameraSource") { // selected digital camera (IIDC.h)
      int port;
      int framerate = -1;
      int mode = -1;
      string uid = "";
      string device_name;
      int isBlocking = -1;
      int delay = 0;

      if (config.get((image_source+"::device_name").c_str(), 
		     device_name) <= 0) { // device_name
	throw InvalidConfigurationException(
          (image_source+"::device_name").c_str());
      }
      if (config.get((image_source+"::port").c_str(), port) <= 0) {    // port
	throw InvalidConfigurationException((image_source+"::port").c_str());
      }
      if (config.get((image_source+"::uid").c_str(), uid) <= 0) {      // requested unit ID of the camera
        uid = "";
	INFO_OUT << "No UID number specified for camera "
		 << image_source << std::endl;
      }

      if (config.get((image_source+"::framerate").c_str(), sTmp) <= 0) {          // framerate
	throw InvalidConfigurationException((image_source+"::framerate").c_str());
      }

      for (int i=0; i < framerate_types_size; i++) {
	if (framerate_types[i].key == sTmp) {
	  framerate = framerate_types[i].value;
	}
      }
      if (framerate < 0) {         // no match
	throw InvalidConfigurationException((image_source+"::framerate").c_str());
      }

      if (config.get((image_source+"::mode").c_str(), sTmp) <= 0) {               // mode
	throw InvalidConfigurationException((image_source+"::mode").c_str());
      }
      for (int i=0; i < mode_types_size; i++) {
	if (mode_types[i].key == sTmp) {
	  mode = mode_types[i].value;
	}
      }
      if (mode < 0) {         // no match
	throw InvalidConfigurationException((image_source+"::mode").c_str());
      }


      if (config.get((image_source+"::blocking").c_str(), isBlocking) <= 0) {               // blocking or non-blocking
	throw InvalidConfigurationException((image_source+"::blocking").c_str());
      }

      if (config.get((image_source+"::delay").c_str(), delay) <= 0) {               // mode
	throw InvalidConfigurationException((image_source+"::delay").c_str());
      }
      
      imgSource = IIDC::getCamera(device_name.c_str(), port, framerate, mode, 
				  uid, (isBlocking==1), delay);
      if (!imgSource) {
	throw BadHardwareException(__FILE__
				   ": Failed ot initialize IIDC camera "
				   "with the given parameters and uid number");
      }
    }
    else if (imgSourceType == "FileSource") {
      if (config.get((image_source+"::filename").c_str(), sTmp) <= 0) {
	throw InvalidConfigurationException((image_source+"::filename").c_str());
      }
      fromFile = true;	//remember setting for desturctor
      imgSource = new FileSource(new PPMIO(), sTmp);
    }
    else {
      throw InvalidConfigurationException((image_handler+"::image_source_type").c_str());
    }

    ret = config.get((image_handler+"::color_classifier_type").c_str(), 
		     classifierType);
    if (ret < 0) {
      throw InvalidConfigurationException(
	      (image_handler+"::color_classifier_type").c_str());
    }
    else if (ret > 0) {
    // Choose ColorClassificator and initialize it ////////////////////////////
      if (classifierType == "YUVLut") {               // YUVLut
	classifier = new YUVLookupTable();
      }

      ret = config.get((image_handler+"::color_classifier_file").c_str(), 
		       sTmp);
      if (ret < 0) {
	throw InvalidConfigurationException(
	        (image_handler+"::color_classifier_file").c_str());
      }
      else if (ret > 0) {
	classifier->load(sTmp);
	classifier->save(("test"+image_handler+".lut").c_str());
      }
    }

    string monitorSection;

    if ((ret=config.get((image_handler+"::monitor").c_str(), 
			monitorSection)) < 0) {
      throw InvalidConfigurationException((image_handler+"::monitor").c_str());
    }
    else if (ret > 0) {
	string filenameBase;
	bool singleFile = false;
	int step=1;
	if (config.get((monitorSection+"::filename_base").c_str(), 
		       filenameBase) <= 0) {
	  throw InvalidConfigurationException(
	    (monitorSection+"::monitor_filename_base").c_str());
	}
	if (config.get((monitorSection+"::single_file").c_str(), 
		       singleFile) < 0) {
	  throw InvalidConfigurationException(
	    (monitorSection+"::single_file").c_str());
	}

	if (config.get((monitorSection+"::step").c_str(), step) <= 0) {
	  throw InvalidConfigurationException(
	    (monitorSection+"::step").c_str());
	}
	sTmp = "PPM";

	if (config.get((monitorSection+"::file_type").c_str(), sTmp) < 0) {
	  throw InvalidConfigurationException(
            (monitorSection+"::file_type").c_str());
	}
	if (sTmp=="PPM") {  // hierfuer eventuell auch eine factory
	  cout << "filename: "<< filenameBase << endl;
	  monitor = new FileMonitor(new PPMIO(), filenameBase, step,
				    singleFile);
	}
	else if (sTmp=="JPEG") {
	  int quality=50;
	  if (config.get("JPEGIO::quality", quality) <= 0) {
	    throw InvalidConfigurationException("JPEGIO::quality");
	  }
	  monitor = new FileMonitor(new JPEGIO(quality), filenameBase, step,
				    singleFile);
	}
	else {
	  throw InvalidConfigurationException(
            (monitorSection+"::file_type").c_str());
	}
    }
  }
  
  
  ImageProducer::~ImageProducer()
  {
    if (imgSource != 0) {
      if (fromFile) {
	delete imgSource;
      }
      else {
	IIDC::destroyCamera();// TODO: Ist das richtig? Welche Kamera zerstört?
      }
    }
    if (classifier != 0) {
      delete classifier;
    }
    if (monitor != 0) {
      delete monitor;
    }
  }

  Image*
  ImageProducer::nextImage() throw (BadHardwareException, TribotsException)
  {
    ImageBuffer buffer;
    do {
      try {
	buffer = imgSource->getImage();
      } catch (HardwareException& e) {
	// TODO: Journal: e.what()
	if (failCount++ >= retriesMax) {
	  string msg = 
	    __FILE__ 
	    ": There is a serious  hardware error. "
	    "A number of retries have not helped to solve the problem. ";
	  msg += string("\nOriginal Exception:\n:") + e.what(); 
	  throw BadHardwareException(msg.c_str());
	}
	else {
	  continue;
	}
      }
      failCount = 0;
    } while (failCount>0);  // repeat until success (or BadHardwareException)
    
    Time imageTimestamp;    // remember Timestamp for the image
    Image* image;
    
    try {
      switch (imageType) {
      case TYPE_RGBImage:  image = new RGBImage (buffer); break;
      case TYPE_UYVYImage: image = new UYVYImage(buffer); break;
      default:             image = new YUVImage (buffer); break;
      }
    } catch (TribotsException& e) {
      // TODO: Journal Ausgabe
      string msg =
	__FILE__
	": There has occured an error while creating an Image arround "
	"the ImageBuffer that has been received from the ImageSource. "
	"It is most likely, that there is no implementation of the "
	"needed conversion method to convert the format of the ImageBuffer "
	" to the format used by the selected Image class.\n\n"
	"Solution: Edit the configuration files and select another Image "
	"class that is compatible to the selected ImageSource.";
      msg += string("\n\nOriginal Exception: ") + e.what();

      throw new BadHardwareException(msg.c_str());
    }
    if (classifier != 0) {
      Time iTp = imageTimestamp;
      iTp.add_msec (-imgSource->getDelay()); // consider camera delay
      image->setTimestamp(iTp); // set timestamp
      image->setClassifier(classifier);
    }
    if (monitor) {
      monitor->monitor(*image, image->getTimestamp(), image->getTimestamp());
    }
    
    return image;
  }

  bool ImageProducer::setFramerate(int fps)
  {
    return imgSource->setFramerate(fps);
  }

} /* namespace Tribots */
  
