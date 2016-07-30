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


#include "DriveVector.h"

Tribots::DriveVector::DriveVector () throw () {
  vtrans.x=0;
  vtrans.y=0;
  vrot=0;
  mode = ROBOTVELOCITY;
  for (int i=0; i<3; i++) vaux[i]=0; 
  kick=false;
}

Tribots::DriveVector::DriveVector (Tribots::Vec vtrans1, double vrot1, bool kick1) throw() 
  : vtrans(vtrans1), vrot(vrot1), kick(kick1), mode(ROBOTVELOCITY) {;}

Tribots::DriveVector::DriveVector (double v1, double v2, double v3, bool _kick, DriveVectorMode _mode)
{
  kick = _kick;
  mode = _mode;
  switch (mode) {
  case ROBOTVELOCITY:
    vtrans.x = v1;
    vtrans.y = v2;
    vrot     = v3;
    for (int i=0; i<3; i++) vaux[i] = 0;
    break;
  case WHEELVELOCITY:
  case MOTORVOLTAGE:
    vaux[0]=v1;
    vaux[1]=v2;
    vaux[2]=v3;
    vtrans.x = 0;
    vtrans.y = 0;
    vrot     = 0;
    break;
  }
}

Tribots::DriveVector::DriveVector (const Tribots::DriveVector& src) throw () : vtrans(src.vtrans), vrot(src.vrot), kick(src.kick), mode(src.mode) {for (int i=0; i<3; i++) vaux[i]=src.vaux[i];}

const Tribots::DriveVector& Tribots::DriveVector::operator= (const Tribots::DriveVector& src) throw () {
  vtrans=src.vtrans;
  vrot = src.vrot;
  kick = src.kick;
  mode = src.mode;
  for (int i=0; i<3; i++) vaux[i]=src.vaux[i];
  return (*this);
}


