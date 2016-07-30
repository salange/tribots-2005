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

#ifndef _image_h_
#define _image_h_

#include "ImageBuffer.h"
#include "../Fundamental/Time.h"

namespace Tribots {

  class ColorClassifier;

  /** Holds a single RGB color value. r,g and b in [0,...,255]. */
  typedef struct {
    unsigned char r; ///< red value in [0,...,255]
    unsigned char g; ///< green value in [0,...,255]
    unsigned char b; ///< blue value in [0,...,255]
  } RGBTuple;

  /** Holds a single YUV color value. 
   *  y in [0,...,255], u and v in [0,...,255]. */
  typedef struct {
    unsigned char y; ///< luminance in [0,...,255]
    unsigned char u; ///< u in [0,...,255]
    unsigned char v; ///< v in [0,...,255]
  } YUVTuple;

  /** Holds a single YUV color value ordered u y v.
   *  y in [0,...,255], u and v in [0,...,255]. */
  typedef struct {
    unsigned char u; ///< u in [0,...,255]
    unsigned char y; ///< luminance in [0,...,255]
    unsigned char v; ///< v in [0,...,255]
  } UYVTuple;

  /** Holds two YUV color values (4:2:2). 
   *  y1 and y2 in [0,...,255], u and v in [0,...,255]. */
  typedef struct {
    unsigned char u; ///< u in [0,...,255]
    unsigned char y1;///< luminance of first pixel in [0,...,255]
    unsigned char v; ///< v in [0,...,255]
    unsigned char y2;///< luminance of second pixel in [0,...,255]
  } UYVYTuple;  

  /**
   * Interface der bei den Tribots verwendeten Bilder. Das Interface 
   * bietet einige Pixelzugriffsmethoden, mit denen die Farbe der 
   * Pixel in verschiedenen Farbräumen gelesen oder gesetzt werden
   * kann. Dabei spielt das verwendete Format (YUV, RGB) des 
   * Bildpuffers beim Zugriff keine Rolle. Es gibt dabei verschiedene
   * Implementierungen, die unterschiedliche Formate für die interne
   * Repräsentierung des Bildes verwenden. Implementierungen
   * besitzt für die Speicherung des Bildes in der Regel keinen 
   * eigenen Speicherbereich sondern dienen lediglich als Wrapper
   * für eine Instanz vom Typ ImageBuffer. Passt dasFormat des
   * übergebenen ImageBuffers allerdings nicht zur ausgewählten
   * Implementierung von Image, so kann es sein, dass der 
   * ursprüngliche Puffer kopiert und konvertiert wird. In einem
   * solchen Fall übernimmt die Instanz vom Typ Image die Verwaltung
   * des neu belegten Speichers.
   *
   * Bei den unterschiedlichen Implementierungen sind die 
   * Zugriffsfunktionen unterschiedlich schnell. Es ist ev. vorteilhaft,
   * wenn die ausgewählte Implementierung (z.B. YUVImage) zu den
   * am häufigsten verwendeten Zugriffen (z.B. getPixelYUV) und zu der
   * verwendeten ImageSource (sollte einen Puffer im gleichen Format
   * liefern) passt. In der Dokumentation der verschiedenen 
   * Iplementierungen werden in der Regel die durschnittlichen 
   * Zugriffszeiten auf einer Referenzmaschine 
   * (P4 2,6 GHz, Hyperthreading) angegeben.
   */
  class Image {
  public:
    /**
     * erzeugt ein leeres Bild und verknüpft dieses mit einem 
     * Farbklassifizierer.
     *
     * \param classifier der zu verwendende ColorClassifier. Wird keiner
     *                   angegeben, so wird der DefaultClassifier 
     *                   verwendet, der immer 0 (COLOR_IGNORE) als Farbklasse
     *                   zurück gibt.
     */
    Image(const ColorClassifier* classifier= 0); 
    /**
     * zerstörrt ein Bild. Wurde bei der Konstruktion ein neuer ImageBuffer 
     * angelegt, wird dieser gelöscht.
     */
    virtual ~Image() {;}

    /**
     * Erzeugt eine exakte Kopie des Bildes mit einem eigenen Speicherbereich.
     */
    virtual Image* clone() const = 0;

    /**
     * liefert die Breite des Bildes
     */
    int getWidth() const;
    /**
     * liefert die Höhe des Bildes
     */
    int getHeight() const;

    /**
     * liefert den mit diesem Bild verknüpften Zeitstempel. Der Zeitstempel
     * wird bei der Konstruktion des Bildes auf die aktuelle Systemzeit 
     * gesetzt, kann aber auch nachträglich durch setTimestamp geändert werden.
     */
    inline const Time& getTimestamp() const;
    /**
     * setzt den mit dem Bild verknüpften Zeitstempel auf die übergebene Zeit
     */
    inline void setTimestamp(const Time& timestamp);

    /**
     * liest den Farbwert des Pixels (x,y) aus und schreibt ihn in das YUVTuple
     *
     * \param x x-Koordinate des auszulesenden Pixels
     * \param y y-Koordinate des auszulesenden Pixels
     * \param yuv Zeiger auf das YUVTuple, in das der Farbwert geschrieben 
     *            werden soll. Es ist nicht erlaubt, einen Nullzeiger 
     *            zu übergeben.
     */
    virtual void getPixelYUV (int x, int y, YUVTuple* yuv) const =0;
    /**
     * liest den Farbwert des Pixels (x,y) aus und schreibt ihn in das 
     * UYVYTuple
     *
     * \param x x-Koordinate des auszulesenden Pixels
     * \param y y-Koordinate des auszulesenden Pixels
     * \param uyvy Zeiger auf das UYVYTuple, in das der Farbwert geschrieben 
     *             werden soll. Es ist nicht erlaubt, einen Nullzeiger 
     *             zu übergeben.
     */
    virtual void getPixelUYVY(int x, int y, UYVYTuple* uyvy) const =0;
    /**
     * liest den Farbwert des Pixels (x,y) aus und schreibt ihn in das RGBTuple
     *
     * \param x x-Koordinate des auszulesenden Pixels
     * \param y y-Koordinate des auszulesenden Pixels
     * \param rgb Zeiger auf das RGB Tuple, in das der Farbwert geschrieben 
     *            werden soll. Es ist nicht erlaubt, einen Nullzeiger 
     *            zu übergeben.
     */
    virtual void getPixelRGB (int x, int y, RGBTuple* rgb) const =0;


    /**
     * setzt den Farbwert des Pixel (x,y) auf den übergebenen Wert
     *
     * \param x x-Koordinate des zu ändernden Pixels
     * \param y y-Koordinate des zu ändernden Pixels
     * \param yuv Farbe, auf die das Pixel gesetzt werden soll
     */
    virtual void setPixelYUV (int x, int y, const YUVTuple& yuv) =0;
    /**
     * setzt den Farbwert des Pixel (x,y) auf den übergebenen Wert
     *
     * \param x x-Koordinate des zu ändernden Pixels
     * \param y y-Koordinate des zu ändernden Pixels
     * \param uyvy Farbe, auf die das Pixel gesetzt werden soll
     */
    virtual void setPixelUYVY(int x, int y, const UYVYTuple& uyvy) =0;
    /**
     * setzt den Farbwert des Pixel (x,y) auf den übergebenen Wert
     *
     * \param x x-Koordinate des zu ändernden Pixels
     * \param y y-Koordinate des zu ändernden Pixels
     * \param rgb Farbe, auf die das Pixel gesetzt werden soll
     */
    virtual void setPixelRGB (int x, int y, const RGBTuple& rgb) =0;

    /**
     * liefert die Farbklasse des Pixels (x,y). Hierzu wird der 
     * mit diesem Bild verknüpfte ColorClassifier verwendet (siehe
     * Konstruktor). Wurde kein Farbklassifizierer angegeben, wird
     * als Default immer 0 (COLOR_IGNORE) zurück gegeben
     *
     * \attention Wenn interne Repräsentierung des Bildes und der 
     *            verwendete ColorClassifier zusammen passen
     *            (z.B. YUVImage und YUVLookupTable), ist dieser
     *            Aufruf in der Regel deutlich schneller als
     *            ein Umweg über 
     *            colorClassifier.lookup(getPixelXYZ(x,y)), da 
     *            Konvertierungen und Speicherkopien vermieden werden
     *            können.
     *
     * \param x x-Koordinate des auszulesenden Pixels
     * \param y y-Koordinate des auszulesenden Pixels
     * \returns Farbklasse des Pixels (siehe ColorClasses.h)
     */     
    virtual unsigned char getPixelClass(int x, int y) const =0;

    /**
     * liefert einen Wert (siehe ImageBuffer), der angibt, wie das Bild
     * intern repräsentiert wird.
     */
    int getNativeFormat() const;

    /**
     * liefert einen Zeiger auf den derzeit mit dem Bild assoziierten 
     * ColorClassifier
     */
    virtual const ColorClassifier* getClassifier() const;   
    /**
     * ändert den derzeit mit dem Bild assoziierten ColorClassifier
     */
    virtual void setClassifier(const ColorClassifier* classifier);

    /**
     * erlaubt den direkten Zugriff auf den vom Image verwendeten Bildpuffer.
     *
     * \attention Änderungen an dem zurückgegeben ImageBuffer gelten auch für
     *            das Image und können hier zu Fehlern führen!
     */
    inline ImageBuffer& getImageBuffer();

    /**
     * setzt alle Randpixel des Bildes auf die Farbe schwarz.
     */
    virtual void setBlackBorder();
    
    /**
     * setzt alle Randpixel des Bildes auf die Farbe weiss.
     */
    virtual void setWhiteBorder();

    /**
     * setzt alle Randpixel des Bildes auf die angegebene Farbe.
     */
    virtual void setBorder(const RGBTuple& rgb);

  protected:
    ImageBuffer buffer;                 ///< ImageBuffer mit dem Bild
    const ColorClassifier* classifier;  ///< der verwendete ColorClassifier
    Time timestamp;                     ///< Zeitstempel des Bildes

  private:
    /**
     * Keine Kopien erlaubt.
     */
    Image(const Image&) { ; }           
    /**
     * Keine Zuweisungen erlaubt.
     */
    Image& operator=(const Image&) {return *this; }
  };

  /**
   * Funktionen zur Umwandlung von einzelnen Pixeln zwischen den verschiedenen
   * Formaten. Die Bildklassen verwenden aus Geschwindigkeitsgründen zum
   * Teil eigene Umwandlungsfunktionen, die generell vorzuziehen sind.
   */
  class PixelConversion {
  public:
    static void convert(const RGBTuple&  rgb,  YUVTuple* yuv);
    static void convert(const YUVTuple&  yuv,  RGBTuple* rgb);
    static void convert(const UYVYTuple& uyvy, YUVTuple* yuv, int pos=0);
    static void convert(const UYVYTuple& uyvy, RGBTuple* rgb, int pos=0);
  };


#define PIXEL_YUV2RGB(y,u,v, r,g,b)                        \
      ((r) = (y)+(((v)*1436)>>10),                         \
       (g) = (y)-(((u)*352+(v)*731)>>10),                  \
       (b) = (y)+(((u)*1814)>>10)) 


#define PIXEL_RGB2YUV(r, g, b, y, u, v)                    \
      ((y) = (306*(r) + 601*(g) + 117*(b))  >> 10,         \
       (u) = ((-172*(r) - 340*(g) + 512*(b)) >> 10) + 128, \
       (v) = ((512*(r) - 429*(g) - 83*(b)) >> 10) + 128)

  // inline //////////////////////////////

  ImageBuffer&
  Image::getImageBuffer()
  {
    return buffer;
  }

  const Time& 
  Image::getTimestamp() const
  { return timestamp; }

  void 
  Image::setTimestamp(const Time& timestamp)
  { this->timestamp=timestamp; }


  inline void 
  PixelConversion::convert(const RGBTuple&  rgb,  YUVTuple* yuv)
  {
    int y, u, v;
    PIXEL_RGB2YUV(rgb.r, rgb.g, rgb.b, y, u, v);
    yuv->y = static_cast<unsigned char>(y <0 ? 0 : (y >255 ? 255 : y ));
    yuv->u = static_cast<unsigned char>(u <0 ? 0 : (u >255 ? 255 : u ));
    yuv->v = static_cast<unsigned char>(v <0 ? 0 : (v >255 ? 255 : v ));
  }

  inline void 
  PixelConversion::convert(const YUVTuple&  yuv,  RGBTuple* rgb)
  {
    int y, u, v, r, g, b;

    y = yuv.y;
    u = yuv.u-128;
    v = yuv.v-128;

    PIXEL_YUV2RGB(y,u,v, r,g,b);
    
    rgb->r = static_cast<unsigned char>(r<0 ? 0 : (r>255 ? 255 : r));
    rgb->g = static_cast<unsigned char>(g<0 ? 0 : (g>255 ? 255 : g));
    rgb->b = static_cast<unsigned char>(b<0 ? 0 : (b>255 ? 255 : b));
  }

  inline void 
  PixelConversion::convert(const UYVYTuple& uyvy, YUVTuple* yuv, int pos) {
    if (pos % 2 == 0) {
      yuv->y = uyvy.y1;
    }
    else {
      yuv->y = uyvy.y2;
    }
    yuv->u = uyvy.u;
    yuv->v = uyvy.v;
  }

  inline void 
  PixelConversion::convert(const UYVYTuple& uyvy, RGBTuple* rgb, int pos) {
    int y, u, v, r, g, b;

    y = (pos%2 == 0) ? uyvy.y1 : uyvy.y2;
    u = uyvy.u-128;
    v = uyvy.v-128;

    PIXEL_YUV2RGB(y,u,v, r,g,b);

    rgb->r = static_cast<unsigned char>(r<0 ? 0 : (r>255 ? 255 : r));
    rgb->g = static_cast<unsigned char>(g<0 ? 0 : (g>255 ? 255 : g));
    rgb->b = static_cast<unsigned char>(b<0 ? 0 : (b>255 ? 255 : b));
  }

}

#endif
