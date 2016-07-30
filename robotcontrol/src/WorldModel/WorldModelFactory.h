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


#ifndef _Tribots_WorldModelFactory_h_
#define _Tribots_WorldModelFactory_h_

#include "WorldModelType.h"
#include "../Fundamental/ConfigReader.h"
#include <string>
#include <map>


namespace Tribots {

  /** Abstrakte Klasse zum Erzeugen von Weltmodellen; fuer jeden Weltmodelltyp
      muss eine eigene Klasse abgeleitet werden, die die Klasse bei der WorldModelFactory
      anmeldet. Als Beispiel siehe die WorldModelDummy-Klasse */
  class WorldModelBuilder {
  public:
    /** Destruktor */
    virtual ~WorldModelBuilder () throw () {;}
    /** erzeuge eine neue Instanz des jeweiligen Weltmodells; werfe eine Exception, wenn
	Objekt nicht ordnungsgemaess erzeugt werden kann; 
	arg1: Typbezeichnung, arg2: Config-Parameter, arg3: NULL-Pointer oder weiteres Weltmodell, 
	falls Weltmodelle verschachtelt werden (z.B. bei AddWriteWorldModel) */ 
    virtual WorldModelType* get_world_model (const std::string&, const ConfigReader&, WorldModelType*) throw (TribotsException,std::bad_alloc) =0;
  };



  /** WorldModelFactory verwaltet die verschiedenen Typen von Weltmodellen;
      Jedes Weltmodell muss sich zunaechst bei der Factory anmelden */
  class WorldModelFactory {
  private:
    std::map<std::string, WorldModelBuilder*> list_of_plugins; ///< die Liste bekannter Weltmodelltypen
    static WorldModelFactory* the_only_factory;                ///< Zeiger auf die einzige Factory (singleton)
    WorldModelFactory () throw ();                             ///< private Konstruktor, wegen singleton-Eigenschaft
    ~WorldModelFactory() throw ();                             ///< Destruktor auch privat, Objekt soll nicht geloescht werden

  public:
    /** statt Konstruktor: statische Aufruffunktion */
    static WorldModelFactory* get_world_model_factory () throw (std::bad_alloc);

    /** Anmeldefunktion, arg1: Typbezeichner, arg2: Zeiger auf Builderobjekt */
    void sign_up (const std::string, WorldModelBuilder*) throw (std::bad_alloc);
    /** Erzeugungsfunktion, arg1: Typbezeichner, arg2: Config-Parameter, return: neues Objekt; 
	bei Problemen werden Ausnahmen geworfen */
    WorldModelType* get_world_model (const std::string, const ConfigReader&) throw (TribotsException,std::bad_alloc,std::invalid_argument);
  };

}

#endif
