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

#include "../../tools/components/ImageProcessingMonitor.h"
#include "../../tools/components/imagewidget.h"
#include "../../ImageProcessing/MultiImageProducer.h"
#include "../../ImageProcessing/Image.h"
#include "../../ImageProcessing/Images/RGBImage.h"
#include "../../ImageProcessing/Images/YUVImage.h"
#include "../../Fundamental/ConfigReader.h"
#include "../../ImageProcessing/ScanLines.h"
#include "../../ImageProcessing/Image2WorldMapping.h"
#include "../../ImageProcessing/RobotMask.h"
#include "../../Fundamental/Vec.h"

using namespace Tribots;
using namespace std;

#define FRAMERATE FRAMERATE_15
#define FORMAT    MODE_640x480_YUV422
#define DEVICE    "/dev/video1394"
#define PORT      0

#define WARP_WIDTH 10000
#define WARP_HEIGHT 10000

int main(int argc, char* argv[])
{
  bool useLut = false;
  bool deWarp = false;

  if (argc<=1) {
    cerr << "Aufruf: " << argv[0] << " Konfigurationsdatei" << endl;
    return -1;
  }
  
  if (argc > 2) {
    if (string(argv[2]) == "c") {      // show segmentation
      useLut = true;
    }
    else if (string(argv[2]) == "w") { // de-warp
      deWarp = true;
    }
  }

  try{
    ConfigReader config(2);
    config.append_from_file(argv[1]);

    MultiImageProducer* producer = new MultiImageProducer(config);
    Image* image = producer->nextImage();

    ColorClassInfoList colClass(config);
    

    string robotMaskFilename="";

    if (config.get("ScanLine::robot_mask_file", robotMaskFilename) < 0) {
      throw InvalidConfigurationException("ScanLine::robot_mask_filename");
    }   
    if (robotMaskFilename != "") {
      cerr << "Loading robot mask file: " << robotMaskFilename << endl;
      RobotMask::load(robotMaskFilename);
    }

    int centerX, centerY;
    int innerRadius, outerRadius;
    int nScanLines;

    if (config.get("ScanLine::image_center_x", centerX) <= 0) {
      throw InvalidConfigurationException("ScanLine::image_center_x");
    }   
    if (config.get("ScanLine::image_center_y", centerY) <= 0) {
      throw InvalidConfigurationException("ScanLine::image_center_y");
    }   
    if (config.get("ScanLine::inner_scan_radius", innerRadius) <= 0) {
      throw InvalidConfigurationException("ScanLine::inner_scan_radius");
    }   
    if (config.get("ScanLine::outer_scan_radius", outerRadius) <= 0) {
      throw InvalidConfigurationException("ScanLine::outer_scan_radius");
    }   
    if (config.get("ScanLine::number_of_scanlines", nScanLines) <= 0) {
      throw InvalidConfigurationException("ScanLine::number_of_scanlines");
    }   
    

    ScanLines* lines = new ScanLines(Vec(centerX,centerY), 
				     innerRadius, outerRadius,
				     640, 480, nScanLines);
    LineScanner scanner(lines, &colClass);

    QApplication app(argc, argv);
    ImageProcessingMonitor widget(config, *image);
    if (!deWarp) {
      widget.setScanLines(lines);
    }

    app.setMainWidget(&widget);
    widget.show();
    
    QObject::connect( &app, SIGNAL(lastWindowClosed()),
		      &app, SLOT(quit()) );

    Image* warpImage=0;
    Image* regionMap=0;
    if (useLut) {
      regionMap = new RGBImage(image->getWidth(), image->getHeight());
    }
    if (deWarp) {
      warpImage = new YUVImage(WARP_WIDTH / 20, WARP_HEIGHT / 20);
    }

    string mapfile;
    if (config.get("ScanLine::image2world_marker_file", mapfile) <= 0) {
      throw InvalidConfigurationException("main::"
					  "image2world_marker_file");
    }
    World2Image* mapping = new World2Image(mapfile);    

    ScanResultList results(colClass.classList.size());
    
    while (app.mainWidget()->isVisible()) {
      results.clear();

      Image* newImage = producer->nextImage();
      
      if (!deWarp) {
	scanner.scan(*newImage, &results);
	widget.setScanResults(&results);
      }

      if (useLut) {
	int c;
	for (int x=0; x < newImage->getWidth(); x++) {
	  for (int y=0; y < newImage->getHeight(); y++) {
	    c = newImage->getPixelClass(x,y);
	    regionMap->setPixelRGB(x,y, colClass.classList[c]->color);
	  }
	}
	widget.setImage(*regionMap);
      }
      else if (deWarp) {

	YUVTuple yuv;

	for (int x=0; x < warpImage->getWidth(); x++) {
	  for (int y=0; y < warpImage->getHeight(); y++) {
	    Vec real((x-warpImage->getWidth()/2)*20, 
		     (y-warpImage->getHeight()/2)*20);

	    Vec img = mapping->map(real); // image Coordinates
	    if ((int)img.x < 0 || (int)img.x > newImage->getWidth() ||
		(int)img.y < 0 || (int)img.y > newImage->getHeight()) {
	      cerr << "Error mapping world coords to image. Real: " 
		   << real.x << ", " << real.y << " Image: "
		   << img.x << ", " << img.y << endl;
	      exit(1);
	    }
	    newImage->getPixelYUV((int)img.x, (int)img.y, &yuv);
	    warpImage->setPixelYUV(x,y, yuv);
	  }
	}
	widget.setImage(*warpImage);
      }
      else {
	widget.setImage(*newImage);
      }

      delete image;
      image = newImage;

      app.processEvents();
    }  

    if (regionMap != 0) {
      delete regionMap;
    }

  }catch(Tribots::InvalidConfigurationException& e){
    cerr << e.what() << "\n\r" << flush;
  }catch(Tribots::TribotsException& e){
    cerr << e.what() << "\n\r" << flush;
  }catch(std::exception& e){
    cerr << e.what() << "\n\r" << flush;
  }
  return 0;
}
