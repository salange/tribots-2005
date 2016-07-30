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


#ifndef Tribots_stringconvert_h
#define Tribots_stringconvert_h

#include <string>
#include <deque>
#include <stdexcept>

namespace Tribots {

  /** Aufspalten eines Strings an den whitspace-Stellen.
      Arg1: Rueckgabewert, Liste von Woertern
      Arg2: Argument, aufzuspaltender String */
  void split_string (std::deque<std::string>&, const std::string&) throw (std::bad_alloc);

  /** Testen, ob Arg1 Praefix von Arg2 ist */
  bool prefix (const std::string&, const std::string) throw ();

  /** Umwandeln eines string in einen double
      Arg1: Rueckgabewert bei Erfolg
      Arg2: Argument, umzuwandelnder String
      Return: true, wenn String tatsaechlich als double interpretiert werden konnte, sonst false */
  bool string2double (double&, const std::string&) throw ();

  /** Umwandeln eines string in einen float
      Arg1: Rueckgabewert bei Erfolg
      Arg2: Argument, umzuwandelnder String
      Return: true, wenn String tatsaechlich als float interpretiert werden konnte, sonst false */
  bool string2float (float&, const std::string&) throw ();

  /** Umwandeln eines string in einen int
      Arg1: Rueckgabewert bei Erfolg
      Arg2: Argument, umzuwandelnder String
      Return: true, wenn String tatsaechlich als int interpretiert werden konnte, sonst false */
  bool string2int (int&, const std::string&) throw ();

  /** Umwandeln eines string in einen unsigned int
      Arg1: Rueckgabewert bei Erfolg
      Arg2: Argument, umzuwandelnder String
      Return: true, wenn String tatsaechlich als unsigned int interpretiert werden konnte, sonst false */
  bool string2uint (unsigned int&, const std::string&) throw ();

  /** Umwandeln eines string in einen long int
      Arg1: Rueckgabewert bei Erfolg
      Arg2: Argument, umzuwandelnder String
      Return: true, wenn String tatsaechlich als long int interpretiert werden konnte, sonst false */
  bool string2lint (long int&, const std::string&) throw ();

  /** Umwandeln eines string in einen unsigned long int
      Arg1: Rueckgabewert bei Erfolg
      Arg2: Argument, umzuwandelnder String
      Return: true, wenn String tatsaechlich als unsigned long int interpretiert werden konnte, sonst false */
  bool string2ulint (unsigned long int&, const std::string&) throw ();

  /** Umwandeln eines string in einen bool
      Arg1: Rueckgabewert bei Erfolg
      Arg2: Argument, umzuwandelnder String
      Return: true, wenn String tatsaechlich als bool interpretiert werden konnte, sonst false */
  bool string2bool (bool&, const std::string&) throw ();

}

#endif
