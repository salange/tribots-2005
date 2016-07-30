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


#include "DistanceCalibration.h"
#include "../../Structures/Journal.h"
#include <fstream>
#include <cstdio>
#include <cmath>

using namespace std;
using namespace Tribots;
using namespace TribotsTools;

DistanceCalibration::DistanceCalibration (Tribots::Vec p) : image_center (p) {;}

DistanceCalibration::~DistanceCalibration () {;}

double DistanceCalibration::search_min_transition_index (const std::deque<MarkerInfo>& m, double dmin) {
  double min=1e300;
  for (unsigned int i=0; i<m.size(); i++)
    if (m[i].image_distance>dmin && m[i].image_distance<min) {
      min = m[i].image_distance;
    }
  return min;
}

void DistanceCalibration::search_markers (const Tribots::ScanResultList& scan_result, Tribots::Angle dir) {
  const Angle viergrad = Angle::deg_angle (4);

  // Ubergaenge blau-weiss und weiss-blau fuer 0.5, 1, 2, 3, und 4m Marker
  deque<MarkerInfo> blue_white;
  deque<MarkerInfo> white_blue;
  bool dir_okay=false;
  vector<Transition>::const_iterator it = scan_result.results[COLOR_BLUE]->transitions.begin();
  vector<Transition>::const_iterator itend = scan_result.results[COLOR_BLUE]->transitions.end();
  while (it<itend) {
    Angle ta = (it->toPos-image_center).angle();
    if (ta.in_between (dir-viergrad, dir+viergrad)) {
      if (it->type==Transition::START && it->twoStep==COLOR_LINE) {
	MarkerInfo mi;
	mi.angle = ta;
	mi.true_distance = 0;
	mi.image_distance = (it->toPos-image_center).length();
	white_blue.push_back (mi);
      } else if (it->type==Transition::END && it->twoStep==COLOR_LINE) {
	MarkerInfo mi;
	mi.angle = ta;
	mi.true_distance = 0;
	mi.image_distance = (it->toPos-image_center).length();
	blue_white.push_back (mi);
      }
      // von nahe nach weit durchgehen
      double imagedist4000, imagedist3000, imagedist2000, imagedist1000, imagedist500;
      double imagedist3700, imagedist2700, imagedist1700, imagedist700;
      imagedist500 = search_min_transition_index (blue_white, 20);
      imagedist700 = search_min_transition_index (white_blue, imagedist500);
      imagedist1000 = search_min_transition_index (blue_white, imagedist700);
      imagedist1700 = search_min_transition_index (white_blue, imagedist1000);
      imagedist2000 = search_min_transition_index (blue_white, imagedist1700);
      imagedist2700 = search_min_transition_index (white_blue, imagedist2000);
      imagedist3000 = search_min_transition_index (blue_white, imagedist2700);
      imagedist3700 = search_min_transition_index (white_blue, imagedist3000);
      imagedist4000 = search_min_transition_index (blue_white, imagedist3700);

      // blau-weiss-Uebergange uebernehmen
      if (imagedist4000<1e100) { // alle relevanten Uebergaenge beobachtet
	dir_okay=true;
	for (unsigned int i=0; i<blue_white.size(); i++) {
	  if (blue_white[i].image_distance<imagedist700) {
	    blue_white[i].true_distance=1;
	    markers.push_back (blue_white[i]);
	  } else if (blue_white[i].image_distance>imagedist700 && blue_white[i].image_distance<imagedist1700) {
	    blue_white[i].true_distance=2;
	    markers.push_back (blue_white[i]);
	  } else if (blue_white[i].image_distance>imagedist1700 && blue_white[i].image_distance<imagedist2700) {
	    blue_white[i].true_distance=3;
	    markers.push_back (blue_white[i]);
	  } else if (blue_white[i].image_distance>imagedist2700 && blue_white[i].image_distance<imagedist3700) {
	    blue_white[i].true_distance=4;
	    markers.push_back (blue_white[i]);
	  } else if (blue_white[i].image_distance>imagedist3700 && blue_white[i].image_distance<imagedist4000+1) {
	    blue_white[i].true_distance=5;
	    markers.push_back (blue_white[i]);
	  }
	}
      }
    }
    it++;
  }

  if (dir_okay) {
    // Uebergaenge weiss-orange und orange-weiss fuer 8 und 16 m Marker
    it = scan_result.results[COLOR_BALL]->transitions.begin();
    itend = scan_result.results[COLOR_BALL]->transitions.end();
    while (it<itend) {
      Angle ta = (it->toPos-image_center).angle();
      if (ta.in_between (dir-viergrad, dir+viergrad)) {
	if (it->type==Transition::START && it->twoStep==COLOR_LINE) {
	  MarkerInfo mi;
	  mi.angle = ta;
	  mi.true_distance = 6;
	  mi.image_distance = (it->toPos-image_center).length();
	  markers.push_back (mi);
	} else if (it->type==Transition::END) {
	  MarkerInfo mi;
	  mi.angle = ta;
	  mi.true_distance = 7;
	  mi.image_distance = (it->toPos-image_center).length();
	  markers.push_back (mi);
	}
      }
      it++;
    }
  }

}



void DistanceCalibration::writeMarkerFile (const std::string& filename) {
  double true_distances [] = { 0, 500, 1000, 2000, 3000, 4000, 8000, 16000 };
  const unsigned int num_markers = 8;
  
  // die Datei schreiben
  string filenamebak = filename+".bak";
  if (rename (filename.c_str(), filenamebak.c_str())!=0)
    JERROR ("Kann Markerdatei nicht zu .bak-Datei umbenennen");
  ofstream dest (filename.c_str());
  if (!dest)
    JERROR ("Kann Markerdatei nicht schreiben!");
  
  dest << 640 << ' ' << 480 << ' ' << static_cast<int>(image_center.x) << ' ' <<static_cast<int>(image_center.y) << '\n';
  dest << 3 << ' ' << num_markers+1 << '\n';
  for (unsigned int i=0; i<num_markers; i++)
    dest << true_distances[i] << ' ';
  dest << "1000000" << '\n';
  
  vector<double> values (num_markers);
  vector<double> values2 (num_markers);
  vector<unsigned int> value_count (num_markers);
  vector<unsigned int> iter_wo_assignments (num_markers);
  vector<double> prototypes (num_markers);
  prototypes[0] = 0;
  prototypes[1] = 40;
  prototypes[2] = 45;
  prototypes[3] = 50;
  prototypes[4] = 55;
  prototypes[5] = 60;
  prototypes[6] = 180;
  prototypes[7] = 200;
  for (unsigned int i=0; i<num_markers; i++)
    iter_wo_assignments[i]=0;
  for (int angleiter=-45; angleiter<45; angleiter++) {  // Iterationen -45 ... -1 zum Einschwingen
    Angle cangle = Angle::deg_angle (angleiter*8);
    Angle leftangle = Angle::deg_angle (angleiter*8+10);
    Angle rightangle = Angle::deg_angle (angleiter*8-10);
    for (unsigned int i=0; i<num_markers; i++) {
      values[i]=6*prototypes[i];   // Regularisierung
      value_count[i]=6;
    }
    
    unsigned int trial=0;
    bool clusters_okay=false;
    while (!clusters_okay && trial<3) {
      trial++;
      for (unsigned int kmeansiter=0; kmeansiter<10; kmeansiter++) {
	for (unsigned int i=1; i<=5; i++) {
	  values[i]=6*prototypes[i];
	  values2[i]=6*prototypes[i]*prototypes[i];
	  value_count[i]=6;
	}
	// Zuordnungen berechnen; den bisherigen Zuordungen in Klasse 1/500, 2/1000, 3/2000, 4/3000, 5/4000 
	// nicht trauen, nur 6/8000 und 7/16000 trauen
	deque<MarkerInfo>::const_iterator it = markers.begin();
	deque<MarkerInfo>::const_iterator itend = markers.end();
	while (it<itend) {
	  if (it->true_distance<=5 && it->angle.in_between (rightangle, leftangle)) {
	    unsigned int bestprototype=0;
	    double bestdist=1e300;
	    for (unsigned int i=1; i<=5; i++) {
	      double d = abs(prototypes[i]-it->image_distance);
	      if (d<bestdist) {
		bestdist=d;
		bestprototype=i;
	      }
	    }
	    values[bestprototype]+=it->image_distance;
	    values2[bestprototype]+=it->image_distance*it->image_distance;
	    value_count[bestprototype]++;
	    iter_wo_assignments[bestprototype]=0;
	  }
	  it++;
	}
	for (unsigned int i=1; i<=5; i++) {
	  prototypes[i]=values[i]/=(static_cast<double>(value_count[i])+1e-20);  // 1e-20 um Division durch 0 zu vermeiden
	  values2[i]=values2[i]/(static_cast<double>(value_count[i])+1e-20)-values[i]*values[i];
	}
	
      }
      clusters_okay=true;
      unsigned int similar_clusters=0;
      unsigned int cluster_wo_assignments=0;
      for (unsigned int i=1; i<5; i++) {
	if (abs(values[i]-values[i+1])<=5) {
	  clusters_okay=false;
	  similar_clusters=i+1;
	}
      }
      for (unsigned int i=1; i<=5; i++) {
	iter_wo_assignments[i]++;
	if (iter_wo_assignments[i]>10) {
	  clusters_okay=false;
	  cluster_wo_assignments=i;
	}
      }
      if (angleiter>=0)
	clusters_okay=true;
      if (!clusters_okay) {
	unsigned int broad_cluster=0;
	double max_var=0;
	for (unsigned int i=1; i<=5; i++) {
	  if (values2[i]>max_var && i!=similar_clusters && i!=cluster_wo_assignments) {
	    max_var = values2[i];
	    broad_cluster=i;
	  }
	}
	
	unsigned int min_cluster = (similar_clusters>0 ? similar_clusters : cluster_wo_assignments);
	if (min_cluster<broad_cluster) {
	  for (unsigned int i=min_cluster; i<broad_cluster; i++)
	    prototypes[i]=prototypes[i+1];
	  prototypes[broad_cluster-1]-=2;
	  prototypes[broad_cluster]+=2;
	} else if (min_cluster>broad_cluster) {
	  for (unsigned int i=min_cluster; i>broad_cluster; i--)
	    prototypes[i]=prototypes[i-1];
	  prototypes[broad_cluster]-=2;
	  prototypes[broad_cluster+1]+=2;
	}
      }
      
    }
    
    if (angleiter>=0) {
      // die nicht vom Clustern betroffenen zurueckschreiben
      deque<MarkerInfo>::const_iterator it = markers.begin();
      deque<MarkerInfo>::const_iterator itend = markers.end();
      while (it<itend) {
	if (it->angle.in_between (rightangle, leftangle)) {
	  if (it->true_distance>=6) {
	    values[it->true_distance]+=it->image_distance;
	    value_count[it->true_distance]++;
	  }
	}
	it++;
      }
      prototypes[6]=values[6]/=(static_cast<double>(value_count[6])+1e-20);
      prototypes[7]=values[7]/=(static_cast<double>(value_count[7])+1e-20);
      
      // die Werte schreiben:
      dest << angleiter << " 0";  // weshalb die Null, weiss ich nicht
      for (unsigned int j=0; j<num_markers-1; j++)
	dest << ' ' << values[j];
      dest << ' ' << 0.5*(values[6]+values[7]) << ' ' << values[7] << '\n';
    }
  }
  
}
  

