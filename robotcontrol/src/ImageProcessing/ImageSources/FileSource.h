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

#ifndef _FILESOURCE_H_
#define _FILESOURCE_H_

#include "../ImageSource.h"
#include "../ImageIO.h"
#include <string>
#include <vector>
#include "../../Fundamental/Time.h"

namespace Tribots {

  class FileSource : public ImageSource
  {
  public:
    FileSource(ImageIO* imageIO, const std::string& filename);
    virtual ~FileSource();
    
    /** implements the ImageSource. Uses getNextImage to give the
     *  next image of the stream. At the end of the stream restarts
     *  with the first image. */
    virtual ImageBuffer getImage() throw (HardwareException);
    
    virtual Image* getNextImage(Image* image=0); 
    virtual Image* getPrevImage(Image* image=0);

    virtual Image* getImageNumber(int n, Image* image=0); 
    /** returns the filename of the present image */
    virtual std::string getFilename() const;
    /** returns the timestamp of the present image */
    virtual Time getTimestamp() const;
    
    virtual int getWidth() const { return width; }
    virtual int getHeight() const { return height; }
    
  protected:
    struct FileInfo {
      int n;
      std::string filename;
      Time timestamp;
      FileInfo(int n, const std::string& filename, const Time& timestamp)
	: n(n), filename(filename), timestamp(timestamp) {}
    };

    ImageIO* imageIO;
    int pos;
    std::vector<FileInfo> images;
    
    int width;
    int height;
    
    Image* image;  ///< merkt sich das Bild, muss sich um den Speicher kümmern
  };
}

#endif
