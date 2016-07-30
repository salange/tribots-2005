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


#ifndef _Tribots_PlayerFactory_h_
#define _Tribots_PlayerFactory_h_

#include "PlayerType.h"
#include "../Structures/TribotsException.h"
#include "../Fundamental/ConfigReader.h"
#include <string>
#include <vector>
#include <map>


namespace Tribots {

  /** Abstrakte Klasse zum Erzeugen von Spielertypen; fuer jeden Spielertyptyp
      muss eine eigene Klasse abgeleitet werden, die die Klasse bei der PlayerFactory
      anmeldet. Als Beispiel siehe die PlayerDummy-Klasse */
  class PlayerBuilder {
  public:
    /** Destruktor */
    virtual ~PlayerBuilder () throw () {;}
    /** erzeuge eine neue Instanz des jeweiligen Spielertyps; werfe eine Exception, wenn
	Objekt nicht ordnungsgemaess erzeugt werden kann; 
	arg1: Typbezeichnung, arg2: Config-Parameter, arg3: NULL-Pointer oder weiteres Spielertyp, 
	falls Spielertype verschachtelt werden (z.B. bei AddJpystickPlayer) */ 
    virtual PlayerType* get_player (const std::string&, const ConfigReader&, PlayerType*) throw (TribotsException,std::bad_alloc) =0;
  };



  /** PlayerFactory verwaltet die verschiedenen Typen von Spielertypen;
      Jeder Spielertyp muss sich zunaechst bei der Factory anmelden */
  class PlayerFactory {
  private:
    std::map<std::string, PlayerBuilder*> list_of_plugins; ///< die Liste bekannter Spielertypen
    static PlayerFactory* the_only_factory;                ///< Zeiger auf die einzige Factory (singleton)
    PlayerFactory () throw ();                             ///< private Konstruktor, wegen singleton-Eigenschaft
    ~PlayerFactory() throw ();                             ///< Destruktor auch privat, Objekt soll nicht geloescht werden

  public:
    /** statt Konstruktor: statische Aufruffunktion */
    static PlayerFactory* get_player_factory () throw (std::bad_alloc);

    /** Anmeldefunktion, arg1: Typbezeichner, arg2: Zeiger auf Builderobjekt */
    void sign_up (const std::string, PlayerBuilder*) throw (std::bad_alloc);
    /** Erzeugungsfunktion, arg1: Typbezeichner, arg2: Config-Parameter, return: neues Objekt; 
	bei Problemen werden Ausnahmen geworfen */
    PlayerType* get_player (const std::string, const ConfigReader&) throw (TribotsException,std::bad_alloc,std::invalid_argument);
    /** schreibe in arg1 die Liste aller verfuegbaren Spielertypen */
    void player_list (std::vector<std::string>&) const throw (std::bad_alloc);
  };

}

#endif
