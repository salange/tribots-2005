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

#ifndef _linedetector_h_
#define _linedetector_h_

#include "../Structures/TribotsException.h"
#include "Image2WorldMapping.h"
#include "../Fundamental/Time.h"
#include "../Structures/VisibleObject.h"

namespace Tribots {

  class ScanResult;
  class Transition;
  class FieldMapper;
  class Image;

  class LineDetector {
  public:
    /**
     * Erzeugt und initialisiert einen Liniendetektoren mit der �bergebenen
     * Koordinatentransformation (Bild->Welt) und den �bergebenen Werten f�r
     * die minimale und maximale Breite von Linien.
     *
     * \param mapping Koordinatentransformation von Bild zu egozentrischen
     *                Weltkoordinaten
     * \param minLineWidth �berg�nge, die dichter beieinander liegen, werden
     *                     aussortiert (Wert im Weltkoordinatensystem)
     * \param maxLineWidth �berg�nge, die weiter auseinander liegen, werden
     *                     aussortiert (Wert im Weltkoordinatensystem)
     */
    LineDetector(const CoordinateMapping* mapping, 
    		 FieldMapper * fieldmapper,
		 double minLineWidth,
		 double maxLineWidth,
		 bool use2Steps = false,
		 bool checkForGreen = false,
		 bool cutOutsideField = false,
		 bool useMaximalDistance = false,
		 double maximalDistance = 3000.,
		 bool useHalfField = false,
		 bool useFieldMapper =false);

    /**
     * Findet Linien in den �bergebenen Resultaten des Linienscanners und
     * �bergibt die gefundenen Punkte als VisibleObject an das Weltmodell.
     * Eine Linie besteht hier immer aus zwei �berg�ngen (zur und von der
     * Line). F�r jede Paarung wird die Methode checkLinePlausibility 
     * aufgerufen, die verschiedene Plausibilit�tstest durchf�hrt und so
     * unwahrscheinliche �berg�nge herausfiltert.
     *
     * \attention �ndert �ber einen Seiteneffekt das Weltmodell.
     *
     * \param scanResults ScanResult Struktur der Farbklasse COLOR_LINE
     * \param vol Wird hier eine VisibleObjectList �bergeben, werden die
     *            Objekte nicht nur ins Weltmodell sondern auch an diese Liste 
     *            angeh�ngt.
     * \param writeWM gibt an, ob die gefundenen Objekte ins Weltmodell
     *                geschrieben werden sollen
     */
    void searchLines(const ScanResult* scanResult, Time time = Time(),
		     VisibleObjectList* vol=0, bool writeWM = true)
      throw (TribotsException);

    /**
     * Untersucht, ob es sich bei den zwei �bergebenen �berg�ngen um die 
     * Kanten einer Line handlet. Derzeitige Annahmen:
     *
     * 0. Durch die �bergabe der ScanResults einer bestimmten Farbklasse:
     *    Die Linien sind weiss (COLOR_LINE).
     *
     * 1. Die Farbklasse vor und nach der Linie ist die gleiche.
     *
     * 2. Der Abstand (egozentrische Weltkoordinaten) zwischen den beiden 
     *    �berg�ngen darf eine Minimalbreite nicht unter- und eine 
     *    Maximalbreite nicht �berschreiten (Um zum Beispiel Poster und 
     *    Werbebanner auszusortieren). Hierzu wird die 
     *    Koordinatentransformation Bildkoordinaten-nach-relative 
     *    Weltkoodinaten ben�tigt.
     *
     * \param transStart �bergang, bei dem die Linie anfangen soll
     * \parma transEnd   �bergang, bei dem die Linie enden soll
     * \returns true, wenn die beiden potentiellen Linien�berg�nge den
     *          Plausibilit�tstest bestanden haben
     */
    bool checkLinePlausibility(const Transition& transStart,
			       const Transition& transEnd,
			       const Time& time) const;

    void setVisualize(Image* vis) { this->vis=vis; }

  protected:
    const CoordinateMapping* mapping;  ///< Koordinatentransformation
    FieldMapper * fieldmapper;
    double minLineWidth;      ///< unterer Schwellwert f�r den Abstand
    double maxLineWidth;      ///< oberer Schwellwert f�r den Abstand
    bool use2Steps;
    bool checkForGreen;
    bool cutOutsideField;
    bool useHalfField;
    bool useMaximalDistance;
    double maximalDistance;
    bool useFieldMapper;
    Image* vis;

    void visualize(const Vec& line) const;
  };

};


#endif
