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

#ifndef _linescanning_h_
#define _linescanning_h_

#include <vector>

#include "../Structures/TribotsException.h"
#include "../Fundamental/geometry.h"
#include "../Fundamental/Vec.h"
#include "ScanLines.h"
#include "Image.h"
#include "ColorClasses.h"

namespace Tribots {

  /**
   * Diese Stuktur enth�lt Informationen �ber im Bild gefundene Farb�berg�nge.
   * Jede Transition ist entweder vom Typ START oder vom Typ END. Der Typ
   * gibt an, ob es sich - in der Richtung, mit der eine Scanlinie 
   * abgeschritten wurde - um den Anfang oder das Ende einer Region handelt.
   */
  struct Transition {
    int type;         ///< START: start of the region, END: end of the region
    int from;         ///< class of the first pixel's color
    int to;           ///< class of the second pixel's color
    int twoStep;      ///< START: class of pixel two steps back, END: class of pixel two Steps ahead
    Vec fromPos;      ///< x-y-coordinates of the first pixel (image coords)
    Vec toPos;        ///< x-y-coordinates of the second pixel (image coords)
    bool virt;        ///< whether or not this transition is a virtual one

    /**
     * Konstruiert eine Transition.
     *
     * \param type    gibt den Typ an (START oder END)
     * \param from    Farbklasse des letzten Pixels vor dem �bergang
     * \param to      Farbklasse des ersten Pixels nach dem �bergang
     * \param fromPos Koordinaten des letzten Pixels vor dem �bergang
     * \param toPos   Koordinaten des letzten Pixels nach dem �bergang
     */
    inline Transition(int type, int from, int to, int twoStep,
		      const Vec& fromPos, const Vec& toPos, bool virt=false);
    enum { START=0, END };
  };
    
  /**
   * Diese Struktur enth�lt die Resultate des Scanvorgangs f�r eine einzelne
   * Farbklasse. Gespeichert werden (je nach Einstellung in der ColorClassInfo)
   * �berg�nge und Punkte (Koordinaten der Pixel dieser Farbklasse).
   */
  struct ScanResult {
    int id;                     ///< Farbklasse, auf die sich bezogen wird
    std::vector <Transition> transitions; ///< gefundene Farb�berg�nge
    std::vector <Vec> points;   ///< gefundene Punkte dieser Farbklasse

    /**
     * Konstruktor, der ein Resultatobjekt f�r die angegebene Farbklasse
     * erzeugt.
     */
    inline ScanResult(int id);  
    /**
     * l�scht alle gefundenen Punkte und �berg�nge
     */
    void clear(); 
  };

  class ScanResultList {
  public:
    std::vector<ScanResult*> results;  ///< Liste von ScanResult Strukturen
    
    /**
     * Konstruktor, der eine Liste f�r n Klassen anlegt.
     */
    ScanResultList(int n);

    /**
     * Destruktor.
     */
    ~ScanResultList();

    /**
     * l�scht alle gefundenen Punkte und �berg�nge
     */
    void clear();
  };
      

  class LineScanner {
  public:
    /**
     * Initialisiert einen Linienscanner mit einer Menge von Linien
     * und f�r die �bergebenen Farbklasseninformationen.
     *
     * \param lines Scanlinien, Instanz geht in den Besitz des Linienscanners
     *              �ber und kann nicht mehr f�r einen anderen Scanner 
     *              verwendet werden.
     * \param colClasses Informationen �ber die zu suchenden Farbklassen.
     *                   Objekt geht nicht in den Besitz des Linienscanners
     *                   �ber und muss daher extern gel�scht werden - 
     *                   allerdings nicht vor dem Linienscanner!
     */
    LineScanner(ScanLines* lines, 
		const ColorClassInfoList* colClasses);
    ~LineScanner();

    /**
     * Scannt ein Bild und zeichnet die gefunden �berg�nge und Pixel in der
     * �bergebenen ScanResultList auf. Die gefundenen Informationen werden
     * an die bereits in der ScanResultList enthaltenen Daten angeh�ngt!
     */
    void scan(const Image& image, ScanResultList* results)
      throw (TribotsException);

    /** 
     * �bergibt das Bild, in das die beim n�chsten Aufruf von scan 
     * gefundenen �berg�nge eingezeichnet werden sollen.
     */
    void setVisualize(Image* vis) { this->vis = vis; }

  protected:
    ScanLines* lines;    ///< Liste von Scanlinien 
    const ColorClassInfoList* colClasses;///< Zeiger zur Farbklasseninformation
    Image* vis;

    void visualize(const ScanResultList* results);
  };


  // inline /////////////////////////////////////////////////////////

  Transition::Transition(int type, int from, int to, int twoStep,
			 const Vec& fromPos, const Vec& toPos, bool virt)
    : type(type), from(from), to(to), twoStep(twoStep), fromPos(fromPos), 
      toPos(toPos), virt(virt)
  {}
  
  ScanResult::ScanResult(int id)
    : id(id)
  {}
   
}

#endif
