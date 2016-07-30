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


#include "CycleInfo.h"

TribotsTools::CycleInfo::CycleInfo () {;}

TribotsTools::CycleInfo::CycleInfo (const TribotsTools::CycleInfo& ci) : rloc_vis(0), bloc_vis(0), rloc_exec(0), bloc_exec(0), ppos(0) {
  cycle_num = ci.cycle_num;
  time_msec = ci.time_msec;
  rloc_vis = ci.rloc_vis;
  bloc_vis = ci.bloc_vis;
  rloc_exec = ci.rloc_exec;
  bloc_exec = ci.bloc_exec;
  oloc = ci.oloc;
  vloc = ci.vloc;
  gs = ci.gs;
  logmsg = ci.logmsg;
  paintmsg = ci.paintmsg;
  ppos = ci.ppos;
}

const TribotsTools::CycleInfo& TribotsTools::CycleInfo::operator= (const TribotsTools::CycleInfo& ci) {
  cycle_num = ci.cycle_num;
  time_msec = ci.time_msec;
  rloc_vis = ci.rloc_vis;
  bloc_vis = ci.bloc_vis;
  rloc_exec = ci.rloc_exec;
  bloc_exec = ci.bloc_exec;
  oloc = ci.oloc;
  vloc = ci.vloc;
  gs = ci.gs;
  logmsg = ci.logmsg;
  paintmsg = ci.paintmsg;
  ppos = ci.ppos;
  return (*this);
}

  
