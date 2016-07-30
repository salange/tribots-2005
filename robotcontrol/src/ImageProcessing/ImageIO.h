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

#ifndef _ImageIO_h_
#define _ImageIO_h_

#include "Image.h"
#include "../Structures/TribotsException.h"
#include <string>
#include <iostream>

namespace Tribots {

  using std::ostream;
  using std::istream;

  class ImageIO {
  public:
    virtual ~ImageIO() {};
    virtual void write(const Image& image, const std::string& filename) const
      throw(TribotsException);
    virtual void write(const Image& image, ostream& out) const
      throw(TribotsException) =0;

    /**
     * Liest ein Bild ein. Wenn image != NULL ist, wird versucht, das Bild
     * in den Puffer des übergebenen Bildes zu schreiben. Dies schlägt mit
     * einer Exception fehl, wenn der Puffer nicht die passende Größe und
     * das passende Format hat.
     *
     * Wenn man die Ausmaße des einzulesenden Bildes nicht kennt, übergibt
     * man am besten einen Nullzeiger. Dann wird ein neues, passendes Image
     * angelegt.
     *
     * \param *image Zeiger auf ein Bild, dessen Puffer gefüllt werden soll.
     *               Schlägt fehlt, wenn der Puffer das falsche Format oder die
     *               falsche Größe hat.
     * \param filename Name der Datei, aus der gelesen werden soll.
     * \return Zeiger auf das neu erzeugte, oder auf das übergebene Bild.
     */
    virtual Image* read(Image* image, const std::string& filename) const
      throw(TribotsException);
    virtual Image* read(Image* image, istream& in) const
      throw(TribotsException) =0;

    virtual std::string getDefEnding() const { return ""; }
    virtual std::string getTypeId() const { return ""; }

  };

  /**
   * Implementierung des ImageIO - Interfaces, die Bilder im PPM Format lesen
   * und schreiben kann.
   */
  class PPMIO : public ImageIO {
  public:
    virtual void write(const Image& image, ostream& out) const
      throw(TribotsException);
    /**
     * PPMIO::read liest RGB Bilder aus PPM Dateien. Wird als erstes Argument
     * kein Nullzeiger übergeben, muss das übergebene Image vom Typ RGBImage
     * sein und die richtige Größe haben. Wird ein Nullzeiger übergeben,
     * legt read ein neues RGBImage passender Größe an, dass in den Bezitz des
     * Aufrufers übergeht.
     * 
     * \param *image RGBImage mit passender Größe oder Nullzeiger
     * \param filename Name der Datei, aus der das PPM gelesen werden soll
     * \return Zeiger auf das neu erzeugte, oder auf das übergebene Bild.
     */
    virtual Image* read(Image* image, istream& in) const
      throw(TribotsException);

    virtual std::string getDefEnding() const;
    virtual std::string getTypeId() const;
  };
};

#endif
