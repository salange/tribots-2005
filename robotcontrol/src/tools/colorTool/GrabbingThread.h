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

#ifndef _GRABBINGTHREAD_H_
#define _GRABBINGTHREAD_H_

#include <pthread.h>

namespace Tribots {
  class Image;
  class ImageProducer;
  class CImageWidget;
  class ColorClassInfoList;
  class HSISegmentationTool;
}

using namespace Tribots;

class POSIXThread {
public:
  virtual ~POSIXThread();
  virtual void run() =0;
  void start();
protected:
  pthread_t id;
};
  

class GrabbingThread : public POSIXThread {
public:
  GrabbingThread(Tribots::ImageProducer* producer);
  virtual ~GrabbingThread();
  
  virtual void run();  
  
  int getFrameCounter() const { return frameCounter; }
  const Image& getImage() const { return *bufferedImage; };
  HSISegmentationTool* getSegmentationTool() { return segmentTool; }
  void showOriginalImage(bool show);
protected:
  Tribots::ImageProducer* producer;
  Tribots::Image* image;
  Tribots::Image* bufferedImage;
  HSISegmentationTool* segmentTool;
  ColorClassInfoList* colorInfos; 
  bool showOrig;
  int frameCounter;
};


#endif
