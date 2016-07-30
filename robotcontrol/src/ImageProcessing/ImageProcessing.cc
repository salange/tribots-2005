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


#include "ImageProcessing.h"
#include "ImageProcessingFactory.h"
#include "../Structures/Journal.h"

using namespace Tribots;
using namespace std;

const char* Tribots::ImageProcessing::get_image_processing_type () const throw () { return image_processing_descriptor; }

Tribots::ImageProcessing::~ImageProcessing () throw () {
  delete the_image_processing;
  delete [] image_processing_descriptor;
}

Tribots::ImageProcessing::ImageProcessing (const ConfigReader& vread) throw (TribotsException, bad_alloc) : the_image_processing(NULL), configuration_list(vread) {
  string confline;
  if (vread.get("image_processing_type", confline)<=0) {
    JERROR("no config line \"image_processing_type\" found");
    throw Tribots::InvalidConfigurationException ("image_processing_type");
  }
  really_change_image_processing_type (confline.c_str(), vread);
}

bool Tribots::ImageProcessing::change_image_processing_type (const char* ipt) throw () {
  return change_image_processing_type (ipt, configuration_list);
}

bool Tribots::ImageProcessing::change_image_processing_type (const char* ipt, const ConfigReader& vread) throw () {
  try{
    really_change_image_processing_type (ipt, vread);
  }catch(bad_alloc&){
    JWARNING("Change of image processing type failed due to lack of memory");
    return false;
  }catch(TribotsException&){
    JWARNING("Change of image processing type failed");
    return false;
  }
  return true;
}

void Tribots::ImageProcessing::really_change_image_processing_type (const char* ipt, const ConfigReader& vread) throw (TribotsException, bad_alloc) {
  ImageProcessingType* new_image_processing;
  char* new_descriptor;
  try{
    string ipts (ipt);
    new_image_processing=ImageProcessingFactory::get_image_processing_factory()->get_image_processing(ipts, vread);
  }catch(invalid_argument&){
    throw Tribots::InvalidConfigurationException ("image_processing_type");
  }
  
  new_descriptor = new char [strlen(ipt)+1];
  strcpy(new_descriptor,ipt);
  if (the_image_processing!=NULL) {
    delete the_image_processing;
    delete [] image_processing_descriptor;
  }
  the_image_processing=new_image_processing;
  image_processing_descriptor=new_descriptor;
}

