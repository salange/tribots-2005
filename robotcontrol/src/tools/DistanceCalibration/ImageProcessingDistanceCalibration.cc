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


#include "ImageProcessingDistanceCalibration.h"
#include <iostream>

#include "../../ImageProcessing/ScanLines.h"
#include "../../ImageProcessing/ImageProcessingFactory.h"
#include "../../ImageProcessing/Image.h"
#include "../../ImageProcessing/RobotMask.h"
#include "../../ImageProcessing/KoordTransformDortmund.h"
#include "../../Structures/Journal.h"
#include "../../WorldModel/WorldModel.h"


// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public Tribots::ImageProcessingBuilder {
    static Builder the_builder;
  public:
    Builder () {
      Tribots::ImageProcessingFactory::get_image_processing_factory ()->sign_up (std::string("DistanceCalibration"), this);
    }
    Tribots::ImageProcessingType* get_image_processing (const std::string&, const Tribots::ConfigReader& reader, Tribots::ImageProcessingType*) throw (Tribots::TribotsException,std::bad_alloc) {
      return new TribotsTools::ImageProcessingDistanceCalibration (reader);
    }
  };
  Builder the_builder;
}

using namespace std;
using namespace Tribots;
using namespace TribotsTools;



ImageProcessingDistanceCalibration::ImageProcessingDistanceCalibration(const ConfigReader& config) {
  int innerRadius, outerRadius;
  int nScanLines;
  string robotMaskFilename="";
  
  if (config.get("ScanLine::robot_mask_file", robotMaskFilename) < 0) {
    throw InvalidConfigurationException("ScanLine::robot_mask_filename");
  }   
  if (config.get("ScanLine::image_center_x", image_center.x) <= 0) {
    throw InvalidConfigurationException("ScanLine::image_center_x");
  }   
  if (config.get("ScanLine::image_center_y", image_center.y) <= 0) {
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
  if (config.get("ScanLine::image2world_marker_file", markerFile) <=0) {
    throw 	InvalidConfigurationException("ScanLine::image2world_marker_file");
  }

  imgProd = new MultiImageProducer(config);
  colClass = new ColorClassInfoList(config);
  colClass->classList[COLOR_BLUE]->findTransitions=true;
  colClass->classList[COLOR_BALL]->findTransitions=true;
  Image* image = imgProd->nextImage();
  
  if (robotMaskFilename != "") {
    RobotMask::load(robotMaskFilename);
  }
  else {
    JWARNING("No robot mask specified!");
  }
  
  ScanLines* lines = new ScanLines(image_center,
				   innerRadius,
				   outerRadius,
				   image->getWidth(), image->getHeight(),
				   nScanLines);
  scanner = new LineScanner(lines, colClass);
  scanResults = new ScanResultList(colClass->classList.size());
  
  Image2WorldDortmund dortmund(markerFile);
  if (static_cast<int>(image_center.x) != dortmund.getMiddle().x ||
      static_cast<int>(image_center.y) != dortmund.getMiddle().y) {
    JWARNING("Image center specified in config files and image center "
	     "specified in distance marker file do not match. " 
	     "It is likely that the mirror has moved slightly since "
	     "the last calibration procedure.\n"
	     "I will use the image center specified "
	     "in the configuration files for both, coordinate "
	     "transformation and distribution of scan lines. Please notice "
	     "that the precision of self localization and localization "
	     "of objects may suffer.");
  }					    
  dortmund.setMiddle(image_center);//use the center from config file
  image2real = dortmund.convert();
  World2Image* w2i = new World2Image(markerFile);
  w2i->setMiddle(image_center);
  real2image = w2i;
  
  ballDetector = new BallDetector(image2real, static_cast<int>(image_center.x), static_cast<int>(image_center.y));
  distcal = new DistanceCalibration (image_center);

  delete image;
}

void ImageProcessingDistanceCalibration::process_image() throw (Tribots::BadHardwareException) {
  Image* image = imgProd->nextImage();
  
  scanResults->clear();     // remove results of the last frame
  scanner->scan(*image, scanResults);

  VisibleObjectList ball;
  ballDetector->searchBall(*image, scanResults->results[COLOR_BALL],
			   image->getTimestamp(), &ball);

  delete image;

  // Ab jetzt nach den Kalibriermerkmalen suchen
  if (MWM.get_game_state().in_game && MWM.get_game_state().refstate==Tribots::freePlay && ball.objectlist.size()>0) { // Ball gefunden
    Angle ballangle = (real2image->map (ball.objectlist[0].pos)-image_center).angle();
    distcal->search_markers (*scanResults, ballangle);
  }

}

ImageProcessingDistanceCalibration::~ImageProcessingDistanceCalibration() throw () {
  distcal->writeMarkerFile (markerFile);

  delete distcal;
  delete imgProd;
  delete colClass;
  delete scanResults;
  delete scanner;
  delete image2real;
  delete real2image;
  delete ballDetector;
}
