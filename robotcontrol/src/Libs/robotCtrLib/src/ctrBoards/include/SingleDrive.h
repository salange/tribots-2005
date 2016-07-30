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

#ifndef _SINGLEDRIVE_H_
#define _SINGLEDRIVE_H_

#include <iostream>

class SingleDrive
{

 protected:
  std::ostream *errout, *infoout;
  
  static const int num_of_enc_imp_default   = 500;
  static const int max_motor_vel_default    = 6000;
  static const int delta_dist_default       = 22;
  static const int C_nom_default            = 130;
  static const int C_max_default            = 590;
  static const int kp_default               = 10;
  static const int ki_default               = 20;
  static const int kd_default               = 0;
  static const float gearparam_default      = 1;

  int num_of_enc_imp;    // number of Impulses on encoder disc
  int max_motor_vel;     // maximal velocity of motor in turns/min
  int delta_dist;        // a distance for 1 impulse
  
  int C_nom;             // nominal current in mA
  int C_max;             // maximal current in mA

  int kp;                // params for the pid velocity controller
  int ki;
  int kd;

  float gearparam;       // gear factor

 public:
  SingleDrive(const char* _conf_fname, const char* _conf_chapter, 
	      std::ostream *_errout = &std::cerr, std::ostream *_infoout = &std::cout);
  ~SingleDrive();

  void get_pid_params(int &_kp, int &_ki, int &_kd){_kp=kp; _ki=ki;_kd=kd;};
  void set_pid_params(int _kp, int _ki, int _kd){kp=_kp; ki=_ki;kd=_kd;};

  void get_current_params(int &_c_nom, int &_c_max){_c_nom=C_nom; _c_max=C_max;};
  void set_current_params(int _c_nom, int _c_max){C_nom=_c_nom; C_max=_c_max;};

  void get_motor_params(int &_num_imp, int &_max_vel, int &_delta_dist){ 
    _num_imp=num_of_enc_imp; _max_vel=max_motor_vel; _delta_dist=delta_dist;};
  void set_motor_params(int _num_imp, int _max_vel, int _delta_dist){ 
    num_of_enc_imp = _num_imp; 
    max_motor_vel  = _max_vel; 
    delta_dist     = _delta_dist;};

  void get_gear_param(float &_gp){_gp=gearparam;};
  void set_gear_param(float _gp){gearparam=_gp;};

  void print(std::ostream &_out);

};



#endif
