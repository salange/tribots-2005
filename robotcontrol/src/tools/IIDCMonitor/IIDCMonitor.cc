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

#include <iostream>

#include "../../tools/components/imagewidget.h"
#include "../../ImageProcessing/ImageSource.h"
#include "../../ImageProcessing/ImageSources/IIDC.h"
#include "../../ImageProcessing/ImageBuffer.h"
#include "../../ImageProcessing/Image.h"
#include "../../ImageProcessing/Images/RGBImage.h"
#include "../../ImageProcessing/Images/YUVImage.h"
#include "../../Fundamental/ConfigReader.h"
#include "../../Fundamental/Time.h"

using namespace Tribots;
using namespace std;

// Default values:

#define FRAMERATE FRAMERATE_15
#define FORMAT    MODE_640x480_YUV422
#define DEVICE    "/dev/video1394"
#define PORT      0


// Mapping of configuration strings to libdc defines
typedef struct {
  string key;
  int value;
} KeyValueEntry;

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

int main(int argc, char* argv[])
{
  if (argc>2) {
    cerr << "Aufruf: " << argv[0] << " [Konfigurationsdatei]" << endl;
    return -1;
  }

  int framerate = FRAMERATE;
  int format = FORMAT;
  string device = DEVICE;
  int port = PORT;

  try{

    if (argc>1) {          // optional: read settings from configuration file
      ConfigReader config(0);
      config.append_from_file(argv[1]);

      string formatStr;
      string framerateStr;

      if (config.get("IIDC::device_name", device) <= 0) {      // device_name
	throw InvalidConfigurationException("IIDC::device_name");
      }
      if (config.get("IIDC::port", port) <= 0) {               // port
	throw InvalidConfigurationException("IIDC::port");
      }

      if (config.get("IIDC::framerate", framerateStr) <= 0) {  // framerate
	throw InvalidConfigurationException("IIDC::framerate");
      }

      if (config.get("IIDC::mode", formatStr) <= 0) {          // mode
	throw InvalidConfigurationException("IIDC::mode");
      }

      for (int i=0; i < mode_types_size; i++) {
	if (mode_types[i].key == formatStr) {
	  format = mode_types[i].value;
	}
      }
      if (format < 0) {             // no match
	throw InvalidConfigurationException("IIDC::mode");
      }
      for (int i=0; i < framerate_types_size; i++) {
	if (framerate_types[i].key == framerateStr) {
	  framerate = framerate_types[i].value;
	}
      }
      if (framerate < 0) {         // no match
	throw InvalidConfigurationException("IIDC::framerate");
      }
    }

    // initialize camera
    ImageSource* imgSrc = IIDC::getCamera(device.c_str(), 
					  port, 
					  framerate,
					  format);
    Image* image = new YUVImage(640,480);
    Image* newImage = image;
    
    QApplication app(argc, argv);
    CImageWidget widget(*image);
    app.setMainWidget(&widget);
    widget.show();
    
    QObject::connect( &app, SIGNAL(lastWindowClosed()),
    &app, SLOT(quit()) );

    while (app.mainWidget()->isVisible()) {

      Time start();
      ImageBuffer buffer = imgSrc->getImage();     // get next image buffer

      newImage = new YUVImage(buffer);
      widget.setImage(*newImage);
    
      if (image) delete image;
      image = newImage;

      app.processEvents();
    }  

    if (newImage) delete newImage;

  }catch(Tribots::InvalidConfigurationException& e){
    cerr << e.what() << "\n\r" << flush;
  }catch(Tribots::TribotsException& e){
    cerr << e.what() << "\n\r" << flush;
  }catch(std::exception& e){
    cerr << e.what() << "\n\r" << flush;
  }
  return 0;
}
