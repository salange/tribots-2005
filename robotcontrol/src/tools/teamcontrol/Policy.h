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


#ifndef TribotsTools_Policy_h
#define TribotsTools_Policy_h

#include "../../Fundamental/ConfigReader.h"
#include "../../Fundamental/Table.h"
#include "../../Structures/TribotsException.h"
#include "RemoteTeamState.h"
#include <string>
#include <stdexcept>

namespace TribotsTools {

  /** Rollenwechselstrategie, abstrakt */  
  class Policy {
  protected:
    std::string name;
  public:
    /** Konstruktor. Arg1=Strategiename */
    Policy (const char* nm) throw (std::bad_alloc, Tribots::TribotsException) : name(nm) {;}
    Policy (const Policy& src) throw (std::bad_alloc) : name (src.name) {;}
    virtual ~Policy () throw () {;}
    virtual Policy* clone () const throw (std::bad_alloc) =0;
    /** Strategie aktualisieren, wenn noetig */
    virtual void update (RemoteTeamState&) throw () {;}
    /** Name der Strategie abfragen */
    virtual const std::string& get_name () const throw () { return name; }
    /** Soll-Rollenverteilung: get_roles()(i,j) ist die Rolle des j+1-ten Roboter, wenn i+1 RCPlayer mitspielen */
    virtual const Tribots::Table<std::string> get_roles () const throw () =0;
  };

  /** Statische Rollenwechselstrategie, jeder Spieler bekommt fest eine Rolle */
  class StaticPolicy : public Policy {
  protected:
    Tribots::Table<std::string> roles;
  public:
    StaticPolicy (const Tribots::ConfigReader&, const char* nm) throw (std::bad_alloc, Tribots::TribotsException);
    StaticPolicy (const StaticPolicy&) throw (std::bad_alloc);
    Policy* clone () const throw (std::bad_alloc);
    const Tribots::Table<std::string> get_roles () const throw ();
  };

  /** Dynamische Strategie: Strategie wechselt bei Ballueberquerung der Mittellinie */
  class DynamicDefendOffendPolicy : public Policy {
  public:
    DynamicDefendOffendPolicy (const Tribots::ConfigReader&, const char*) throw (std::bad_alloc, Tribots::TribotsException);
    DynamicDefendOffendPolicy (const DynamicDefendOffendPolicy&) throw (std::bad_alloc);
    Policy* clone () const throw (std::bad_alloc);
    ~DynamicDefendOffendPolicy () throw ();
    void update (RemoteTeamState&) throw ();
    const Tribots::Table<std::string> get_roles () const throw ();
  protected:
    double hyst_y1, hyst_y2;
    Policy* defend_policy;
    Policy* offend_policy;
    bool was_defend;
  };
 
}


#endif
