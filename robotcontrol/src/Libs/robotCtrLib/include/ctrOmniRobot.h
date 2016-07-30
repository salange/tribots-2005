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

#ifndef _CTROMNIROBOT_H_
#define _CTROMNIROBOT_H_

#include "OmniRobotKin.h"
#include "Tmc200.h"
#include "OmniRobotData.h"


std::ostream& operator << (std::ostream& os, ctrOmniRobotData_t& data);

class ctrOmniRobot 
{
 protected:
  std::ostream *errout, *infoout;

  OmniRobotKin * kin;
  Tmc200       * board;

  bool         inited;
  void read_params(const char * _fname, const char * _chapter);
  void cmptOmnirobotData(ctrOmniRobotData_t & target, tmc_real_data_t tmc_src);

  float        max_wheel_vel;      static const float max_wheel_vel_def   = 35.0; //rad/s
  float        max_wheel_acc;      static const float max_wheel_acc_def   = 50.0; //rad/s*s
  float        max_wheel_deacc;    static const float max_wheel_deacc_def = 60.0; //rad/s*s

 public:
  ctrOmniRobot(const char* _conf_fname, const char * _conf_chapter,
	    std::ostream *_errout = &std::cerr, 
	    std::ostream *_infoout = &std::cout);
  ~ctrOmniRobot();

  bool init();

  bool setVel_wheels_nonblocking(float _v1, float _v2, float _v3, timeval *tstmp = NULL);
  bool setVel_wheels_blocking(float _v1, float _v2, float _v3, ctrOmniRobotData_t * data, timeval *tstmp = NULL);
  
  bool setVel_vec_nonblocking(float _xm, float _ym, float _phim, timeval *tstmp = NULL, 
			      float *v1 =0, float* v2=0, float* v3=0);
  bool setVel_vec_blocking(float _xm, float _ym, float _phim, ctrOmniRobotData_t * data, timeval *tstmp = NULL, 
			   float *v1 =0, float* v2=0, float* v3=0);

  bool setKickPort_blocking(bool high, timeval *tstmp = NULL);
  bool setKickPort_nonblocking(bool high, timeval *tstmp = NULL);

  int  getData_nonblocking(ctrOmniRobotData_t * datalist, int maxnum);

  // Funktion zur Berechnung eines durchführbaren neuen Geschwindigkeitsvektors nach Zeit _dt_ms ms
  // basierend auf der aktuellen Geschwindigkeit
  bool get_realizable_vel_vec(float _act_xm, float _act_ym, float _act_phim,  
			      float _new_xm, float _new_ym, float _new_phim, float _dt_ms,
			      float & _realizable_xm, float & _realizable_ym, float & _realizable_phim);
  
  // get a pointer to the control Board ... for Debugging purposes only
  // do not use this if you don't really know what you are doing
  Tmc200* getTmc_ptr(void){return board;};
};

#endif
