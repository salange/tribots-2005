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

#include "sercom.h"

#include "ValueReader.h"
#include "my_err_msg.h"
#include "sercom_std.h"


SerCom::SerCom (const char   *_conf_fname,           // Name for parameter file to read from
		const char   *_conf_chapter,         // chapter in parameterfile
		ostream* _errout,         // ostream to write error messages
		ostream* _infoout)        // ostream to write informations
{
  
  errout  = _errout;
  infoout = _infoout;

  init_params();

  read_params(_conf_fname, _conf_chapter);

  switch (com_mode)
    {
    case ::SerComI::COMMODE_LINUX:
    case ::SerComI::COMMODE_LINUX_USB:
      com_obj = new SerComStd(_conf_fname, _conf_chapter, errout, infoout);
      break;
    case ::SerComI::COMMODE_RTLINUX:
      //com_obj = new SerComRtl();
      THIS_ERROUT("RTCOM Not yet implemented!");
      break;
    default:
      THIS_ERROUT("SER_MODE not known!");
    }
  
  if (!com_obj->init())
    {
      THIS_ERROUT("Can't init a SerCom Object.");
      exit(0);
    }
}


SerCom::~SerCom()
{
  com_obj->deinit();
  if (com_obj)
    delete com_obj;
  THIS_INFOOUT("OK, bye!");

  (*errout) << std::flush;
  (*infoout) << std::flush;
}


void SerCom::init_params()
{
  com_obj  = 0;

  com_mode = com_mode_def;
}



bool  SerCom::read_params(const char   *name, const char   *chapter)
{
  ValueReader vr;
  
  if (!vr.append_from_file (name, chapter))
    {
      THIS_ERROUT("Failed to read params for serial communication from file: " << name << " chapter: " << chapter << " using defaults!.");
      return false;
    }
  if (!vr.get("SER_MODE",com_mode)) THIS_ERROUT("Cant read param: SER_MODE, using default: " << com_mode_def);

  return true;
}

int   SerCom::flush_line(bool _verbose)
{
  return com_obj->flush_line(_verbose);
}

bool  SerCom::talk                  (char    *_send, 
				     char    *_receive, 
				     int      _recv_len, 
				     int      _test,
				     timeval *_sendtime, 
				     timeval *_receivetime,
				     bool     _verbose)
{
  return com_obj->talk(_send, _receive, _recv_len, _test, _sendtime, _receivetime, _verbose);
}



bool  SerCom::readline              (char    *_receive,
				     int      _recv_len,
				     timeval *_receivetime,
				     bool     _verbose)
{
  return com_obj->readline(_receive,_recv_len,_receivetime, _verbose);
}


bool  SerCom::readline_async        (char    *_receive,
				     int      _recv_len,
				     long     timeout_us,
				     timeval *_receivetime,
				     bool     _verbose)
{
  return com_obj->readline_async (_receive, _recv_len, timeout_us, _receivetime, _verbose);
}

  
bool  SerCom::send                  (char  *_send,
				     timeval *_sendtime,
				     bool     _verbose)
{
  return com_obj->send(_send, _sendtime, _verbose);
}
