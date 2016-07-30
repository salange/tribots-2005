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


#ifndef tribots_world_model_h
#define tribots_world_model_h

#include "WorldModelType.h"
#include "../Structures/TribotsException.h"
#include "../Structures/MessageBoard.h"
#include <iostream>


namespace Tribots {

  class AddComUserInterface;

  /** Schnittsetelle des Weltmodells nach aussen
      Aufgaben: Verwaltung allgemeiner Information, Fortschreiben des Weltzustandes */
  class WorldModel {
  private:
    WorldModelType* the_world_model;
    char* world_model_descriptor;
    const ConfigReader& configuration_list;

    /** eigentliche Implementierung des Weltmodellwechsels 
        Arg1: Bezeichner, Arg2=Parametersatz, Arg3=bisheriges Weltmodell oder NULL */
    void really_change_world_model_type (const char*, const ConfigReader&) throw (TribotsException,std::bad_alloc);

    /** Zeiger auf das zentrale Weltmodell */
    static WorldModel* main_world_model;
  public:
    /** Moeglichkeit, sich eine Referenz auf das zentrale Weltmodell zu besorgen; arbeitet erst korrekt, nachdem ein erstes Weltmodell von ControlLoop erzeugt wurde */
    static WorldModel& get_main_world_model () throw ();
    static bool is_main_world_model_available () throw ();

    /** Konstruktor liest allerlei Parameter aus der Konfigurationsdatei */
    WorldModel (const ConfigReader&) throw (TribotsException,std::bad_alloc);
    ~WorldModel () throw ();


    // Beeinflussung des Weltbildtyps:
    /** Wechsel des Weltmodelltyps
        Weltmodell-Parameter werden aus dem ConfigReader gelesen, der mit dem Konstruktor uebergeben wurde
        Arg1: Bezeichner fuer Weltmodell */
    bool change_world_model_type (const char*) throw ();
    /** Wechsel des Weltmodelltyps
        Arg1: Bezeichner fuer Weltmodell
        Arg2: Parameterliste fuer neues Weltmodell */
    bool change_world_model_type (const char*, const ConfigReader&) throw ();

    /** liefere eine Beschreibung des aktuellen Weltmodelltyps */
    const char* get_world_model_type () const throw ();



    // Informationsgewinnung:
    /** liefere Spielfeldgeometrie */
    inline const FieldGeometry& get_field_geometry () const throw () { return the_world_model->get_field_geometry(); }
    /** liefere Orientierung des Spielfeldes (eigene Haelfte), +1, wenn Spiel von gelbem Tor auf blaues Tor, sonst -1 */
    inline int get_own_half () const throw () { return the_world_model->get_own_half(); }
    /** liefere letzten Spielzustand */
    inline const GameState& get_game_state () const throw () { return the_world_model->get_game_state(); }
    /** liefere Roboterposition, Geschwindigkeit und Orientierung zu einem Zeitpunkt (in Weltkoordinaten); 
	Arg1: Zeitpunkt, Arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    inline const RobotLocation& get_robot_location (Time t, bool ia =true) throw () { return the_world_model->get_robot_location(t, ia); }
    /** liefere Ballposition und Geschwindigkeit zu einem Zeitpunkt (in Weltkoordinaten):
	Arg1: Zeitpunkt, Arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    inline const BallLocation& get_ball_location (Time t, bool ia =true) throw () { return the_world_model->get_ball_location(t, ia); }
    /** liefere Position der Hindernisse;
	Arg1: Zeitpunkt, Arg2: sollen Objektinteraktionen beruecksichtigt werden? */
    inline const ObstacleLocation& get_obstacle_location (Time t, bool ia = true) throw () { return the_world_model->get_obstacle_location(t, ia); }
    /** liefere Robotereigenschaften */
    inline const RobotProperties& get_robot_properties () const throw () { return the_world_model->get_robot_properties(); }
    /** liefere zusätzliche Daten des Robters
	Arg1: Referenz auf Zeitobjekt in das die Zeit geschrieben wird von dem die Daten stammen */
    inline const RobotData& get_robot_data (Time& t) const throw () { return the_world_model->get_robot_data (t);};
    /** liefere eine aus der Selbstlokalisierung geschaetzte Geschwindigkeit 
	Arg1: Referenz auf Zeitobjekt in das die Zeit geschrieben wird von dem die Daten stammen */
    inline const RobotLocation& get_slfilter_robot_location (Time& t) const throw () { return the_world_model->get_slfilter_robot_location (t); }



    // Informationen einfliessen lassen:
    /** Spielrichtung setzen; Arg1: +1, wenn Spiel von gelbem Tor auf blaues Tor, sonst -1 */
    inline void set_own_half (int h) throw () { the_world_model->set_own_half(h); }
    /** Signal der Refereebox einarbeiten */
    inline void update_refbox (RefboxSignal sig) throw () { the_world_model->update_refbox(sig); }
    /** Roboter starten (true)/stoppen(false), veraendern des "in_game"-Flags */
    inline void startstop (bool b) throw () { the_world_model->startstop (b); }
    /** neuen Fahr-/Kickvektor mitteilen
	Arg1: Fahrvektor
	Arg2: Zeitpunkt, zu dem der Fahrvektor gesetzt wurde */
    inline void set_drive_vector (DriveVector dv, Time t) throw () { the_world_model->set_drive_vector(dv,t); }
    /** gemessene Bewegung mitteilen
	Arg1: gemessener Fahrvektor
	Arg2: Zeitpunkt der Messung */
    inline void set_odometry (DriveVector dv, Time t) throw () { the_world_model->set_odometry(dv,t); }
    /** neue Information des visuellen Sensors integrieren
	Arg1: Objektliste */
    inline void set_visual_information (const VisibleObjectList& vol) throw () { the_world_model->set_visual_information(vol); }
    /** neue Information des visuellen Sensors integrieren
	Arg1: Einzelnes Objekt 
	Arg2: Zeitstempel fuer dieses Objekt (Zeitpunkt der Aufnahme) */
    inline void set_visual_information (const VisibleObject& vo, Time t) throw () { the_world_model->set_visual_information(vo,t); }
    /** trage Robotereigenschaften ein */
    inline void set_robot_properties (const RobotProperties& rpr) throw () { the_world_model->set_robot_properties (rpr); }
    /** neue Roboterdaten setzen, diese werden im Moment nicht weitergerechnet 
	Arg1: Roboterdaten
	Arg2: Zeitpunkt von dem die Daten stammen */
    inline void set_robot_data (const RobotData& rd, Time t) throw() {the_world_model->set_robot_data ( rd, t);};
    

    /** Einarbeitung der neuen Informationen in das Weltmodell initiieren */
    inline void update () throw () { the_world_model->update(); }
    /** Weltmodell zuruecksetzen (falls es sich verloren hat) */
    inline void reset () throw () { the_world_model->reset(); }
    /** Weltmodell zuruecksetzen auf bestimmte Selbstlokalisierungsposition */
    inline void reset (const Vec p) throw () { the_world_model->reset(p); }
    /** Weltmodell zuruecksetzen auf bestimmte Selbstlokalisierungsposition und Orientierung */
    inline void reset (const Vec p , const Angle a) throw () { the_world_model->reset(p, a); }


    /** Log-Information in Log-Stream schreiben, siehe auch Define LOUT */
    inline std::ostream& log_stream () throw () { return the_world_model->log_stream (); }
    /** Start einer neuen Iteration mitteilen, nur zu Protokollzwecken 
	Arg1: Zeitpunkt des Zyklusbeginns
	Arg2: erwartete Ausfuehrungszeit des Fahrbefehls */
    inline void init_cycle (Time t1, Time t2) throw () { the_world_model->init_cycle(t1, t2); }
    /** Das MessageBoard ausgehaendigt bekommen zur Kommunikation ueber WLAN mit dem Trainer und anderen Spielern */
    inline MessageBoard& get_message_board () throw () { return the_world_model->get_message_board(); }

  protected:
    /** die Liste erkannter Objekte im Bild in Roboterkoordinaten liefern; diese Methode dient nur dem Zwecke, Linien im Teamcontrol 
     anzeigen zu koennen, um fehlerhafte Einstellungen festzustellen; daher protected mit friend; die Methode ist evtl. nicht
    bei allen Weltmodelltypen sinnvoll implementiert */
    inline const VisibleObjectList& get_visible_objects () throw () { return the_world_model->get_visible_objects(); }

    friend class AddComUserInterface;
    friend class TribotsUdpServer;
  };

}

#define MWM Tribots::WorldModel::get_main_world_model()
#define LOUT Tribots::WorldModel::get_main_world_model().log_stream()



#endif

