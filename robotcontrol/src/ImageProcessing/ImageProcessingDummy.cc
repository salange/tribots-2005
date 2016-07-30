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


#include "ImageProcessingDummy.h"
#include "ImageProcessingFactory.h"

using namespace Tribots;
using namespace std;


// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public ImageProcessingBuilder {
    static Builder the_builder;
  public:
    Builder () {
      ImageProcessingFactory::get_image_processing_factory ()->sign_up (string("Dummy"), this);
    }
    ImageProcessingType* get_image_processing (const std::string&, const ConfigReader& reader, ImageProcessingType*) throw (TribotsException,bad_alloc) {
      return new ImageProcessingDummy;
    }
  };
  Builder the_builder;
}





void ImageProcessingDummy::process_image () throw () {
  VisibleObjectList vol;
  WorldModel::get_main_world_model().set_visual_information (vol);
}
