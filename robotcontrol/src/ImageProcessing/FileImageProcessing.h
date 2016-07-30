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


#ifndef _Tribots_FileImageProcessing_h_
#define _Tribots_FileImageProcessing_h_

#include "ImageProcessingType.h"
#include "../Structures/VisibleObjectReadWriter.h"
#include "../Fundamental/ConfigReader.h"
#include <fstream>

namespace Tribots {

  /** Klasse FileImageProcessing liest Information ueber erkannte Objekte 
      aus einer Datei */
  class FileImageProcessing : public ImageProcessingType {
  public:
    /** Konstruktor */
    FileImageProcessing (const ConfigReader&) throw (std::bad_alloc, Tribots::InvalidConfigurationException);
    /** Destruktor */
    ~FileImageProcessing () throw ();
    /** weitere Objekte aus Datei auslesen */
    void process_image () throw ();

  private:
    std::ifstream* stream;
    VisibleObjectReader* reader;
  };

}

#endif
