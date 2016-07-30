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

#include "ImageIO.h"
#include "ImageBuffer.h"
#include "Images/RGBImage.h"

#include <fstream>

namespace Tribots {
  
  using namespace std;

  void
  ImageIO::write(const Image& image, const std::string& filename) const
    throw(TribotsException)
  {
    ofstream out(filename.c_str());
    if (!out) {
      throw TribotsException(__FILE__": could not open file for output.");
    }

    write(image, out);

    out.close();
  }

  string
  PPMIO::getDefEnding() const 
  {
    return ".ppm";
  }

  string
  PPMIO::getTypeId() const 
  {
    return "PPM";
  }

  void
  PPMIO::write(const Image& image, ostream& out) const
    throw(TribotsException)
  {
    ImageBuffer& imgBuf = const_cast<Image&>(image).getImageBuffer();

    // write header

    out << "P6" << endl;
    out << "# CREATOR: PPMIO.h, robotcontrol\n";
    out << image.getWidth() << " " << image.getHeight() << " " 
	<< "255" << "\n";

    // write binary data
    if (image.getNativeFormat() == ImageBuffer::FORMAT_RGB) {
      out.write(reinterpret_cast<char*>(imgBuf.buffer), imgBuf.size);
    }
    else {
      ImageBuffer newBuf(image.getWidth(), image.getHeight(), 
			 ImageBuffer::FORMAT_RGB, 
			 new unsigned char[image.getWidth() * 
					   image.getHeight() * 3],
			 image.getWidth() * image.getHeight() * 3);
      ImageBuffer::convert(imgBuf, newBuf);
      out.write(reinterpret_cast<char*>(newBuf.buffer), newBuf.size);
      delete [] newBuf.buffer;
    }    
  }

  /* skips spaces and recursivly multi-line comments on a given stream. */
  static void skip(istream& in);

  Image*
  ImageIO::read(Image* image, const std::string& filename) const
    throw(TribotsException)
  {
    ifstream in(filename.c_str());
    if (!in) {
      throw TribotsException(__FILE__": could not open file for input.");
    }
    Image* img = read(image, in);
    in.close();
    return img;
  }

  Image*
  PPMIO::read(Image* image, istream& in) const
    throw(TribotsException)
  {
    // read header
    int type, maxCol;
    int width, height;
    char c;

    skip(in);
    if (!in.get(c)) {
      throw TribotsException(__FILE__ ": Unexpected end of file.");
    }
    if (c != 'P' && c != 'p') {
      throw TribotsException(__FILE__ ": Not a PPM file.");    
    }
    
    in >> type;
    if (type != 6) {      // binary format?
      throw TribotsException(__FILE__ ": Not a binary PPM.");
    }

    skip(in);
    in >> width;
    skip(in);
    in >> height;
    skip(in);
    in >> maxCol;            // will be ignored

    in.ignore(1);

    if (image != 0) {
      if (image->getWidth() != width || image->getHeight() != height) {
	throw TribotsException(__FILE__ ": Given image of wrong dimensions. "
			       "Use a NULL-pointer instead.");
      }
    }
    else {
      image = new RGBImage(width, height);
    }

    // read binary data
    if (image->getNativeFormat() == ImageBuffer::FORMAT_RGB) {
      in.read(reinterpret_cast<char*>(image->getImageBuffer().buffer), 
	      image->getImageBuffer().size);
    }
    else {
      ImageBuffer newBuf(width, height, 
			 ImageBuffer::FORMAT_RGB, 
			 new unsigned char[width * height * 3],
			 width * height * 3);
      in.read(reinterpret_cast<char*>(newBuf.buffer), newBuf.size);
      ImageBuffer::convert(newBuf, image->getImageBuffer());
      
      delete [] newBuf.buffer;
    }    
    
    return image;
  }

  /* skips spaces and recursivly multi-line comments on a given stream.
   */
  static void skip(istream& in)
  {
    char c;
    do {
      if (!in.get(c)) return;
    } while(isspace(c) || c=='\n' || c=='\r');
    if (c=='#') {                  // start of a comment
      do {                         // look for the end of the line
	if (!in.get(c)) return;
    } while(c!='\n');
      skip(in);                    // skip again at start of next line.
    }
    else                           // c was neither comment(#) nor space
      in.putback(c);               // so better put it back,
  }
}
