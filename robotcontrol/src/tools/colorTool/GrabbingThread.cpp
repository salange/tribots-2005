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

#include "GrabbingThread.h"
#include "ColorTools.h"
#include "../../ImageProcessing/ImageProducer.h"
#include "../../ImageProcessing/Image.h"
#include "../../ImageProcessing/Images/RGBImage.h"
#include "../../ImageProcessing/ColorClasses.h"
#include "../components/imagewidget.h"

using namespace Tribots;
using namespace std;

static void* startPThread(void* arg)
{
  static_cast<POSIXThread*>(arg)->run();
  return 0;
}

void POSIXThread::start()
{
  pthread_create(&id, 0, startPThread, this);
}

POSIXThread::~POSIXThread()
{}

GrabbingThread::GrabbingThread(ImageProducer* producer)
: producer(producer), image(0), bufferedImage(0), segmentTool(0),
  colorInfos(0), showOrig(true),frameCounter(0)
{
  image = producer->nextImage();
  bufferedImage = new RGBImage(image->getWidth(), image->getHeight());
  segmentTool = new HSISegmentationTool();
  colorInfos = new ColorClassInfoList();
}

GrabbingThread::~GrabbingThread()
{
  if (image) delete image;
  if (bufferedImage) delete image;
  if (segmentTool) delete segmentTool;
  if (colorInfos) delete colorInfos;
}

void GrabbingThread::run()
{
  while(1) {
    delete image;
    image = producer->nextImage();
    image->setClassifier(segmentTool);
    
    unsigned char c;
    RGBTuple rgb; 
    rgb.r = 0;
    rgb.g = 0;
    rgb.b = 0;
    for (int x=0; x < image->getWidth(); x++) {
      for (int y=0; y < image->getHeight(); y++) {
        c = image->getPixelClass(x,y);
        if (c == COLOR_IGNORE) {
          if (showOrig) image->getPixelRGB(x,y, &rgb);
          bufferedImage->setPixelRGB(x,y, rgb); 
        }
        else {
          bufferedImage->setPixelRGB(x,y, colorInfos->classList[c]->color);
        }
      }
    }
    frameCounter++; 
  }
}

void GrabbingThread::showOriginalImage(bool show) 
{
  showOrig = show;
}
