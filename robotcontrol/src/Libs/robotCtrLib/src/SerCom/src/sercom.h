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

#include <stdio.h>
#include <termios.h>
#include <sys/time.h>
#include <iostream>
#include "sercom_i.h"


class SerCom 
{
 private:
  std::ostream *errout, *infoout;
  
  SerComI *com_obj;

  // params to read and init
  int     com_mode;     static const int com_mode_def   =  ::SerComI::COMMODE_LINUX;
  
  void  init_params();
  bool  read_params           (const char   *name,                 // Name for parameter file to read from
			       const char   *chapter);              // chapter in parameterfile

 public:
 
 
  /*---CONSTRUCTORS--------------------------*/
  SerCom                     (const char   *_conf_fname,                 // Name for parameter file to read from
			      const char   *_conf_chapter,              // chapter in parameterfile
			      std::ostream* _errout=&std::cerr,        // ostream to write error messages
			      std::ostream* _infoout=&std::cout);      // ostream to write informations

			      
  /*---DESTRUCTOR----------------------------*/
  ~SerCom                    ();


  /*---PUBLIC FUNCTIONS----------------------*/
  void print_params           (std::ostream &_out);
  
  int flush_line(bool _verbose = 1);

  bool  talk                  (char    *_send, 
			       char    *_receive, 
			       int      _recv_len, 
			       int      _test = 0,
			       timeval *_sendtime=0, 
			       timeval *_receivetime=0,
			       bool     _verbose = 1);
  
  bool  readline              (char    *_receive,
			       int      _recv_len,
			       timeval *_receivetime=0,
			       bool     _verbose = 1);

  bool  readline_async        (char    *_receive,
			       int      _recv_len,
			       long     timeout_us,
			       timeval *_receivetime=0,
			       bool     _verbose = 1);

  bool  send                  (char    *_send,
			       timeval *_sendtime=0,
			       bool     _verbose = 1);
};
