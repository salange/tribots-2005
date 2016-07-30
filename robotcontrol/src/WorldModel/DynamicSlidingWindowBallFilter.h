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


#ifndef Tribots_DynamicSlidingWindowBallFilter_h
#define Tribots_DynamicSlidingWindowBallFilter_h

#include "../Fundamental/ConfigReader.h"
#include "../Fundamental/DynamicRingBuffer.h"
#include "../Fundamental/RingBuffer.h"
#include "../Structures/VisibleObject.h"
#include "../Structures/BallLocation.h"
#include "../Structures/RobotLocation.h"
#include <fstream>

namespace Tribots {

  /** Klasse DynamicSlidingWindowBallFilter modelliert eine einfache Ballverfolgung und 
      Geschwindigkeitsschaetzung; Das Verfahren basiert auf der Bildung eines
      kleinste-Quadrate-Schaetzers ueber die letzten n gesehenen Ballpositionen
      bei linearem Bewegungsmodell (gleichfoermige Bewegung); n wird automatisch angepasst */
  // zukuenftige Moeglichkeit, eine gemeinsame Oberklasse fuer verschiedene BallFilter zu verwenden
  class DynamicSlidingWindowBallFilter {
  public:
    /** erzeuge BallFilter; lese Parameter aus arg1 */
    DynamicSlidingWindowBallFilter (const ConfigReader&) throw (std::bad_alloc);
    /** Destruktor */
    ~DynamicSlidingWindowBallFilter () throw ();
    /** uebergebe gemessene Position an Filter;
	arg1=Position des Balls in Weltkoordinaten, 
	arg2=Zeitpunkt der Messung,
	arg3=Position des Roboters in Weltkoordinaten */
    void update (const Vec, const Time, const Vec) throw ();
    /** uebergebe neue Bildinformation an Filter, 
	arg1=gesehener Ballposition, 
	arg2=Roboterposition und -ausrichtung zum Zeitpunkt der Bildinformation */
    void update (const VisibleObjectList&, const RobotLocation&) throw ();
    // Kommunizierten Ball uebergeben (in Wltkoordinaten */
    void comm_ball (Vec) throw ();
    /** liefere mutmasliche Position und Geschwindigkeit zum Zeitpunkt arg1 */
    BallLocation get (Time) const throw ();

  private:
    /** Paar aus Ballposition und Zeitstempel */
    struct PairTimePos {
      Vec pos;           // Position      
      Time timestamp;    // Zeitstempel
    };
    DynamicRingBuffer<PairTimePos> pos_list;   // Liste alter Zeitstempel (Ringpuffer)
    RingBuffer<PairTimePos> pos_for_raised;   // Liste alter Zeitstempel fuer raised Position

    bool first_call;                     // true vor dem ersten Aufruf von new_position
    unsigned int max_buffer_size;        // maximale Groesse des Ringpuffers
    unsigned int min_buffer_size;        // minimale Groesse des Ringpuffers

    Time ref_time;               // Referenz-Zeitpunkt = Zeitpunkt der letzten Bildinformation
    Vec ref_bpos;                // Referenz-Position des Balls in Roboterkoordinaten
    Vec ref_bvel;                // Referenz-Geschwindigkeit des Balls in Roboterkoordinaten

    Time latest_motion_model;    // Zeitpunkt, zu dem letztmalig ein gueltiges Bewegungsmodell bestimmt wurde

    double latest_error;         // Abweichung prognostizierte/gemessene Ballposition bei der vorangegangenen Aktualisierung
    double max_error;            // maximal erlaubter Fehler
    bool ball_raised;            // ist true, falls zuvor erkannt wurde, dass der Ball angehoben wurde

    double raised_hysterese_lower2;     // oberer Schwellwert der Geschwindigkeit zum Quadrat fuer "raised-Analyse"
    double raised_hysterese_upper2;     // unterer Schwellwert der Geschwindigkeit zum Quadrat fuer "raised-Analyse"

    std::ofstream* plog;         // Ausgabestream fuer Debugen

    bool update_motion_model () throw ();               // berechnete Referenz-Ballposition und -geschwindigkeit neu, liefert true falls Berechnung numerisch stabil
    void rescale_ring_buffer (unsigned int) throw ();   // Ringpuffer auf die Groesse arg1 verkleinern (arg1>=1)
    bool inside_field (Vec) const throw ();             // prueft, ob die Position innerhalb des Spielfeldes (einschlieslich Rand) liegt

    Vec comm_ball_pos;           // letzter Kommunizierter Ball
    Time comm_ball_pos_time;     // Zeitpunkt der letzten Kommunikation
  };

}

   
#endif

