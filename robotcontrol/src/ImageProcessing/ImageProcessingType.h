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


#ifndef tribots_image_processing_type_h
#define tribots_image_processing_type_h

#include "../Structures/TribotsException.h"
#include "../Structures/VisibleObject.h"
#include "Image.h"

namespace Tribots {

  /** abstrakte Klasse zur Modellierung einer Bildfolgenauswertung
      Schnittstelle der Bildfolgenauswertungs-Komponente nach innen */
  class ImageProcessingType {
  public:
    virtual ~ImageProcessingType () throw () {;}

    /** liefere eine Liste aus der Bildquelle erkannter Objekte
	Seiteneffekte: - Bildquelle (Kamera/Simulator/Datei) wird ausgelesen
	               - Liste der erkannten Objekte wird an das Weltmodell uebergeben
                       - ggf. werden Informationen aus dem Weltmodell verwendet */
    virtual void process_image () throw (BadHardwareException) =0;    
    virtual const VisibleObjectList& get_last_seen_objects() const throw()
    { return lastSeenObjects; }

    virtual void request_image_raw() throw(TribotsException) {;}
    virtual void request_image_processed() throw(TribotsException) {;}
    virtual bool is_image_available() throw() { return false; }
    virtual const Image* get_image() throw(TribotsException) { throw TribotsException("No image ready. This feature is not implemented in this ImageProcessingType."); }
    virtual void free_image() throw(TribotsException) {;}

  protected:
    VisibleObjectList lastSeenObjects;
  };

}

#endif

