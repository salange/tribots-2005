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
#include <fstream>
#include <qmessagebox.h>
#include <string>

using namespace std;

void imageview::init()
{
  imgSource = 0;
  freezed = false;
}

void imageview::showImage(int cycle)
{
  if (! imgSource) {  // keine Bildquelle ausgewählt
    return ;
  }
  if (! isVisible()) {  // wird eh nicht angezeigt
    return ;
  }
  if (freezed) {       // aktuelles Bild eingefroren
    return;
  }
  Image* image = imgSource->getImageNumber(cycle, 0);
  imageWidget->setImage(*image);
  filenameInfo->setText(imgSource->getFilename());
  timestampInfo->setText(QString::number(imgSource->getTimestamp().get_msec()));
  delete image;
}

void imageview::toggleFreeze(bool b)
{
  freezed = b;
}

void imageview::loadImages( QString filename, int cycle )
{
  ifstream in(filename);
  if (!in) {
    QMessageBox::information(this, "File access error", "Could not open " + filename + " for reading");
  }
  string tmp, fileType;
  in >> tmp;
  in >> fileType;
  in.close();
  
  if (fileType == "PPM") {
    imgSource = new FileSource(new PPMIO(), filename);
  }
  else if (fileType == "JPEG") {
    imgSource = new FileSource(new JPEGIO(), filename);
  }
  else {
    QMessageBox::information(this, "Wrong format", "I can not read images of the format " + fileType + "!");
    return;
  }
  
  showImage(cycle);
}
