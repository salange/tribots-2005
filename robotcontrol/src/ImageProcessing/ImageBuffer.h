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

#ifndef _imagebuffer_h_
#define _imagebuffer_h_

#include "../Structures/TribotsException.h"

namespace Tribots {

  /**
   * "D�nne" Schicht, die um die Bildpuffer, die von den Bildquellen geliefert
   * werden, gewickelt werden kann. Die Puffer werden so mit einem Tag 
   * versehen, das angibt, in welchem Format die Daten in dem Puffer 
   * organisiert sind. Au�erdem sind Informationen �ber die Gr��e des Puffers
   * und die Zeilen und Spaltenanzahl des Bildes enthalten.
   *
   * Diese Klasse ist die gemeinsame Schnittstelle zwischen den Bildquellen 
   * und den verschiedenen Bildklassen. Sofern der Benutzer die Fassade
   * ImageProducer verwendet, kommt er mit dieser Klasse nicht in Ber�hrung.
   *
   * \attention Diese Klasse sollte nicht direkt vom Benutzer verwendet werden.
   *            Bilder erh�lt man von der Klasse ImageProducer, die Bilder vom
   *            Typ Image zur�ckgibt.
   */
  class ImageBuffer 
  {
  public: 
    enum {
      FORMAT_YUV444=0,
      FORMAT_YUV422,
      FORMAT_YUV411,
      FORMAT_RGB,
      FORMAT_MONO,
      FORMAT_MONO16,
      FORMAT_UYV
    };      

    int width;              ///< Breite des Bildes
    int height;             ///< H�he des Bildes
    int format;             ///< Format des Bildes (FORMAT_YUV444,...)
    unsigned char* buffer;  ///< Zeiger auf den Speicherbereich
    int size;               ///< Gr��e des Speicherbereichs in byte

    /**
     * Erzeugt eine neue Instanz von ImageBuffer um den �bergebenen Puffer
     * buffer. Der Puffer muss bereits alloziert und initialisiert worden sein.
     * Eine Plausibili�tspr�fung der Parameter width, height, format und size
     * geschieht nicht.
     *
     * \param width Breite des Bildes (in Pixeln)
     * \param height H�he des Bildes (in Pixeln)
     * \param format Datenformat des Puffers (FORMAT_YUV444=0, FORMAT_YUV422,
     *        FORMAT_YUV411, FORMAT_RGB, FORMAT_MONO, FORMAT_MONO16, 
     *        FORMAT_UYV)
     * \param buffer Zeiger auf den Speicherbereich, der das Bild enth�lt
     * \param size Gr��e des Speicherbereichs in byte
     */
    ImageBuffer(int width=0, int height=0, int format=0, 
		unsigned char* buffer=0, int size=0);

    /**
     * Konvertiert den Inhalt eines Puffers in das Datenformat eines anderen
     * Puffers. Der Benutzer hat selbst daf�r zu sorgen, dass die Puffergr��en
     * der �bergebenen ImageBuffer zueinander passen.
     *
     * Diese Methode sucht eine passende Konvertierungsfunktion aus einer
     * zentralen Registrierung. Ist eine ben�tigte Konvertierung nicht 
     * implementiert, schl�gt der Aufruf mit einer Exception fehl.
     *
     * \param src Der Puffer, von dem kopiert werden soll.
     * \param dst Der Puffer, in den geschrieben werden soll. Das Datenformat
     *            des Puffers wird beachtet, die Gr��e muss zu src passen.
     */
    static void convert(const ImageBuffer& src, ImageBuffer& dst) 
      throw (TribotsException);			
  };

}

#endif
