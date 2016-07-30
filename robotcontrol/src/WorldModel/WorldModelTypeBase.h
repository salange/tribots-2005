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


#ifndef tribots_world_model_type_base_h
#define tribots_world_model_type_base_h

#include "WorldModelType.h"
#include "../Fundamental/ConfigReader.h"
#include "GameStateManager.h"
#include <fstream>


namespace Tribots {

  class LocationShortTimeMemory;

  /** Implementierung einiger Funktionen des WorldModelTypes wie Verwlatung der Feld- und Roboterdaten */
  class WorldModelTypeBase : public WorldModelType {
  protected:
    FieldGeometry    field_geometry;    ///< die Feldgeometrie
    GameStateManager gsman;             ///< die Verwaltungsstruktur fuer GameStates
    MessageBoard     mboard;            ///< das Messageboard
    int              own_half;          ///< eigene Haelfte: +1 , wenn Spiel von gelbem Tor auf blaues Tor, sonst -1
    RobotProperties  robot_properties;  ///< Robotereigenschaften
    RobotData        robot_data;        ///< Zusätzliche Daten des Roboters (Temperatur der Motoren, Ströme, usw.)
    Time             robot_data_time;  
    VisibleObjectList   empty_vol;

    LocationShortTimeMemory* locations; ///< Pufferspeicher fuer Robot-, Ball- und ObstacleLocation

    std::ofstream    null_stream;       ///< Stream auf /dev/null

    virtual void update_game_state () throw ();        ///< der Teil des Updates, der sich um den Gamestate kuemmert
    virtual void update_localisation () throw () =0;   ///< der Teil des Updates, der sich um Roboter-, Ball- und Hindernisposition kuemmert 

  public:
    WorldModelTypeBase (const ConfigReader&) throw (InvalidConfigurationException, std::bad_alloc);
    ~WorldModelTypeBase () throw ();

    // Informationsgewinnung:
    /** liefere Spielfeldgeometrie */
    const FieldGeometry& get_field_geometry () const throw ();
    /** liefere Orientierung des Spielfeldes (eigene Haelfte), +1, wenn Spiel von gelbem Tor auf blaues Tor, sonst -1 */
    int get_own_half () const throw ();
    /** liefere letzten Spielzustand */
    const GameState& get_game_state () const throw ();
    /** liefere Robotereigenschaften */
    const RobotProperties& get_robot_properties () const throw ();
    /** liefere zusätzliche Daten des Robters
	Arg1: Referenz auf Zeitobjekt in das die Zeit geschrieben wird von dem die Daten stammen */
    const RobotData& get_robot_data (Time&) const throw ();
    /** Dummy-Implementierung, tut nichts, liefert stets den Nullvektor */
    const RobotLocation& get_slfilter_robot_location (Time&) const throw ();

    // Im WorldModelTypeBase werden die Objektinteraktionen untersucht sowie eine 
    // Zwischenspeicherung fuer haeufig abgefragte Zeitpunkte realisiert
    // Dazu werden die Methoden get_robot_location, get_ball_location und 
    // get_obstacle_location realisiert. Um an die nicht-interagierten Positionen zu kommen, 
    // wird auf die Methoden get_robot, get_ball und get_obstacles zugegriffen
    virtual RobotLocation get_robot (Time) const throw () =0;
    virtual BallLocation get_ball (Time) const throw () =0;
    virtual ObstacleLocation get_obstacles (Time) const throw () =0;

    const RobotLocation& get_robot_location (Time, bool) throw ();
    const BallLocation& get_ball_location (Time, bool) throw ();
    const ObstacleLocation& get_obstacle_location (Time, bool) throw ();
    virtual Time get_timestamp_latest_update () const throw () =0;   ///< liefere Zeitpunkt, fuer den die Selbstlokalisation zum letzten Mal aktualisiert wurde

    // Informationen einfliessen lassen:
    /** Spielrichtung setzen; Arg1: +1, wenn Spiel von gelbem Tor auf blaues Tor, sonst -1 */
    void set_own_half (int) throw ();
    /** Signal aus der Refereebox einbinden */
    void update_refbox (RefboxSignal) throw ();
    /** Roboter starten (true)/stoppen(false), veraendern des "in_game"-Flags */
    void startstop (bool) throw ();
    /** trage Robotereigenschaften ein */
    void set_robot_properties (const RobotProperties&) throw ();
    /** neue Roboterdaten setzen, diese werden im Moment nicht weitergerechnet 
	Arg1: Roboterdaten
	Arg2: Zeitpunkt von dem die Daten stammen */
    void set_robot_data (const RobotData&, Time) throw();

    void reset () throw () =0;
    /** Weltmodell zuruecksetzen auf bestimmte Selbstlokalisierungsposition */
    void reset (const Vec) throw () =0;
    /** Weltmodell zuruecksetzen auf bestimmte Selbstlokalisierungsposition; verwirft die Orientierung */
    void reset (const Vec, const Angle) throw ();

    /** Einarbeitung der neuen Informationen in das Weltmodell initiieren */
    void update () throw ();

    /** Log-Information verwerfen (liefert geschlossenen Stream auf /dev/null) */
    std::ostream& log_stream () throw ();
    /** Start einer Iteration mitteilen, nur zu Protokollzwecken 
	Arg1: Zeitpunkt des Zyklusbeginns
	Arg2: Erwarteter Ausfuehrungszeitpunkt des Fahrtbefehls */
    void init_cycle (Time, Time) throw ();
    /** Das MessageBoard ausgehaendigt bekommen zur Kommunikation ueber WLAN mit dem Trainer und anderen Spielern */
    MessageBoard& get_message_board () throw ();

    /** Dummy-Implementierung, liefert eine leere Liste */
    const VisibleObjectList& get_visible_objects () throw () { return empty_vol; }
    
  private:
    RobotLocation no_good_rloc;   // wird fuer Dummy-Implementierung von get_slfilter_robot_location(Time&) benoetigt 
  };

}

#endif

