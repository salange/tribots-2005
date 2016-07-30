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

#include "ScanLineImageProcessing.h"

#include <string>
#include <iostream>
#include <sstream>

#include "ScanLines.h"
#include "ImageProcessingFactory.h"
#include "../Fundamental/Vec.h"
#include "../Structures/Journal.h"
#include "LineScanning.h"
#include "LineDetector.h"
#include "FieldMapper.h"
#include "GoalDetector.h"
#include "ObstacleDetector.h"
#include "SimpleObstacleDetector.h"
#include "Image.h"
#include "Image2WorldMapping.h"
#include "BallDetector.h"
#include "../Structures/Journal.h"
#include "KoordTransformDortmund.h"
#include "RobotMask.h"
#include "../WorldModel/WorldModel.h"
#include "Painter.h"


// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public Tribots::ImageProcessingBuilder {
    static Builder the_builder;
  public:
    Builder () {
      Tribots::ImageProcessingFactory::get_image_processing_factory ()->sign_up (std::string("ScanLine"), this);
    }
    Tribots::ImageProcessingType* get_image_processing (const std::string&, const Tribots::ConfigReader& reader, Tribots::ImageProcessingType*) throw (Tribots::TribotsException,std::bad_alloc) {
      return new Tribots::ScanLineImageProcessing (reader);
    }
  };
  Builder the_builder;
}

namespace Tribots {

  using namespace std;

  ScanLineImageProcessing::ScanLineImageProcessing(const ConfigReader& config)
    throw (InvalidConfigurationException, HardwareException,
	   BadHardwareException, TribotsException)
    : producerTime(0), scannerTime(0), lineDetectorTime(0), 
      goalDetectorTime(0), ballDetectorTime(0), obstacleDetectorTime(0),
      fieldMapperTime(0),
      cycles(0), report_computation_time(false), 
      report_computation_time_gnuplot(false),
      rawImageRequested(false), processedImageRequested(false),
      requestedImage(0), imageCollected(false)
  {
    int innerRadius, outerRadius;
    int nScanLines;
    string markerFile="";
    double minWidth, maxWidth;
    string imageLogFilename;
    string obstDectName ="";

    string robotMaskFilename="";
    bool checkForGreen = false;
    bool use2Steps = false;
    bool cutOutsideField = false;
    bool useHalfField = false;
    bool useMaximalDistance = false;
    double maximalDistance;
    bool useFieldMapper = false;
       
    if (config.get("ScanLine::robot_mask_file", robotMaskFilename) < 0) {
      throw InvalidConfigurationException("ScanLine::robot_mask_filename");
    }   
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
    if (config.get("ScanLine::image2world_marker_file", markerFile) <=0) {
      throw 
	InvalidConfigurationException("ScanLine::image2world_marker_file");
    }   
    if (config.get("ScanLine::min_line_width", minWidth) <= 0) {
      throw InvalidConfigurationException("ScanLine::min_line_width");
    }   
    if (config.get("ScanLine::max_line_width", maxWidth) <= 0) {
      throw InvalidConfigurationException("ScanLine::max_line_width");
    }   
    if (config.get("ScanLine::obstacle_detector", obstDectName) < 0) {
      throw InvalidConfigurationException("ScanLine::obstacle_detector");
    }   
    if (config.get("ScanLine::use_two_steps", use2Steps) < 0) {
      throw InvalidConfigurationException("ScanLine::use_two_steps");
    }   
    if (config.get("ScanLine::cut_lines_outside_field", cutOutsideField) < 0) {
      throw InvalidConfigurationException("ScanLine::cut_lines_outside_field");
    }  
    if (config.get("ScanLine::check_for_field", checkForGreen) < 0) {
      throw InvalidConfigurationException("ScanLine::check_for_field");
    }  
    if (config.get("ScanLine::use_maximal_distance", useMaximalDistance) < 0) {
      throw InvalidConfigurationException("ScanLine::use_maximal_distance");
    }  
    if (useMaximalDistance) {
      if (config.get("ScanLine::maximal_distance", maximalDistance) <= 0) {
	throw InvalidConfigurationException("ScanLine::maximal_distance");
      }
    }
    if (config.get("ScanLine::use_half_field", useHalfField) < 0) {
      throw InvalidConfigurationException("ScanLine::use_half_field");
    }  
    if (config.get("report_computation_time", report_computation_time) < 0) {
      throw InvalidConfigurationException("report_computation_time");
    }  
    if (config.get("ScanLine::useFieldMapper", useFieldMapper) < 0) {
      throw InvalidConfigurationException("ScanLine::useFieldMapper");
    }  
    if (config.get("report_computation_time_gnuplot", 
		   report_computation_time_gnuplot) < 0) {
      throw InvalidConfigurationException("report_computation_time_gnuplot");
    }  

    imgProd = new MultiImageProducer(config);
    colClass = new ColorClassInfoList(config);
    Image* image = imgProd->nextImage();

    if (robotMaskFilename != "") {
      RobotMask::load(robotMaskFilename);
    }
    else {
      JWARNING("No robot mask specified!");
    }
        
    ScanLines* lines = new ScanLines(Vec(centerX,
					 centerY), 
				     innerRadius,
				     outerRadius,
				     image->getWidth(), image->getHeight(),
				     nScanLines);
    scanner = new LineScanner(lines, colClass);
    scanResults = new ScanResultList(colClass->classList.size());

    Image2WorldDortmund dortmund(markerFile);
    if (centerX != dortmund.getMiddle().x ||
	centerY != dortmund.getMiddle().y) {
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
    dortmund.setMiddle(Vec(centerX, centerY));//use the center from config file
    image2real = dortmund.convert();

    World2Image* w2i = new World2Image(markerFile);
    w2i->setMiddle(Vec(centerX, centerY));
    real2image = w2i;
    fieldmapper=new FieldMapper();
    
    
    lineDetector = new LineDetector(image2real,fieldmapper, minWidth, maxWidth,
				    use2Steps, checkForGreen, 
				    cutOutsideField, useMaximalDistance,
				    maximalDistance, useHalfField, useFieldMapper);   
    
    goalDetector = new GoalDetector(centerX, centerY, real2image);

    ballDetector = new BallDetector(image2real, centerX, centerY);

    
    if (obstDectName == "SimpleObstacleDetector") {   
      obstacleDetector = new SimpleObstacleDetector(config, image2real,
						    real2image, 1);
    }
    else {
      JWARNING("No obstacle detector selected!");
      obstacleDetector = 0;
    }

    delete image;
  }

  /* lokale Hilfsmethode, die eine Modulnachricht erzeugt. */
  static const char* slip_prepareMsg(string module, double time)
  {
    string str;
    stringstream stream;
    stream << "average " << module << " time was " 
	   << (time / 1000.) << "ms" << endl;
    getline(stream, str);
    return str.c_str();
  }

  ScanLineImageProcessing::~ScanLineImageProcessing() throw()
  {  
    if (cycles > 0 && report_computation_time) {
      JMESSAGE(slip_prepareMsg("image production", 
			       static_cast<double>(producerTime) / cycles));
      JMESSAGE(slip_prepareMsg("line scanning", 
			       static_cast<double>(scannerTime) / cycles));
      JMESSAGE(slip_prepareMsg("ball detecting", 
			       static_cast<double>(ballDetectorTime)/ cycles));
      JMESSAGE(slip_prepareMsg("goal detecting", 
			       static_cast<double>(goalDetectorTime)/ cycles));
      JMESSAGE(slip_prepareMsg("line detecting", 
			       static_cast<double>(lineDetectorTime)/ cycles));
      JMESSAGE(slip_prepareMsg("field mapper", 
			       static_cast<double>(fieldMapperTime)/ cycles));
      JMESSAGE(slip_prepareMsg("obstacle detecting", 
      		       static_cast<double>(obstacleDetectorTime) 
      		       / cycles));
    }      

    delete ballDetector;
    delete goalDetector;
    delete lineDetector;
    delete obstacleDetector;
    delete fieldmapper;
    //delete cc;
    delete image2real;
    if (real2image)
      delete real2image;
    delete scanner;
    delete scanResults;
    delete colClass;
    delete imgProd;
  }

  void 
  ScanLineImageProcessing::process_image() 
    throw (Tribots::BadHardwareException)
  {

    Time time;
    Image* image = imgProd->nextImage();
    producerTime += time.elapsed_usec();
    stringstream gnuplot;

    if (rawImageRequested || processedImageRequested) {
      requestedImage = image->clone();
      rawImageRequested = false;
    }
    
    if (processedImageRequested) {          // visualize global information?
      visualizeCommonInfo();
    }

    gnuplot << "IMAGEPROCESSING_ELAPSED_GNUPLOT: "
	    << WorldModel::get_main_world_model().get_game_state().cycle_num
	    << " ";

    //Field Mapper
    time.update();
    fieldmapper->buildFieldMap(*image);
    fieldMapperTime+= time.elapsed_usec();
    if (report_computation_time_gnuplot) 
      gnuplot  << time.elapsed_usec() <<" ";//Field Mapper build Fieldmap ready
      
    if (processedImageRequested) 
      scanner->setVisualize(requestedImage);
    time.update();
    scanResults->clear();     // remove results of the last frame
    scanner->scan(*image, scanResults);
    scannerTime += time.elapsed_usec();
    if (report_computation_time_gnuplot) 
      gnuplot  << time.elapsed_usec() << " ";
    lastSeenObjects.objectlist.clear();  // delete old objects

    if (processedImageRequested)
      lineDetector->setVisualize(requestedImage);
    time.update();
    lineDetector->searchLines(scanResults->results[COLOR_LINE],
			      image->getTimestamp(), &lastSeenObjects); 
       // side effect: adds detected line transitions to world model
    lineDetectorTime += time.elapsed_usec();
    if (report_computation_time_gnuplot) 
      gnuplot  << time.elapsed_usec() << " ";

    if (processedImageRequested)
      goalDetector->setVisualize(requestedImage);
    time.update();
    goalDetector->searchGoals(*image, 
			      image->getTimestamp(), &lastSeenObjects);
    goalDetectorTime += time.elapsed_usec();
    if (report_computation_time_gnuplot) 
      gnuplot  << time.elapsed_usec() << " ";
    
    if (processedImageRequested)
      ballDetector->setVisualize(requestedImage);
    time.update();
    ballDetector->searchBall(*image, scanResults->results[COLOR_BALL],
			     image->getTimestamp(), &lastSeenObjects);
    ballDetectorTime += time.elapsed_usec();
    if (report_computation_time_gnuplot) 
      gnuplot  << time.elapsed_usec() << " ";
    time.update();
    // der obstacledetector geht davon aus, dass Randpixel eine andere 
    // farbe haben...

    if (obstacleDetector) { // if obstacle Detector selected
      try {
	if (processedImageRequested) 
	  obstacleDetector->setVisualize(requestedImage);
	obstacleDetector->searchObstacles(*image, 
					  scanResults->results[COLOR_OBSTACLE],
					  image->getTimestamp(), 
					  &lastSeenObjects);
      } catch (TribotsException &e) {
	std::cerr << "Fehler in Simple ObstDetector: " << e.what() << "\n";
      }
    }
    
    
    obstacleDetectorTime += time.elapsed_usec();
    if (report_computation_time_gnuplot) gnuplot  << time.elapsed_usec() << " ";
    cycles++;    
    delete image;

    if (report_computation_time_gnuplot) {
      string str;
      getline(gnuplot, str);
      JMESSAGE(str.c_str());
    }

    if (processedImageRequested) {
      processedImageRequested = false;       // image preparation finished
    }
  }

  void ScanLineImageProcessing::request_image_raw() throw(TribotsException)
  {
    if (imageCollected) {
      throw TribotsException("You have collected an image, but have not "
			     "freed the image yet.");
    }
    if (rawImageRequested || processedImageRequested) {
      throw TribotsException("At the moement, I'm already preparing another "
			     "image.");
    }
    if (requestedImage) {
      delete requestedImage;
      requestedImage = 0;
    }
    rawImageRequested = true;
  }
      
  void ScanLineImageProcessing::request_image_processed() throw(TribotsException)
  {
    if (imageCollected) {
      throw TribotsException("You have collected an image, but have not "
			     "freed the image yet.");
    }
    if (rawImageRequested || processedImageRequested) {
      throw TribotsException("At the moement, I'm already preparing another "
			     "image.");
    }
    if (requestedImage) {
      delete requestedImage;
      requestedImage = 0;
    }
    processedImageRequested = true;
  }

  bool ScanLineImageProcessing::is_image_available() throw()
  {
    return requestedImage != 0;
  }
  
  const Image* ScanLineImageProcessing::get_image() 
    throw(TribotsException)
  {
    if (! requestedImage) {
      throw TribotsException("No image ready to collect. Please request an "
			     "image first and make sure it's ready to be "
			     "collected (check is_image_available from time "
			     " to time until it returns true).");
    }
    imageCollected = true;
    return requestedImage;
  }

  void ScanLineImageProcessing::free_image() throw(TribotsException)
  {
    if (! imageCollected) {
      throw TribotsException("You have not collected an image. There is "
			     "nothing to free.");
    }
    delete requestedImage;
    requestedImage = 0;
    imageCollected = false;
  }


  void ScanLineImageProcessing::visualizeCommonInfo() 
  {
    const RobotMask* mask = RobotMask::getMask();
    RGBTuple rgb;
    
    for (int x=0; x < requestedImage->getWidth(); x++) {  // draw image mask
      for (int y=0; y < requestedImage->getHeight(); y++) {
	if (!mask->isValid(x,y)) {
	  requestedImage->getPixelRGB(x,y, &rgb);
	  rgb.r = ((rgb.r + 175) / 2);
	  rgb.g = ((rgb.g + 175) / 2);
	  rgb.b = ((rgb.b + 250) / 2);
	  requestedImage->setPixelRGB(x,y, rgb);
	}
      }
    }

    Painter painter(*requestedImage);            // draw image center
    painter.setColor(Painter::white);
    painter.markCrosshair(centerX, centerY, 5);
  }

}
