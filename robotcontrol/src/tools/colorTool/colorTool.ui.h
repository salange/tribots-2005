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
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include <iostream>
#include <qstatusbar.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <string>
#include <sstream>

#include "../../Fundamental/ConfigReader.h"
#include "../../ImageProcessing/ImageProducer.h"
#include "../../ImageProcessing/Image.h"
#include "../../ImageProcessing/ColorClasses.h"
#include "GrabbingThread.h"
#include "ColorTools.h"
#include "unistd.h"

#ifndef MIN 
#define MIN(x,y) (x) < (y) ? (x) : (y)
#endif

#ifndef MAX
#define MAX(x,y) (x) > (y) ? (x) : (y)
#endif


// In this file you'll find only GUI and controller stuff. The work
// (store range information, segment, color space transformation) is
// done by the HSISegmentationTool in ColorTools.h and the GrabbingThread.

// Range mappings between sliders and model (ColorRange object):
// H: Slider [0,360]   ColorRange [0.,360.]
// S: Slider [0,100]   ColorRange [0.,1.]
// I: Slider [0,100]   ColorRange [0.,1.]

using namespace std;
using namespace Tribots;

void mainWindow::init()
{
  configReader = 0;
  producer = 0;
  image = 0;
  slidersActivated = true;
  filename = "";
  lutFilename = "";

  lastH = lastI = lastS = 0.;

  if (qApp->argc() != 2)
  {      // check arguments
    cerr << "Usage: " << qApp->argv()[0] << " config_file" << endl;
    QApplication::exit(1);
  }

  ColorClassInfoList colorInfos;
  colorSelector->insertItem("ALL");//class 0 (IGNORE_COLOR): display all
  for (unsigned int i=1; i<colorInfos.classList.size(); i++)
  {
    colorSelector->insertItem(colorInfos.classList[i]->name,
                              colorInfos.classList[i]->id);
  }


  configFileName = qApp->argv()[1];

  try
  {                     // try to initialize and start image capturing
    configReader = new ConfigReader(0);
    configReader->append_from_file(configFileName.c_str());

    producer = new ImageProducer(*configReader);
    //    producer->setFramerate(3);  // \todo: produces hang downs on robots
    grabbingThread = new GrabbingThread(producer);
  }
  catch (exception& e)
  {
    cerr << e.what() << "\n\r" << endl;
    qApp->exit(1);
  }
  initSlidersAndLabels();
  saveLookupTableAction->setEnabled(false);
  saveAsLookupTableAction->setEnabled(false);
  useLookupTableAction->setEnabled(false);
  
  
  LUTLabel->setText("ColorRanges");

  grabbingThread->start();  // start grabbing and segmenatation thread

  connect(imageWidget,SIGNAL(mousePressed(QMouseEvent*)),this,SLOT(slotImageWidgetClicked(QMouseEvent*)));

}


/** init all sliders and text labels when the program has been started */
void mainWindow::initSlidersAndLabels()
{
  int colorID = colorSelector->currentItem();
  ColorRange range =
    grabbingThread->getSegmentationTool()->getColorRange(colorID);

  //  hMinSlider->setValue((int)range.getMin(0));
  hMinLabel->setValue((int)range.getMin(0));
  //  hMaxSlider->setValue((int)range.getMax(0));
  hMaxLabel->setValue((int)range.getMax(0));

  //  sMinSlider->setValue((int)(range.getMin(1) * 100.));
  sMinLabel->setValue((int)(range.getMin(1) * 100.));
  //  sMaxSlider->setValue((int)(range.getMax(1) * 100.));
  sMaxLabel->setValue((int)(range.getMax(1) * 100.));

  //  iMinSlider->setValue((int)(range.getMin(2) * 100.));
  iMinLabel->setValue((int)(range.getMin(2) * 100.));
  //  iMaxSlider->setValue((int)(range.getMax(2) * 100.));
  iMaxLabel->setValue((int)(range.getMax(2) * 100.));

  if (colorID == 0) activateSliders(false);
}

/** copies images from grabbing thread (when new image ready, checks by
 *  polling) and executes window events (calls qApp->proccessEvents)
 */
void mainWindow::run()
{
  int frameCounter = 0;
  while (qApp->mainWidget()->isVisible())
  {
    if (grabbingThread->getFrameCounter() != frameCounter)
    {
      frameCounter = grabbingThread->getFrameCounter();
      imageWidget->setImage(grabbingThread->getImage());
    }
    qApp->processEvents();
    usleep(10 * 1000);
  }
}

/** clean up */
void mainWindow::destroy()
{
  if (configReader) delete configReader;
  if (producer) delete producer;
}

/** the active color has been changed (using the color selector). notify
 *  the grabbingThread's segmentationTool and adjust the sliders. */
void mainWindow::colorChanged(int c)
{
  grabbingThread->getSegmentationTool()->setActiveColor(c);
  statusBar()->message(
    "Changed active color to color with id " + QString::number(c));

  if (c == 0)
  {
    activateSliders(false);
  }
  else
  {
    if (! slidersActivated) activateSliders(true);
    updateRanges();
  }
}

/** activate and deactivate the sliders */
void mainWindow::activateSliders(bool act)
{
  hMinSlider->setEnabled(act);
  hMaxSlider->setEnabled(act);
  sMinSlider->setEnabled(act);
  sMaxSlider->setEnabled(act);
  iMinSlider->setEnabled(act);
  iMaxSlider->setEnabled(act);
  hMinLabel->setEnabled(act);
  hMaxLabel->setEnabled(act);
  sMinLabel->setEnabled(act);
  sMaxLabel->setEnabled(act);
  iMinLabel->setEnabled(act);
  iMaxLabel->setEnabled(act);
  slidersActivated = act;
}

// 6 slots for the 6 min/max - sliders.  /////////////////////////////////

void mainWindow::hMinChanged(int min)
{
  int id = grabbingThread->getSegmentationTool()->getActiveColor();
  ColorRange range =
    grabbingThread->getSegmentationTool()->getColorRange(id);
  range.setMin(0, min);
  grabbingThread->getSegmentationTool()->setColorRange(id, range);
  if (hMinSlider->value()!=min) hMinSlider->setValue (min);
  if (hMinLabel->value()!=min) hMinLabel->setValue (min);
}

void mainWindow::hMaxChanged(int max)
{
  int id = grabbingThread->getSegmentationTool()->getActiveColor();
  ColorRange range =
    grabbingThread->getSegmentationTool()->getColorRange(id);
  range.setMax(0, max);
  grabbingThread->getSegmentationTool()->setColorRange(id, range);
  if (hMaxSlider->value()!=max) hMaxSlider->setValue (max);
  if (hMaxLabel->value()!=max) hMaxLabel->setValue (max);
}

void mainWindow::sMinChanged(int min)
{
  int id = grabbingThread->getSegmentationTool()->getActiveColor();
  ColorRange range =
    grabbingThread->getSegmentationTool()->getColorRange(id);
  range.setMin(1, min / 100.);
  grabbingThread->getSegmentationTool()->setColorRange(id, range);
  if (sMinSlider->value()!=min) sMinSlider->setValue (min);
  if (sMinLabel->value()!=min) sMinLabel->setValue (min);
}

void mainWindow::sMaxChanged(int max)
{
  int id = grabbingThread->getSegmentationTool()->getActiveColor();
  ColorRange range =
    grabbingThread->getSegmentationTool()->getColorRange(id);
  range.setMax(1, max / 100.);
  grabbingThread->getSegmentationTool()->setColorRange(id, range);
  if (sMaxSlider->value()!=max) sMaxSlider->setValue (max);
  if (sMaxLabel->value()!=max) sMaxLabel->setValue (max);
}

void mainWindow::iMinChanged(int min)
{
  int id = grabbingThread->getSegmentationTool()->getActiveColor();
  ColorRange range =
    grabbingThread->getSegmentationTool()->getColorRange(id);
  range.setMin(2, min / 100.);
  grabbingThread->getSegmentationTool()->setColorRange(id, range);
  if (iMinSlider->value()!=min) iMinSlider->setValue (min);
  if (iMinLabel->value()!=min) iMinLabel->setValue (min);
}

void mainWindow::iMaxChanged(int max)
{
  int id = grabbingThread->getSegmentationTool()->getActiveColor();
  ColorRange range =
    grabbingThread->getSegmentationTool()->getColorRange(id);
  range.setMax(2, max / 100.);
  grabbingThread->getSegmentationTool()->setColorRange(id, range);
  if (iMaxSlider->value()!=max) iMaxSlider->setValue (max);
  if (iMaxLabel->value()!=max) iMaxLabel->setValue (max);
}

//////////////////////////////////////////////////////// Finished slots //



/** update the sliders to the present values of the model. Is called,
 *  whenever the active color has been changed.
 */
void mainWindow::updateRanges()
{
  ColorRange range = grabbingThread->getSegmentationTool()->getColorRange(
                       grabbingThread->getSegmentationTool()->getActiveColor());

  hMinLabel->setValue((int)range.getMin(0));
  hMaxLabel->setValue((int)range.getMax(0));

  sMinLabel->setValue((int)(range.getMin(1) * 100.));
  sMaxLabel->setValue((int)(range.getMax(1) * 100.));

  iMinLabel->setValue((int)(range.getMin(2) * 100.));
  iMaxLabel->setValue((int)(range.getMax(2) * 100.));

  hMinSlider->setValue((int)range.getMin(0));
  hMaxSlider->setValue((int)range.getMax(0));

  sMinSlider->setValue((int)(range.getMin(1) * 100.));
  sMaxSlider->setValue((int)(range.getMax(1) * 100.));

  iMinSlider->setValue((int)(range.getMin(2) * 100.));
  iMaxSlider->setValue((int)(range.getMax(2) * 100.));
}

void mainWindow::fileNew()
{
}


void mainWindow::fileOpen()
{
  QString filename =
    QFileDialog::getOpenFileName(QString::null,
                                 QString::null,
                                 0,
                                 "Open File Dialog",
                                 "Choose file to open");
  if (filename != "")
  {
    grabbingThread->getSegmentationTool()->load(filename);
    updateRanges();
    statusBar()->message("Loaded ranges from file " + filename);
    this->filename = filename;       // remember file
  }
}

void mainWindow::openLookupTable()
{
  QString filename =
    QFileDialog::getOpenFileName(QString::null,
                                 QString::null,
                                 0,
                                 "Open Lookup Table Dialog",
                                 "Choose a file to open");
  if (filename != "")
  {
    grabbingThread->getSegmentationTool()->loadLookupTable(filename);
    statusBar()->message("Loaded lookuptable from file " + filename);
    this->lutFilename = filename;       // remember file

    saveLookupTableAction->setEnabled(true);
    saveAsLookupTableAction->setEnabled(true);
    useLookupTableAction->setEnabled(true);
  }
}


void mainWindow::fileSave()
{
  if (filename == "")
  {
    fileSaveAs();
  }
  else
  {
    saveRanges();
  }
}

void mainWindow::saveRanges()
{
  grabbingThread->getSegmentationTool()->save(filename);
  statusBar()->message(QString("Saved ranges to file ") + filename);
}


void mainWindow::fileSaveAs()
{
  QString filename =
    QFileDialog::getSaveFileName(QString::null,
                                 QString::null,
                                 0,
                                 "Save File Dialog",
                                 "Choose a filename to save under");
  if (filename != "")
  {
    this->filename = filename;
    saveRanges();
  }
}


void mainWindow::saveLookupTable()
{
  if (lutFilename == "")
  {
    saveAsLookupTable();
  }
  else
  {
    doSaveLookupTable();
  }
}

void mainWindow::saveAsLookupTable()
{
  QString filename =
    QFileDialog::getSaveFileName(QString::null,
                                 QString::null,
                                 0,
                                 "Save File Dialog",
                                 "Choose a filename to save under");
  if (filename != "")
  {
    this->lutFilename = filename;
    doSaveLookupTable();
  }
}

void mainWindow::doSaveLookupTable()
{
  grabbingThread->getSegmentationTool()->saveLookupTable(lutFilename);
  statusBar()->message(QString("Saved lookup table to file ") +
                       lutFilename);
}

void mainWindow::fileExit()
{
  qApp->mainWidget()->close();
}


void mainWindow::helpIndex()
{
}


void mainWindow::helpContents()
{
}

void mainWindow::createLookupTable()
{
  statusBar()->message("This will take a few seconds. "
                       "Sorry for not responding.");
  grabbingThread->getSegmentationTool()->createLookupTable();
  statusBar()->message("Have finished the creation of the lookup table!");

  saveLookupTableAction->setEnabled(true);
  saveAsLookupTableAction->setEnabled(true);
  useLookupTableAction->setEnabled(true);
}

void mainWindow::useLookupTable(bool a)
{
  if (a && !grabbingThread->getSegmentationTool()->haveLookupTable())
  {
    statusBar()->message("Could not do that! Please first create "
                         "a lookup table.");
    useLookupTableAction->setOn(false);
  }
  else
  {
    if(!a) LUTLabel->setText("Ranges");
    else LUTLabel->setText("LookUp");
    activateSliders(! a);
    colorSelector->setEnabled(! a);
    grabbingThread->getSegmentationTool()->useLookupTable(a);
  }
}


void mainWindow::helpAbout()
{
  QMessageBox::about(this,
                     "About Tribots ColorTool",
                     "Tool to define the ranges of the\n"
                     "color classes that are used by the\n"
                     "tribots control program. Generates\n"
                     "and writes lookuptables.\n\n"
                     "(c) 2005 Sascha Lange\nEmail: salange@uos.de");
}


void mainWindow::toggleOrigImage( bool  showIt)
{
  grabbingThread->showOriginalImage(showIt);
}

void mainWindow::slotImageWidgetClicked( QMouseEvent * ev)
{
  // Imagewidget sends signal mousepressed
  int x=ev->x();
  int y=ev->y();
  YUVTuple yuv;
  RGBTuple rgb;

  grabbingThread->getImage().getPixelYUV(x,y,&yuv);
  PixelConversion::convert(yuv,&rgb);

  rgb2hsy(rgb, lastH, lastS, lastI); // use the conical hsl space with y 

  stringstream str;
  str << "RGB: (" << (int)rgb.r << ", " << (int)rgb.g << ", " << (int)rgb.b 
      << ")  YUV: (" << (int)yuv.y << ", " << (int)yuv.u << ", " << (int)yuv.v
      << ")  HSX: (" << lastH << ", " << lastS << ", " << lastI << ")";
  statusBar()->message(str.str().c_str());
  
  colorField->setPaletteBackgroundColor(QColor((int)rgb.r,
					       (int)rgb.g,
					       (int)rgb.b));
}

void mainWindow::setColor()
{
  hMinSlider->setValue((int)(lastH-10));
  hMaxSlider->setValue((int)(lastH+10));

  sMinSlider->setValue((int)(100*lastS-10));
  sMaxSlider->setValue((int)(100*lastS+10));

  iMinSlider->setValue((int)(100*lastI-10));
  iMaxSlider->setValue((int)(100*lastI+10));
}

void mainWindow::addColor()
{
  hMinSlider->setValue((int)(MIN(lastH-10, hMinSlider->value())));
  hMaxSlider->setValue((int)(MAX(lastH+10, hMaxSlider->value())));

  sMinSlider->setValue((int)(MIN(100*lastS-10, sMinSlider->value())));
  sMaxSlider->setValue((int)(MAX(100*lastS+10, sMaxSlider->value())));

  iMinSlider->setValue((int)(MIN(100*lastI-10, iMinSlider->value())));
  iMaxSlider->setValue((int)(MAX(100*lastI+10, iMaxSlider->value())));
}

void mainWindow::subColor()
{
  // links oder rechts? oder da, wo näher??
}

void mainWindow::nextColor()
{
  int num;
  int active;
  num =colorSelector->count ();
  active=colorSelector->currentItem();
  colorSelector->setCurrentItem((active+1)%num);
  colorChanged((active+1)%num);
}


void mainWindow::previousColor()
{
  int num;
  int active;
  num =colorSelector->count ();
  active=colorSelector->currentItem();
  active=active-1;
  if (active <0) active =num-1;
  colorSelector->setCurrentItem(active);
  colorChanged(active);
}


void mainWindow::slotQuickLoad()
{
  QString filename;
  QDir access;
  access=access.home();
  filename=access.absFilePath(".robotcontrol/colors.ranges");
  //cout <<filename<<endl;
  if (QFile::exists(filename))
  {
    grabbingThread->getSegmentationTool()->load(filename);
    updateRanges();
    statusBar()->message("Loaded ranges from file " + filename);
    this->filename = filename;       // remember file
  }


  access=access.home();
  filename=access.absFilePath(".robotcontrol/colors.lut");
  
  
  if (QFile::exists(filename))
  {
    grabbingThread->getSegmentationTool()->loadLookupTable(filename);
    statusBar()->message("Loaded lookuptable from file " + filename);
    this->lutFilename = filename;       // remember file

    saveLookupTableAction->setEnabled(true);
    saveAsLookupTableAction->setEnabled(true);
    useLookupTableAction->setEnabled(true);
  }
  
  
  
  
    
  
  
}


void mainWindow::slotQuickSave()
{
  QString filename;
  QDir access;
  access=access.home();
  filename=access.absFilePath(".robotcontrol/colors.ranges");
  //cout <<filename<<endl;
   grabbingThread->getSegmentationTool()->save(filename);
    statusBar()->message(QString("Saved ranges to file ") + filename);
   this->filename = filename;       // remember file
  


  access=access.home();
  filename=access.absFilePath(".robotcontrol/colors.lut");
  
  
grabbingThread->getSegmentationTool()->saveLookupTable(lutFilename);
  statusBar()->message(QString("Saved lookup table to file ") +
                       lutFilename);
    this->lutFilename = filename;       // remember file

    }
  
  
