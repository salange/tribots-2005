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


#ifndef tribots_world_model_type_h
#define tribots_world_model_type_h

#include "../Structures/FieldGeometry.h"
#include "../Structures/VisibleObject.h"
#include "../Structures/DriveVector.h"
#include "../Structures/RobotLocation.h"
#include "../Structures/BallLocation.h"
#include "../Structures/ObstacleLocation.h"
#include "../Structures/RobotProperties.h"
#include "../Structures/RobotData.h"
#include "../Structures/GameState.h"
#include "../Structures/MessageBoard.h"


namespace Tribots {

  /** abstrakte Klasse als Schnittstelle des Weltmodells nach innen */
  class WorldModelType {
  public:
    virtual ~WorldModelType () throw () {;}

    // Informationsgewinnung:
    /** liefere Spielfeldgeometrie */
    virtual const FieldGeometry& get_field_geometry () const throw () =0;
    /** liefere Orientierung des Spielfeldes (eigene Haelfte), +1, wenn Spiel von gelbem Tor auf blaues Tor, sonst -1 */
    virtual int get_own_half () const throw () =0;
    /** liefere letzten Spielzustand */
    virtual const GameState&  get_game_state () const throw () =0;
    /** liefere Roboterposition, Geschwindigkeit und Orientierung zum Zeitpunkt arg1, arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    virtual const RobotLocation& get_robot_location (Time, bool) throw () =0;
    /** liefere Ballposition und Geschwindigkeit zum Zeitpunkt arg1, arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    virtual const BallLocation& get_ball_location (Time, bool) throw () =0;
    /** liefere Position der Hindernisse zum Zeitpunkt arg1, arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    virtual const ObstacleLocation& get_obstacle_location (Time, bool) throw () =0;
    /** liefere Robotereigenschaften */
    virtual const RobotProperties& get_robot_properties () const throw () =0;
    /** liefere zusaetzliche Daten des Robters
	Arg1: Referenz auf Zeitobjekt in das die Zeit geschrieben wird von dem die Daten stammen */
    virtual const RobotData& get_robot_data (Time&) const throw () =0;
    /** liefere eine aus der Selbstlokalisierung geschaetzte Geschwindigkeit 
	Arg1: Referenz auf Zeitobjekt in das die Zeit geschrieben wird von dem die Daten stammen */
    virtual const RobotLocation& get_slfilter_robot_location (Time&) const throw () =0;


    // Informationen einfliessen lassen:
    /** Spielrichtung setzen; Arg1: +1, wenn Spiel von gelbem Tor auf blaues Tor, sonst -1 */
    virtual void set_own_half (int) throw () =0;
    /** Signal der Refereebox einarbeiten */
    virtual void update_refbox (RefboxSignal) throw () =0;
    /** Roboter starten (true)/stoppen(false), veraendern des "in_game"-Flags */
    virtual void startstop (bool) throw () =0;
    /** neuen Fahr-/Kickvektor mitteilen
	Arg1: Fahrvektor
	Arg2: Zeitpunkt, zu dem der Fahrvektor gesetzt wurde */
    virtual void set_drive_vector (DriveVector, Time) throw () =0;
    /** gemessene Bewegung mitteilen
	Arg1: gemessener Fahrvektor
	Arg2: Zeitpunkt der Messung */
    virtual void set_odometry (DriveVector, Time) throw () =0;
    /** neue Information des visuellen Sensors integrieren
	Arg1: Objektliste */
    virtual void set_visual_information (const VisibleObjectList&) throw () =0;
    /** neue Information des visuellen Sensors integrieren
	Arg1: Einzelnes Objekt 
	Arg2: Zeitstempel fuer dieses Objekt (Zeitpunkt der Aufnahme) */
    virtual void set_visual_information (const VisibleObject&, Time) throw () =0;
    /** trage Robotereigenschaften ein */
    virtual void set_robot_properties (const RobotProperties&) throw () =0;
    /** neue Roboterdaten setzen, diese werden im Moment nicht weitergerechnet 
	Arg1: Roboterdaten
	Arg2: Zeitpunkt von dem die Daten stammen */
    virtual void set_robot_data (const RobotData&, Time) throw() =0;

    /** Einarbeitung der neuen Informationen in das Weltmodell initiieren */
    virtual void update () throw () =0;
 
    /** Weltmodell zuruecksetzen (falls es sich verloren hat) */
    virtual void reset () throw () =0;
    /** Weltmodell zuruecksetzen auf bestimmte Selbstlokalisierungsposition */
    virtual void reset (const Vec) throw () =0;
    /** Weltmodell zuruecksetzen auf bestimmte Selbstlokalisierungsposition und Orientierung */
    virtual void reset (const Vec, const Angle) throw () =0;

    /** Log-Information in Log-Stream schreiben */
    virtual std::ostream& log_stream () throw () =0;
    /** Start einer neuen Iteration mitteilen, nur zu Protokollzwecken 
	Arg1: Zeitpunkt des Zyklusbeginns
	Arg2: Erwarteter Ausfuehrungszeitpunkt des Fahrtbefehls */
    virtual void init_cycle (Time, Time) throw () =0;
    /** Das MessageBoard ausgehaendigt bekommen zur Kommunikation ueber WLAN mit dem Trainer und anderen Spielern */
    virtual MessageBoard& get_message_board () throw () =0;


    /** Liefert die aktuell erkannten Liniensegmente in Roboterkoordinaten, nur zu Testzwecken */
    virtual const VisibleObjectList& get_visible_objects () throw () =0;
   };

}

#endif

