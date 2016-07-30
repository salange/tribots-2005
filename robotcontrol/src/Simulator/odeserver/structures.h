/*
 * Copyright (c) 1999 - 2001, Artur Merke <amerke@ira.uka.de> 
 *
 * This file is part of FrameView2d.
 *
 * FrameView2d is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * FrameView2d is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FrameView2d; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _structuresH_
#define _structuresH_

#include "v2d.h"
#include "iostream"


struct Robot{
double steer[3];
double odo[3];
bool kick;

Robot(){
kick=false;
for(int i=0;i<3;i++){
steer[i]=0;
odo[i]=0;
}
}

};


#endif
