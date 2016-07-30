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


#ifndef _Tribots_RobotFactory_h_
#define _Tribots_RobotFactory_h_

#include "RobotType.h"
#include "../Structures/TribotsException.h"
#include "../Fundamental/ConfigReader.h"
#include <string>
#include <map>


namespace Tribots {

  /** Abstrakte Klasse zum Erzeugen von Robotertypen; fuer jeden Robotertyp
      muss eine eigene Klasse abgeleitet werden, die die Klasse bei der RobotFactory
      anmeldet. Als Beispiel siehe die RobotDummy-Klasse */
  class RobotBuilder {
  public:
    /** Destruktor */
    virtual ~RobotBuilder () throw () {;}
    /** erzeuge eine neue Instanz des jeweiligen Robotertyps; werfe eine Exception, wenn
	Objekt nicht ordnungsgemaess erzeugt werden kann; 
	arg1: Typbezeichnung, arg2: Config-Parameter, arg3: NULL-Pointer oder weiteres Spielertyp, 
	falls Robotertypen verschachtelt werden sollen */ 
    virtual RobotType* get_robot (const std::string&, const ConfigReader&, RobotType*) throw (TribotsException,std::bad_alloc) =0;
  };



  /** RobotFactory verwaltet die verschiedenen Typen von Robotertypen;
      Jeder Robotertyp muss sich zunaechst bei der Factory anmelden */
  class RobotFactory {
  private:
    std::map<std::string, RobotBuilder*> list_of_plugins; ///< die Liste bekannter Robotertypen
    static RobotFactory* the_only_factory;                ///< Zeiger auf die einzige Factory (singleton)
    RobotFactory () throw ();                             ///< private Konstruktor, wegen singleton-Eigenschaft
    ~RobotFactory() throw ();                             ///< Destruktor auch privat, Objekt soll nicht geloescht werden

  public:
    /** statt Konstruktor: statische Aufruffunktion */
    static RobotFactory* get_robot_factory () throw (std::bad_alloc);

    /** Anmeldefunktion, arg1: Typbezeichner, arg2: Zeiger auf Builderobjekt */
    void sign_up (const std::string, RobotBuilder*) throw (std::bad_alloc);
    /** Erzeugungsfunktion, arg1: Typbezeichner, arg2: Config-Parameter, return: neues Objekt; 
	bei Problemen werden Ausnahmen geworfen */
    RobotType* get_robot (const std::string, const ConfigReader&) throw (TribotsException,std::bad_alloc,std::invalid_argument);
  };

}

#endif
