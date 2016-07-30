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

#ifndef _SERCOM_I_H_
#define _SERCOM_I_H_

#include <sys/time.h>
#include <iostream>


class SerComI
{
 protected:
  std::ostream *errout, *infoout;

  int    ser_mode;            static const int ser_mode_def  =  0;
  int    ser_port;            static const int ser_port_def  =  0;
  int    ser_speeds[10];      
  int    ser_speed;           static const int ser_speed_def =  1;
  int    ser_stbits;          static const int ser_stbits_def =  1;
  int    inter_char_timeout;  static const int inter_char_timeout_def = 100; // timout in ms
  int    comm_timeout;        static const int comm_timeout_def       = 400; // timout in ms
  int    num_junk_after_nl;   static const int num_junk_after_nl_def  = 0;
  
  void init_params();
  bool read_params(const char *_fname, const char * _chapter);
  void print_params(std::ostream & _out);

 public:
  static const int COMMODE_LINUX        = 0;
  static const int COMMODE_LINUX_USB    = 1;
  static const int COMMODE_RTLINUX      = 2;

  static const int T_NOTHING            = 0;
  static const int T_SECOND             = 1;  
  static const int T_OK                 = 2;             
  static const int T_FIRST_SMALL        = 3; 

  SerComI(const char * _conf_fname, const char * _conf_chapter, 
	  std::ostream* _errout=&std::cerr, std::ostream* _infoout=&std::cout);

  virtual ~SerComI();

  virtual bool  init();

  virtual bool  deinit();

  virtual int flush_line(bool _verbose = 1);

  virtual bool  talk                  (char    *_send, 
				       char    *_receive, 
				       int      _recv_len, 
				       int      _test = 0,
				       timeval *_sendtime=0, 
				       timeval *_receivetime=0,
				       bool     _verbose = 1);
  
  
  virtual bool  readline              (char    *_receive,
				       int      _recv_len,
				       timeval *_receivetime=0,
				       bool     _verbose = 1);

  virtual bool  readline_async        (char    *_receive,
				       int      _recv_len,
				       long     timeout_us,
				       timeval *_receivetime=0,
				       bool     _verbose = 1);
  

  virtual bool  send                  (char    *_send,
				       timeval *_sendtime=0,
				       bool     _verbose = 1);  
};

#endif
