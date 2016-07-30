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

#ifndef _ScanLineImageProcessing_h_
#define _ScanLineImageProcessing_h_

#include "ImageProcessingType.h"
#include "MultiImageProducer.h"
#include "ColorClasses.h"
#include "../Structures/TribotsException.h"

namespace Tribots {

  class ScanResultList;
  class LineScanner;
  class LineDetector;
  class GoalDetector;
  class FieldMapper;
  class BallDetector;
  class CoordinateMapping;
  class ObstacleDetector;
  class ChainCoder;

  /**
   * Diese Bildverarbeitung verwendet radial angeordnete Scanlinine um
   * interessante Farben und Farbübergänge im Bild zu finden und diese
   * einzelnen Objekten zuzuordnen.
   */
  class ScanLineImageProcessing : public Tribots::ImageProcessingType {
  public:
    ScanLineImageProcessing(const ConfigReader& config) 
      throw(InvalidConfigurationException, HardwareException,
	    BadHardwareException, TribotsException);
    virtual ~ScanLineImageProcessing() throw();

    virtual void process_image() throw (Tribots::BadHardwareException);   

    virtual void request_image_raw() throw(TribotsException);
    virtual void request_image_processed() throw(TribotsException);
    virtual bool is_image_available() throw();
    virtual const Image* get_image() throw(TribotsException);
    virtual void free_image() throw(TribotsException); 

  protected: 

    long producerTime;
    long scannerTime;
    long lineDetectorTime;
    long goalDetectorTime;
    long ballDetectorTime;
    long obstacleDetectorTime;
    long fieldMapperTime;

    long cycles;

    MultiImageProducer* imgProd;
    ColorClassInfoList* colClass;
    ScanResultList* scanResults;
    LineScanner* scanner;
    CoordinateMapping* image2real;
    CoordinateMapping* real2image;
    LineDetector* lineDetector;
    GoalDetector* goalDetector;
    BallDetector* ballDetector;
    ChainCoder* cc;
    ObstacleDetector* obstacleDetector;
    FieldMapper * fieldmapper;

    bool report_computation_time;
    bool report_computation_time_gnuplot;

    bool rawImageRequested;
    bool processedImageRequested;
    Image* requestedImage;
    bool imageCollected;

    int centerX, centerY;

    void visualizeCommonInfo();
  };
}

#endif
