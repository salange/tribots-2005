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


#include "TribotsException.h"

Tribots::TribotsException::TribotsException (const char* err_type) {
  what_string = std::string ("TribotsException: ")+std::string(err_type);
}

const char* Tribots::TribotsException::what () throw () {
  return what_string.c_str();
}

Tribots::HardwareException::HardwareException (const char* err_type) : TribotsException (err_type) {;}

Tribots::BadHardwareException::BadHardwareException (const char* err_type) : TribotsException (err_type) {;}

Tribots::InvalidConfigurationException::InvalidConfigurationException (const char* err_line) : TribotsException ((std::string ("invalid configuration line \"")+std::string(err_line)+std::string("\"")).c_str()) {;}

