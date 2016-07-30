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


#ifndef TribotsTools_Coach_h
#define TribotsTools_Coach_h

#include "RemoteTeamState.h"
#include "Policy.h"
#include "../../Fundamental/ConfigReader.h"
#include "../../Fundamental/Table.h"
#include "../../Fundamental/Time.h"
#include "../../Structures/TribotsException.h"
#include <vector>
#include <deque>

namespace TribotsTools {

  class Coach {
  public:
    /** Konstruktor; Arg1: Configfile fuer Taktikvorgaben; Arg2: Liste der Roboter-IDs */
    Coach (const Tribots::ConfigReader&, const std::vector<std::string>&) throw (Tribots::TribotsException);
    ~Coach () throw ();

    /** Treffen der taktischen Entscheidungen und kommunikation mit den Spielern;
	Arg1 ist der zuletzt empfangene Gesamtzustand des Teams */
    void update (RemoteTeamState&) throw ();

    /** ein-/ausschalten, dass die Ballpositionen kommuniziert werden */
    void set_comm_ball_mode (bool) throw ();
    /** Kommunizieren von Ballpositionen eingeschaltet? */
    bool get_comm_ball_mode () const throw ();

    /** Owns-Ball-Modus ein-/ausschalten */
    void set_owns_ball_mode (bool) throw ();
    /** Owns-Ball-Modus eingeschaltet? */
    bool get_owns_ball_mode () const throw ();

    /** Dynamische Rollenvergabe auswaehlen (Leerstring=ausschalten), zurueckgegeben wird true, wenn ein entsprechender Modus gefunden wurde */
    bool dynamic_role_change_mode (const char*) throw ();
    /** Dynamische Rollenvergabe, Modus */
    const std::string& dynamic_role_change_mode () const throw ();
    /** Einem Spieler manuell eine Rolle zuweisen; Arg1: Spielerbezeichner, Arg2: Rollenbezeichner */
    void change_player_role (const char*, const char*) throw ();
    /** Eine Liste aller verfuegbaren Rollen */
    std::vector<std::string> get_list_of_roles () const throw ();
    /** Eine Liste aller verfuegbaren Rollenwechsel-Strategien */
    std::vector<std::string> get_list_of_policies () const throw ();
    /** die Rolle eines Spielers (arg1) anfragen */
    std::string get_players_role (const std::string&) const throw ();
    /** einen String als textuelle Ausgabe der derzeitigen Rollenverteilung */
    const std::string& get_role_description () const throw ();

    /** KickOff-Variante waehlen; Rueckgabe: gewuenschte Variante existiert und konnte gesetzt werden? */
    bool kick_off_own_procedure (const std::string&) throw ();
    /** KickOff-Variante waehlen; Rueckgabe: gewuenschte Variante existiert und konnte gesetzt werden? */
    bool kick_off_opponent_procedure (const std::string&) throw ();
    /** KickOff-Variante */
    const std::string& kick_off_own_procedure () const throw ();
    /** KickOff-Variante */
    const std::string& kick_off_opponent_procedure () const throw ();
    /** Eine Liste aller verfuegbaren Kick-Off-Varianten */
    std::vector<std::string> get_list_of_kick_off_procedures () const throw ();

   /** ein-/ausschalten, dass alle nicht vom Trainer verarbeiteten Nachrichten an alle weitergesendet werden;
	deaktivierte Roboter erhalten alle Nachrichten, koennen aber keine abgeben */
    void set_broadcast_mode (bool) throw();
    /** Broadcast-Modus eingeschaltet? */
    bool get_broadcast_mode () const throw ();

  private:
    struct HPos {
      std::string role;
      Tribots::Vec pos;
      Tribots::Angle heading;
    };
    struct KickOffPolicy {
      std::string name;
      std::vector<std::deque<HPos> > pos;
      KickOffPolicy () throw (std::bad_alloc);
      KickOffPolicy(const KickOffPolicy&) throw (std::bad_alloc);
    };

    // Rollenwechsel-Strategien fuer den RCPlayer
    std::vector<Policy*> policies;  // die Beschreibung der Ersetzungsstrategien
    std::vector<std::string> robot_ids;   // die Roboter-IDs
    std::vector<std::string> desired_roles;  // die gewuenschten Rollen der einzelnen Roboter
    std::vector<std::string> actual_roles;  // die tatsaechlichen Rollen der einzelnen Roboter
    std::vector<bool> role_known;  // ist die tatsaechliche Rolle der einzelnen Roboter bekannt?
    std::vector<bool> is_active;  // ist ein Roboter im Spiel?
    std::vector<Tribots::Time> activation_time;  // wann ist der Spieler aktiviert worden?
    std::vector<KickOffPolicy> ko_policies;  // Beschreibung der Anstossvarianten

    int policies_index (const std::string&) throw();  // durchsucht die Policy-Liste nach einem string und liefert den Index bei Erfolg bzw. -1 bei Misserfolg
    int ko_policies_index (const std::string&) throw (); // dito fuer Kick-Off-Policies

    bool comm_ball_mode;
    bool broadcast_mode;
    bool owns_ball_mode;
    std::string rc_player_dynamic_role_change_mode;
    std::string kick_off_own_policy;
    std::string kick_off_opponent_policy;

    std::vector<std::string> ignore_broadcast_prefix;  // Liste mit Prefixen, die im Broadcast-Modus nicht weiterkommuniziert werden
    std::string role_description;
  };

}

#endif
