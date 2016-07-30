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

#ifndef _multiimageproducer_h_
#define _multiimageproducer_h_

#include <stdio.h>
#include <vector>
#include "Image.h"
#include "ImageProducer.h"
#include "../Structures/TribotsException.h"
#include "../Fundamental/ConfigReader.h"
#include "ImageMonitor.h"

namespace Tribots {

  class ImageSource;

  /**
   * MultiImageProducer ist ein Container, der mehrere ImageProducer enthält -
   * also eine Fassade zum erzeugen neuer Bilder unabhängig von deren Quelle
   * und Format.
   * Wird nur eine Bildquelle benutzt, so ist MultiImageProducer zu
   * ImageProducer equivalent.
   *
   */
  class MultiImageProducer {
  public:
    /**
     * Initializes the MultiImageProducer and creates all its ImageProducers.
     * Reads from the config the number of ImageProducers that should be constructed
     * and initializes them by calling their constructor with a suffix string which
     * specifies the parts of the config that should be used.
     *
     * \param config ConfigReader, von dem die Parameter gelesen werden sollen
     */
    MultiImageProducer(const ConfigReader& config);

    /**
     * Der Destruktor schließt alle Bildquellen wieder.
     *
     * \attention Bilder, die von einem MultiImageProducer geliefert wurden,
     *            sind nach der Zerstörrung dieses MultiImageProducers unter
     *            Umständen ungültig.
     */
    ~MultiImageProducer();

    /**
     * Liefert das nächste Bild von der ersten Bildquelle. Tritt beim Holen des
     * Bildes ein Hardwarefehler auf, wird die Operation einige Male
     * wiederholt, bevor ggf. ein schwerer (irreperabler) Hardwarefehler
     * ausgelöst wird.
     *
     * \returns das nächste Bild
     */
    Image* nextImage() throw (BadHardwareException);

    /**
     * Liefert das nächste Bild von der durch producer_id spezifizierten
     * Bildquelle. Die producer_id ist durch die Reihenfolge des erstellens
     * bestimmt: die erste Bildquelle hat id = 0 usw.
     * Tritt beim Holen des Bildes ein Hardwarefehler auf, wird die Operation
     * einige Male wiederholt, bevor ggf. ein schwerer (irreperabler) Hardware-
     * fehler ausgelöst wird.
     *
     * \returns das nächste Bild von Imageproducer #producer_id
     */
    Image* nextImage(int producer_id) throw (BadHardwareException);

    /**
     * Returns the number of ImageProducers.
     */
    int numSources(){ return producers.size(); }


  protected:
    std::vector <ImageProducer*> producers;///< Zeiger auf die ImageProducer

  };
}

#endif
