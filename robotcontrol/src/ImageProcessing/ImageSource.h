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

#ifndef _imagesource_h_
#define _imagesource_h_

#include "../Structures/TribotsException.h"
#include "ImageBuffer.h"

namespace Tribots {

  /**
   * Interface defining a source of images. It is implemented by 
   * "drivers" for different devices like IEEE1394 cameras or 
   * video4linux frame grabbers. An ImageSource always returns
   * a plain image buffer. To get higher level access methods,
   * it is possible to wrap implementations of the Image Interface
   * around these plain buffers.
   */
  class ImageSource 
  {
  public:
    
    /**
     * get the next image from the image source. No image should 
     * be returned more than once.
     *
     * \returns an ImageBuffer holding the plain image. The memory
     *          of the ImageBuffer returned is owned by the image
     *          source an may change during the next call of getImage
     */
    virtual ImageBuffer 
    getImage() throw (HardwareException, TribotsException)=0;

    /**
     * get the width of the images that this image source returns
     */
    virtual int getWidth() const=0;
    /**
     * get the height of the images that this image source returns
     */
    virtual int getHeight() const=0;

    /**
     * try to change the frame rate. Returns true on success and false, if
     * the frame rate remains unchanged.
     */
    virtual bool setFramerate(int fps) { return false; }

    /** 
     * get the delay in ms of the image source
     */
    virtual int getDelay () const throw () { return 0; }
  };
};


#endif /* _imagesource_h_ */
