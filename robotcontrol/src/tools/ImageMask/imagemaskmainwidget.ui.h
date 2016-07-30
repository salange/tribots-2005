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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

using namespace Tribots;
using namespace std;

#include <qstring.h>
#include "../../ImageProcessing/ImageSources/IIDC.h"

void ImageMaskMainWidget::init()
{
  if (qApp->argc() <= 1)  {
     qFatal("Aufruf %s Konfigurationsdatei", qApp->argv()[0]);
  }
  
  configFileName = qApp->argv()[1];
 
  ConfigReader config(0);
  config.append_from_file(configFileName);
  ConfigReader wmconfig(0);
  
  programStart = true;
  capturing = false;
  threshold = 7;
  thresholdSlider->setValue(threshold);
  
  whiteRegionMin = 4000;
  blackRegionMin = 1000;
  
  centerX = 320;
  centerY = 240;
  
  filename = "";

  producer = new ImageProducer(config);
  
  image = producer->nextImage();
  cImageWidget->setImage(*image);
  
  distanceFilter = new ImageDistanceFilter();
  mask = 0;
  
  fileSaveAction->setEnabled(false);
  fileSaveAsAction->setEnabled(false);
  
  timer = new QTimer( this );
  timer->start(50, false);
  connect(timer, SIGNAL(timeout()), this, SLOT(nextImageSlot()));
  connect(this, SIGNAL(imageChanged(const Image&)), cImageWidget, 
               SLOT(setImage(const Image&)));
 
  whiteClassifier = new WhiteClassifier();
  blackClassifier = new BlackClassifier();
  regionDetector = new RegionDetector();
  
  robot = 0;
  try {
    robot = new Robot(config);
  } catch (TribotsException& e) {
    robot = 0;
    qWarning("Kein Roboter verfügbar: %s\nFahre ohne Roboter fort.", e.what());
  } catch (std::exception& e) {
    robot = 0;
    qWarning("KeinRoboter verfügbar (std::exception): %s\nFahre ohne Roboter fort.",
             e.what());
  }  
}

void ImageMaskMainWidget::destroy()
{
  if (whiteClassifier) delete whiteClassifier;
  if (blackClassifier) delete blackClassifier;
  if (regionDetector) delete regionDetector;
  if (distanceFilter) delete distanceFilter;
  if (producer) delete producer;
}

void ImageMaskMainWidget::fileSave()
{
  if (! mask || capturing) {
    qWarning("Have to create a mask first.");
    return;
  }
  
  if (filename == "") {
    fileSaveAs();
  }
  else {
    configSaveAction->setEnabled(true);
    writeMask(filename);
    statusBar()->message("Saved mask in " + filename);
  }
}

void ImageMaskMainWidget::fileSaveAs()
{
  if (! mask || capturing) {
    qWarning("Have to create a mask first.");
    return;
  }
  
  filename = QFileDialog::getSaveFileName(QString::null, "Protable Pixmap (*.ppm)", this,
                                                                       "Save File Dialog", 
                                                                      "Choose a filename to save under");  
  configSaveAction->setEnabled(true);
  writeMask(filename);
  statusBar()->message("Saved mask in " + filename);
}

void ImageMaskMainWidget::writeConfig()
{
  config.set("ScanLine::robot_mask_file", (const std::string)(const char*)filename);
  config.set("ScanLine::image_center_x", centerX);
  config.set("ScanLine::image_center_y", centerY);
  config.replace_config_file(configFileName);
  statusBar()->message("Wrote configuration to file " + configFileName);
}

void ImageMaskMainWidget::writeMask(QString filename)
{
  ImageIO* imgIO = new PPMIO();
  try {
    imgIO->write(*mask, filename);
  } catch (TribotsException& e) {
    qWarning("Could not write image to file:\n%s", e.what());
  }
  delete imgIO;
}


void ImageMaskMainWidget::fileExit()
{
  qApp->quit();
}


void ImageMaskMainWidget::helpIndex()
{
  
}


void ImageMaskMainWidget::helpContents()
{
  
}


void ImageMaskMainWidget::helpAbout()
{
  
}


void ImageMaskMainWidget::nextImageSlot()
{
  delete image;
  image = producer->nextImage();
  
  // had problems with the resize policy, have to clean this up some time...
  centralWidget()->setMaximumSize(frame3->width()+image->getWidth(), 
                                                          image->getHeight());
  
  setMaximumSize(centralWidget()->maximumWidth(), 
                              centralWidget()->maximumHeight()
                              + menuBar()->height() 
                              + statusBar()->height());
  
  if (capturing) {
    distanceFilter->add(*image);

    if (robot) {   // setze Fahrbefehl
      robot->set_drive_vector(DriveVector(Vec(0.,0.), 1.66, false));
    }
    else { 
      emit imageChanged(*image);
    }
  }
  else if (programStart) {
    emit imageChanged(*image);
  }
  else {
    if (mask) {
      emit imageChanged(*mask); 
    }
  }
}

void ImageMaskMainWidget::toggleCapture(bool state)
{
  if (state) {   // capture on
    textLabel1->setEnabled(false);
    thresholdSlider->setEnabled(false);
    fileSaveAction->setEnabled(false);
    fileSaveAsAction->setEnabled(false);    
    
    distanceFilter->initialize(image->getWidth(), image->getHeight());    
   
    programStart = false;
    capturing = true;
    
    if (robot) {
      captureButton->setEnabled(false); // nach Roboterbewegung wieder angestellt
      QTimer::singleShot( 9 * 1000, this, SLOT(captureOff())); // 10s -> aus
    }
  }
  else {      // capture switched off
    if (distanceFilter->getNumImagesAdded() < 1) {
      qWarning("Please grab at least 2 images");
      
      captureButton->setOn(true);  // grab at least two images
      return;
    }
    
    if (robot) {
      captureButton->setEnabled(true);
    }
    
    this->recalculateMask();
    this->recalculateImageCenter();
    
    textLabel1->setEnabled(true);
    thresholdSlider->setEnabled(true);
    fileSaveAction->setEnabled(true);
    fileSaveAsAction->setEnabled(true);
    
    capturing = false;
  }
}


void ImageMaskMainWidget::recalculateMask()
{
  if (mask) delete mask;
  mask = 0;
  mask = distanceFilter->createMask(threshold);
  
  // Erosionsoperator anwenden um kleine weisse unterbrechungen zu entfernen
  // und die Maske (schwarze bereiche) an den Objekträndern um ein Pixel zu 
  // verbreitern
  Erosion erosion = Erosion(3);
  Image* tmp = erosion(*mask);
  delete mask;
  mask = tmp;
  
  // Rand schwärzen  
  mask->setBlackBorder();
 
  // kleine weisse Flecken mit schwarz füllen
  RGBTuple black = { 0, 0, 0  };
  mask->setClassifier(whiteClassifier);
  vector<Tribots::Region> regions;
  regionDetector->findRegions(*mask, 1, &regions);
  
  for (unsigned int i=0; i < regions.size(); i++) {
    if (regions[i].getArea() < whiteRegionMin) {
      fillRegion(mask, regions[i], black);
    }
  }
  
  // Rand weissen  
  mask->setWhiteBorder();
  
  // kleine schwarze Flecken mit weiss füllen
  mask->setClassifier(blackClassifier);
  RGBTuple white = { 255, 255, 255 };
  mask->setClassifier(blackClassifier);
  regions.clear();
  regionDetector->findRegions(*mask, 1, &regions);
  
  for (unsigned int i=0; i < regions.size(); i++) {
    if (regions[i].getArea() < blackRegionMin) {
      fillRegion(mask, regions[i], white);
    }
  }
  
  // weitere Erosionsschritte
  for (int i=0; i < 4; i++) {
    tmp = erosion(*mask);
    delete mask;
    mask = tmp;
  }
  
  // rand schwärzen (sollte sicher sein)...  
  mask->setBlackBorder();  
}


void ImageMaskMainWidget::thresholdChanged( int threshold )
{
  this->threshold = threshold;
  if (!capturing && !programStart) {
    this->recalculateMask();
    this->recalculateImageCenter();
  }
}

// als annaehrung mal das gewichte mittel der koordinaten der weissen Pixel. Spaeter soll
// hier was anderes hin (z.B. Hugh-Transformation)
void ImageMaskMainWidget::recalculateImageCenter()
{
  double cX = 0;
  double cY = 0;
  int count = 0;
  mask->setClassifier(whiteClassifier);  
  for (int x=0; x < mask->getWidth(); x++) {
    for (int y=0; y < mask->getHeight(); y++) {
      if (mask->getPixelClass(x,y) == 1)  {
        cX += x; cY += y;
        count++;
      }
    }
  }
  centerX = (int)(cX / count);
  centerY = (int)(cY / count);
  
  statusBar()->message("New center calculated: " + QString::number(centerX) + ", " + QString::number(centerY));
}

/* Achtung: Löcher werden NICHT gefüllt!!!!  Dazu muss diese Funktion auch noch
   mit der Kante des Lochs aufgerufen werden!  Das stellt allerdings ein Problem dar,
   weil erstmal das loch gesucht werden muss... 
   
   Zur Zeit werden Löcher daher einfach nicht richtig behandelt ! */
void ImageMaskMainWidget::fillRegion( Tribots::Image * image, 
                                      const Tribots::Region & region,
                                      const Tribots::RGBTuple & color )
{
  int x = region.x;
  int y = region.y;
  
  for (unsigned int i=0; i < region.chainCode.size(); i++) {
    int dir = region.chainCode[i];
    
    if (dir == 3 || dir == 0) {    // bei jedem Schritt nach unten oder nach rechts
      int xImg = x;
      while (image->getPixelClass(xImg,y) == region.colClass && 
                xImg < image->getWidth()) {
        image->setPixelRGB(xImg, y, color);
        xImg++;
      }
    } 
    x += ChainCoder::xDir[dir];
    y += ChainCoder::yDir[dir];
  } 
  
  // sonderbehandlung für erstes==letztes 
  // pixel  und den fall, dass die Fläche nur ein pixel hat
  int xImg = x;
  while (image->getPixelClass(xImg,y) == region.colClass && 
         xImg < image->getWidth()) {
    image->setPixelRGB(xImg, y, color);
    xImg++;
  } 
}


void ImageMaskMainWidget::captureOff()
{
  toggleCapture(false);
}
