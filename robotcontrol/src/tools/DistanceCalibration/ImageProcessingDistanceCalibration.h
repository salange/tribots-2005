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


#ifndef TribotsTools_ImageProcessingDistanceCalibration_h
#define TribotsTools_ImageProcessingDistanceCalibration_h

#include "../../ImageProcessing/ImageProcessingType.h"
#include "../../ImageProcessing/MultiImageProducer.h"
#include "../../ImageProcessing/ColorClasses.h"
#include "../../ImageProcessing/LineScanning.h"
#include "../../ImageProcessing/BallDetector.h"
#include "DistanceCalibration.h"
#include <string>


namespace TribotsTools {

  /**
   * Pseudobildverarbeitung, um eine Distanztabelle zu erzeugen
   */
  class ImageProcessingDistanceCalibration : public Tribots::ImageProcessingType {
  public:
    ImageProcessingDistanceCalibration(const Tribots::ConfigReader& config);
    virtual ~ImageProcessingDistanceCalibration() throw();

    void process_image() throw (Tribots::BadHardwareException);    

  private:
    Tribots::Vec image_center;
    std::string markerFile;

    Tribots::MultiImageProducer* imgProd;
    Tribots::ColorClassInfoList* colClass;
    Tribots::ScanResultList* scanResults;
    Tribots::LineScanner* scanner;
    Tribots::CoordinateMapping* image2real;
    Tribots::CoordinateMapping* real2image;
    Tribots::BallDetector* ballDetector;
    TribotsTools::DistanceCalibration* distcal;
  };
}

#endif
