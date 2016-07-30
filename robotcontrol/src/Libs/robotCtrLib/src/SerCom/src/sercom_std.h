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

#ifndef _SERCOM_STD_H_
#define _SERCOM_STD_H_

#include "sercom_i.h"
#include <termios.h>

#define MAX_ASYNC_BUFFER 200

//#define DEBUG
//#define DEBUG_LEVEL   3

class SerComStd : public SerComI
{
 protected:  
  fd_set readfs;
  int    maxfd;     /* maximum file desciptor used */
  struct timeval ComTimeout;

  char string_buffer_async[MAX_ASYNC_BUFFER];
  bool string_buffer_async_empty;

  int fd;
  struct termios oldtio,newtio;


 public:
  SerComStd(const char * _conf_fname, const char * _conf_chapter, 
	    std::ostream* _errout=&std::cerr, std::ostream* _infoout=&std::cout) : SerComI( _conf_fname, _conf_chapter,_errout, _infoout){;};


  bool  init();
  bool  deinit();

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

#endif
