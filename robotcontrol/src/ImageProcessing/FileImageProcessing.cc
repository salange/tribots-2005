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


#include "FileImageProcessing.h"
#include "ImageProcessingFactory.h"
#include "../Structures/Journal.h"
#include "../WorldModel/WorldModel.h"
#include "../Fundamental/stringconvert.h"

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public ImageProcessingBuilder {
    static Builder the_builder;
  public:
    Builder () {
      ImageProcessingFactory::get_image_processing_factory ()->sign_up (string("File"), this);
    }
    ImageProcessingType* get_image_processing (const std::string&, const ConfigReader& reader, ImageProcessingType*) throw (TribotsException,bad_alloc) {
      return new FileImageProcessing (reader);
    }
  };
  Builder the_builder;
}





FileImageProcessing::~FileImageProcessing () throw () {
  if (reader)
    delete reader;
  if (stream)
    delete stream;
}

FileImageProcessing::FileImageProcessing (const ConfigReader& read) throw (std::bad_alloc, Tribots::InvalidConfigurationException) : stream(NULL), reader(NULL) {
  string fname;
  if (read.get ("visual_info_file", fname)>0) {
    stream = new ifstream (fname.c_str());
    if (!(*stream)) {
      JERROR ("Visual info file: file error");
      delete stream;
      stream=NULL;
    } else {
      reader = new VisibleObjectReader (*stream);
    }
  }
}

void FileImageProcessing::process_image () throw () {
  Time now;
  if (reader) {
    unsigned long int t1, t2;
    VisibleObjectList obj;
    bool success = reader->read_until (t1, t2, obj, now.get_msec());
    if (success)
      MWM.set_visual_information (obj);
  }
}
