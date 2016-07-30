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
   * Die Klasse ImageProducer ist eine Fassade f�r die verschiedenen
   * Bildquellen und Bildumwandlungsmethoden. Der ImageProducer kennt alle
   * zur Verf�gung stehenden Implementierungen (sowohl alle von ImageSource
   * abgeleiteten Klassen, als auch alle verf�gbaren Bildklassen, die von
   * Image abgeleitet wurden) und wei� wie sie zu erzeugen und zu konfigurieren
   * sind. Die zu verwendenden Implementierungen werden in einer 
   * Konfigurationsdatei ausgew�hlt und eingestellt.
   *
   * Ein Benutzer sollte lediglich diese Klasse und das Interface Image
   * kennen m�ssen (und nat�rlich die m�glichen Einstellungen in der
   * Konfigurationsdatei ;).
   *
   * Um mehrere Image-quellen gleichzeitig benutzen zu k�nnen, kann dem
   * Konstruktor optional ein postfix string �bergeben werden, mit dem
   * der konkrete Bereich der Configdateien f�r den jeweiligen Producer
   * festgelegt werden kann.
   */
  class ImageProducer {
  public:
    /**
     * Der Konstruktor liest die Konfiguration vom �bergebenen ConfigReader.
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
     * Der Destruktor schlie�t die Bildquelle wieder. 
     *
     * \attention Bilder, die von einem ImageProducer geliefert wurden,
     *            sind nach der Zerst�rrung dieses ImageProducers unter 
     *            Umst�nden ung�ltig.
     */
    ~ImageProducer();

    /**
     * Liefert das n�chste Bild von der Bildquelle. Tritt beim Holen des
     * Bildes ein Hardwarefehler auf, wird die Operation einige Male 
     * wiederholt, bevor ggf. ein schwerer (irreperabler) Hardwarefehler
     * ausgel�st wird.
     *
     * \returns das n�chste Bild
     */
    Image* nextImage() throw (BadHardwareException, TribotsException);

    enum { TYPE_RGBImage=0, TYPE_UYVYImage, TYPE_YUVImage };

    /** Versucht eine Bildrate zu setzen. Falls dies gelungen ist, wird
     *  true zur�ckgeliefert. Die Funktionsweise und die zur Verf�gung 
     *  stehenden Frameraten sind vom Ger�t (ImageSource) abh�ngig.
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
    int retriesMax;        ///< H�chstzahl der erfolglosen Wiederholungen
    int imageType;         ///< zu verwendender Bildtyp (TYPE_RGBImage,...)

    int camera_delay;      ///< Zeitverzoegerung der Kamera in ms

    ImageBuffer buffer;	   ///< Used to buffer the images in nextImage(); ???
    bool fromFile;
  };
}


#endif
