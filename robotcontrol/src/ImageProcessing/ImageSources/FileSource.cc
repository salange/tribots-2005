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

#include "FileSource.h"
#include <fstream>

namespace Tribots {
  using namespace std;

  FileSource::FileSource(ImageIO* imageIO, const string& filename)
    : ImageSource(), imageIO(imageIO), pos(0), width(0), height(0)
  {
    ifstream in(filename.c_str());
    if (!in) {
      throw BadHardwareException(__FILE__ ": Could not open file.");
    }
    string type;
    in >> type;
    if (! (type == "single")) {
      throw BadHardwareException(__FILE__ ": This type is not supported.");
    }

    string ioType;
    in >> ioType;
    if (! (ioType == imageIO->getTypeId())) {
      throw BadHardwareException(__FILE__": The file type does not match "
				 "the imageIO you have provided");
    }

    int n;
    string imgFile;
    long timestamp;
    Time time;
    while(in) {
      in >> n >> imgFile >> timestamp;
      time.set_msec(timestamp);
      images.push_back(FileInfo(n, imgFile, time));
    }
    in.close();

    if (images.size() == 0) {
      throw BadHardwareException(__FILE__ 
				 ": File does not conatain any images.");
    }

    image = imageIO->read(0, images[0].filename); // momentan wird erstes bild
    // verschluckt (passiert auch in imageprocessing, oder???)
    width = image->getWidth();
    height = image->getHeight();
  }

  FileSource::~FileSource()
  {
    if (imageIO) {
      delete imageIO;
    }
    delete image;
  }

  ImageBuffer 
  FileSource::getImage() throw (HardwareException)
  {
    image = getNextImage(image);
    ImageBuffer buffer = image->getImageBuffer();
    return buffer;
  }

  Image*
  FileSource::getNextImage(Image* image)
  {
    pos = (pos+1) % images.size();
    image = imageIO->read(image, images[pos].filename);
    image->setTimestamp(images[pos].timestamp);
    return image;
  }

  Image*
  FileSource::getPrevImage(Image* image)
  {
    pos = (pos-1+images.size()) % images.size();
    image = imageIO->read(image, images[pos].filename);
    image->setTimestamp(images[pos].timestamp);
    return image;
  }

  string
  FileSource::getFilename() const
  {
    return images[pos].filename;
  }

  Time
  FileSource::getTimestamp() const
  {
    return images[pos].timestamp;
  }


  // Beispiel: Vektor enthält Bild 0 3 7 10
  // Anfrage: Bild 3  -> liefert Bild 3
  // Anfrage: Bild 5  -> liefert Bild 3
  // Anfrage: Bild 16 -> liefert Bild 10
  // Anfrage: Bild -1 -> liefert Bild 0  (!!!!!)
  Image*
  FileSource::getImageNumber(int n, Image* image)
  {
    // dafür sorgen, dass pos aufs richtige oder auf ein späteres zeigt
    while (pos < (int)images.size()-1 && n > images[pos].n) {
      pos++;
    }
    // assertion:  n <= images[pos].n || pos == images.size()-1
    // dafür sorgen, dass pos aufs richtige (wenn da) oder auf das direkt 
    // davor liegende zeigt
    while (pos > 0 && n < images[pos].n) {
      pos--;
    }
    image = imageIO->read(image, images[pos].filename);
    image->setTimestamp(images[pos].timestamp);
    return image;
  }
}
