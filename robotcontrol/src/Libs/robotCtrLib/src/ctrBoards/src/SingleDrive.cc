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

#include "SingleDrive.h"
#include "ValueReader.h"
#include "my_err_msg.h"

SingleDrive::SingleDrive(const char* _conf_fname, const char* _conf_chapter,  std::ostream *_errout, std::ostream *_infoout)
{
  errout  = _errout;
  infoout = _infoout;
  
  ValueReader vr;

  if (! vr.append_from_file (_conf_fname, _conf_chapter ) )
    {
      THIS_ERROUT("Problem with reading " << _conf_fname << " in Chapter " << _conf_chapter);
    }
  
  if (vr.get ("NUM_OF_ENC_IMP", num_of_enc_imp) < 0 ) 
    {
      THIS_ERROUT("Can't read param: NUM_OF_ENC_IMP, using default: "<< num_of_enc_imp_default);
      num_of_enc_imp=num_of_enc_imp_default;
    }
  if (vr.get ("MAX_MOTOR_VEL", max_motor_vel) < 0 ) 
    {
      THIS_ERROUT("Can't read param: MAX_MOTOR_VEL, using default: "<< max_motor_vel_default);
      max_motor_vel=max_motor_vel_default;
    }
  if (vr.get ("DELTA_DIST", delta_dist) < 0 ) 
    {
      THIS_ERROUT("Can't read param: DELTA_DIST, using default: "<< delta_dist_default);
      delta_dist=delta_dist_default;
    }
  if (vr.get ("CMAX", C_max) < 0 ) 
    {
      THIS_ERROUT("Can't read param: CMAX, using default: "<< C_max_default);
      C_max=C_max_default;
    }
  if (vr.get ("CNOM", C_nom) < 0 ) 
    {
      THIS_ERROUT("Can't read param: CNOM, using default: "<< C_nom_default);
      C_nom=C_nom_default;
    }
  if (vr.get ("KP", kp) < 0 ) 
    {
      THIS_ERROUT("Can't read param: KP, using default: "<< kp_default);
      kp=kp_default;
    }
  if (vr.get ("KI", ki) < 0 ) 
    {
      THIS_ERROUT("Can't read param: KI, using default: "<< ki_default);
      ki=ki_default;
    }
  if (vr.get ("KD", kd) < 0 ) 
    {
      THIS_ERROUT("Can't read param: KD, using default: "<< kd_default);
      kd=kd_default;
    }
  if (vr.get ("GEARFACTOR", gearparam) < 0 ) 
    {
      THIS_ERROUT("Can't read param: GEARFACTOR, using default: "<< gearparam_default);
      gearparam=gearparam_default;
    }
}


SingleDrive::~SingleDrive()
{
  (*errout) << std::flush;
  (*infoout) << std::flush;
}

void SingleDrive::print(std::ostream &_out)
{
  _out << "Number of encoder impulses: " << num_of_enc_imp << "\n"
       << "Maximal motor velocity [turns/min]: " << max_motor_vel << "\n"
       << "Distance for one impulse [um/imp]: " << delta_dist << "\n"
       << "Maximal current [mA]: " << C_max << "\n"
       << "Nominal current [mA]: " << C_nom << "\n"
       << "PID params: " << kp << "  " << ki << "  " << kd << " \n"
       << "Gear factor: " << gearparam << "\n";
}
