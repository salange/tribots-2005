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

#ifndef _imageproducer_h_
#define _imageproducer_h_

#include "Image.h"
#include "../Structures/TribotsException.h"
#include "../Fundamental/ConfigReader.h"
#include "ImageMonitor.h"

#include <string>

namespace Tribots {
  
  using std::string;

  class ImageSource;

  /**
   * Die Klasse ImageProducer ist eine Fassade für die verschiedenen
   * Bildquellen und Bildumwandlungsmethoden. Der ImageProducer kennt alle
   * zur Verfügung stehenden Implementierungen (sowohl alle von ImageSource
   * abgeleiteten Klassen, als auch alle verfügbaren Bildklassen, die von
   * Image abgeleitet wurden) und weiß wie sie zu erzeugen und zu konfigurieren
   * sind. Die zu verwendenden Implementierungen werden in einer 
   * Konfigurationsdatei ausgewählt und eingestellt.
   *
   * Ein Benutzer sollte lediglich diese Klasse und das Interface Image
   * kennen müssen (und natürlich die möglichen Einstellungen in der
   * Konfigurationsdatei ;).
   *
   * Um mehrere Image-quellen gleichzeitig benutzen zu können, kann dem
   * Konstruktor optional ein postfix string übergeben werden, mit dem
   * der konkrete Bereich der Configdateien für den jeweiligen Producer
   * festgelegt werden kann.
   */
  class ImageProducer {
  public:
    /**
     * Der Konstruktor liest die Konfiguration vom übergebenen ConfigReader.
     * An Hand der Konfiguration wird entschieden, welche Bildquelle 
     * instantiert und welches Bildformat von der Bildklasse verwendet wird.
     *
     * \param config ConfigReader, von dem die Parameter gelesen werden sollen
     */
    ImageProducer(const ConfigReader& config) 
      throw(TribotsException);

    /** Konstruktor, der die Angabe der image_source (Kamera) und des
     *  image_handlers (ImageProducer settings) im image_producer.cfg config
     *  file erlaubt. Es werden die sections mit den entsprechenden Namen
     *  ausgewertet.
     */
    ImageProducer(const ConfigReader& config, 
		  const std::string& image_source, 
		  const std::string& image_handler)      
      throw(TribotsException);


    /**
     * Der Destruktor schließt die Bildquelle wieder. 
     *
     * \attention Bilder, die von einem ImageProducer geliefert wurden,
     *            sind nach der Zerstörrung dieses ImageProducers unter 
     *            Umständen ungültig.
     */
    ~ImageProducer();

    /**
     * Liefert das nächste Bild von der Bildquelle. Tritt beim Holen des
     * Bildes ein Hardwarefehler auf, wird die Operation einige Male 
     * wiederholt, bevor ggf. ein schwerer (irreperabler) Hardwarefehler
     * ausgelöst wird.
     *
     * \returns das nächste Bild
     */
    Image* nextImage() throw (BadHardwareException, TribotsException);

    enum { TYPE_RGBImage=0, TYPE_UYVYImage, TYPE_YUVImage };

    /** Versucht eine Bildrate zu setzen. Falls dies gelungen ist, wird
     *  true zurückgeliefert. Die Funktionsweise und die zur Verfügung 
     *  stehenden Frameraten sind vom Gerät (ImageSource) abhängig.
     */
    bool setFramerate(int fps);

  protected:
    /** Used by both Constructors to do the actual initialization of the
     *  ImageProducer.
     */
    void init(const ConfigReader& config, 
	      const string& image_source, const string& image_handler);

    ImageSource* imgSource;///< Zeiger auf die aktive Bildquelle
    ColorClassifier* classifier;///< Zeiger auf den verwendeten Klassifikator
    ImageMonitor* monitor; ///< Zeiger auf den zu verwendenden Monitor
    int failCount;         ///< Anzahl misslungener Zugriffe auf die Bildquelle
    int retriesMax;        ///< Höchstzahl der erfolglosen Wiederholungen
    int imageType;         ///< zu verwendender Bildtyp (TYPE_RGBImage,...)

    int camera_delay;      ///< Zeitverzoegerung der Kamera in ms

    ImageBuffer buffer;	   ///< Used to buffer the images in nextImage(); ???
    bool fromFile;
  };
}


#endif
