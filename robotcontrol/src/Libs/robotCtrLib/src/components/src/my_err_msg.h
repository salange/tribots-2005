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

#ifndef _MY_ERR_MSG_H_
#define _MY_ERR_MSG_H_

#include <iostream>

#ifndef THIS_ERROUT
#define THIS_ERROUT(__msg__)  *this->errout<<  "##ERR  | " << __FILE__ << " | " << __PRETTY_FUNCTION__ << " | " << __msg__ << "\n" << std::flush
#endif

#ifndef THIS_INFOOUT
#define THIS_INFOOUT(__msg__) *this->infoout<< "##INFO | " << __FILE__ << " | " << __PRETTY_FUNCTION__ << " | " << __msg__ << "\n" << std::flush
#endif

#endif
