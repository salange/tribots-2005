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

#ifndef __SIMPLE_VEC_CLUSTER_H__
#define __SIMPLE_VEC_CLUSTER_H__

#include "../Fundamental/Vec.h"
#include <vector>

namespace Tribots {

class VecCluster {
 public:
  std::vector<Vec> samples;
  Vec    com;
  double comWidth;
  Vec    support;
  double minRadAng, maxRadAng;

  double width;

  void update();

  VecCluster();

  bool add(const Tribots::Vec& v);
  double dist(const Tribots::Vec& v) const;
  double dist(const Tribots::VecCluster& vc);
  VecCluster glue(const Tribots::VecCluster &other);
  
  void clear() {samples.clear(); support.x=0; support.y=0; width=0;};
};


class SimpleVecCluster {
 public:
  double thresh;
  std::vector<VecCluster> cluster;

  SimpleVecCluster(double _thresh);
  void add(const Tribots::Vec& v);
  void prune();
  
};
}

#endif
