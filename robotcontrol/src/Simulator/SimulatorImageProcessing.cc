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


#include "SimulatorImageProcessing.h"
#include "../ImageProcessing/ImageProcessingFactory.h"
#include "../Fundamental/random.h"
#include "../Structures/Journal.h"
#include "../WorldModel/WorldModel.h"
#include <cmath>

using namespace Tribots;
using namespace std;

// Folgendes wird zur Anmeldung bei der Factory benoetigt:
namespace {
  class Builder : public ImageProcessingBuilder {
    static Builder the_builder;
  public:
    Builder () {
      ImageProcessingFactory::get_image_processing_factory ()->sign_up (string("Simulator"), this);
    }
    ImageProcessingType* get_image_processing (const std::string&, const ConfigReader& reader, ImageProcessingType*) throw (TribotsException,bad_alloc) {
      return new SimulatorImageProcessing (reader);
    }
  };
  Builder the_builder;
}






SimulatorImageProcessing::SimulatorImageProcessing (const ConfigReader& reader) throw (std::bad_alloc, Tribots::InvalidConfigurationException) : the_sim_client (NULL) {
  string confline;
  if (reader.get("Simulator::robot_config_file", confline)<=0) {
    JERROR("no config line \"Simulator::robot_config_file\" found");
    throw Tribots::InvalidConfigurationException("Simulator::robot_config_file");
  }
  try{
    the_sim_client=SimClient::get_sim_client (confline.c_str());
  }catch(std::invalid_argument&){
    JERROR("cannot connect to simulator");
    throw Tribots::InvalidConfigurationException("Simulator::robot_config_file");
  }
  
  noise_level = 0.001;
  mis_probability = 0;
  goal_mis_probability=0;
  ball_mis_probability=0;
  line_vision_radius = 4000;
  double d;
  if (reader.get ("SimulatorImageProcessing::noise_level",d)>=1 && d>0)
    noise_level=d;
  if (reader.get ("SimulatorImageProcessing::mis_probability",d)>=1 && d>=0 && d<=1)
    mis_probability=d;
  if (reader.get ("SimulatorImageProcessing::goal_mis_probability",d)>=1 && d>=0 && d<=1)
    goal_mis_probability=d;
  if (reader.get ("SimulatorImageProcessing::ball_mis_probability",d)>=1 && d>=0 && d<=1)
    ball_mis_probability=d;
  if (reader.get ("SimulatorImageProcessing::line_vision_radius",d)>=1 && d>=0)
    line_vision_radius=d;

  // Linien eintragen
  const FieldGeometry& fg (WorldModel::get_main_world_model().get_field_geometry());
  lines.push_back (LineSegment (Vec(-0.5*fg.field_width,-0.5*fg.field_length), Vec(0.5*fg.field_width,-0.5*fg.field_length)));
  lines.push_back (LineSegment (Vec(-0.5*fg.field_width,0.5*fg.field_length), Vec(0.5*fg.field_width,0.5*fg.field_length)));
  lines.push_back (LineSegment (Vec(-0.5*fg.field_width,-0.5*fg.field_length), Vec(-0.5*fg.field_width,0.5*fg.field_length)));
  lines.push_back (LineSegment (Vec(0.5*fg.field_width,-0.5*fg.field_length), Vec(0.5*fg.field_width,0.5*fg.field_length)));
  lines.push_back (LineSegment (Vec(-0.5*fg.field_width,0), Vec(0.5*fg.field_width,0)));
  if (fg.goal_area_length>0 && fg.goal_area_width>0) {
    lines.push_back (LineSegment (Vec (-0.5*fg.goal_area_width,-0.5*fg.field_length), Vec(-0.5*fg.goal_area_width,-0.5*fg.field_length+fg.goal_area_length)));
    lines.push_back (LineSegment (Vec (0.5*fg.goal_area_width,-0.5*fg.field_length), Vec(0.5*fg.goal_area_width,-0.5*fg.field_length+fg.goal_area_length)));
    lines.push_back (LineSegment (Vec (-0.5*fg.goal_area_width,-0.5*fg.field_length+fg.goal_area_length), Vec(0.5*fg.goal_area_width,-0.5*fg.field_length+fg.goal_area_length)));
    lines.push_back (LineSegment (Vec (-0.5*fg.goal_area_width,0.5*fg.field_length), Vec(-0.5*fg.goal_area_width,0.5*fg.field_length-fg.goal_area_length)));
    lines.push_back (LineSegment (Vec (0.5*fg.goal_area_width,0.5*fg.field_length), Vec(0.5*fg.goal_area_width,0.5*fg.field_length-fg.goal_area_length)));
    lines.push_back (LineSegment (Vec (-0.5*fg.goal_area_width,0.5*fg.field_length-fg.goal_area_length), Vec(0.5*fg.goal_area_width,0.5*fg.field_length-fg.goal_area_length)));
  }
  if (fg.penalty_area_length>0 && fg.penalty_area_width>0) {
    lines.push_back (LineSegment (Vec (-0.5*fg.penalty_area_width,-0.5*fg.field_length), Vec(-0.5*fg.penalty_area_width,-0.5*fg.field_length+fg.penalty_area_length)));
    lines.push_back (LineSegment (Vec (0.5*fg.penalty_area_width,-0.5*fg.field_length), Vec(0.5*fg.penalty_area_width,-0.5*fg.field_length+fg.penalty_area_length)));
    lines.push_back (LineSegment (Vec (-0.5*fg.penalty_area_width,-0.5*fg.field_length+fg.penalty_area_length), Vec(0.5*fg.penalty_area_width,-0.5*fg.field_length+fg.penalty_area_length)));
    lines.push_back (LineSegment (Vec (-0.5*fg.penalty_area_width,0.5*fg.field_length), Vec(-0.5*fg.penalty_area_width,0.5*fg.field_length-fg.penalty_area_length)));
    lines.push_back (LineSegment (Vec (0.5*fg.penalty_area_width,0.5*fg.field_length), Vec(0.5*fg.penalty_area_width,0.5*fg.field_length-fg.penalty_area_length)));
    lines.push_back (LineSegment (Vec (-0.5*fg.penalty_area_width,0.5*fg.field_length-fg.penalty_area_length), Vec(0.5*fg.penalty_area_width,0.5*fg.field_length-fg.penalty_area_length)));
  }
  if (fg.center_circle_radius>0)
    arcs.push_back (Arc(Vec(0,0),fg.center_circle_radius, Angle::zero, Angle::deg_angle (259.99)));
  if (fg.corner_arc_radius>0) {
    arcs.push_back (Arc(Vec(0.5*fg.field_width,0.5*fg.field_length),fg.corner_arc_radius,Angle::half,Angle::three_quarters));
    arcs.push_back (Arc(Vec(-0.5*fg.field_width,0.5*fg.field_length),fg.corner_arc_radius,Angle::three_quarters,Angle::zero));
    arcs.push_back (Arc(Vec(0.5*fg.field_width,-0.5*fg.field_length),fg.corner_arc_radius,Angle::quarter,Angle::half));
    arcs.push_back (Arc(Vec(-0.5*fg.field_width,-0.5*fg.field_length),fg.corner_arc_radius,Angle::zero,Angle::quarter));
  }
}

SimulatorImageProcessing::~SimulatorImageProcessing () throw () {;}

void SimulatorImageProcessing::process_image () throw () {
  the_sim_client->update();
  const FieldGeometry& fg (WorldModel::get_main_world_model().get_field_geometry());
  VisibleObjectList vol;
  vol.timestamp = the_sim_client->timestamp;

  // Ball eintragen:
  if (!brandom (ball_mis_probability))
    vol.objectlist.push_back (VisibleObject ((the_sim_client->ball_position-the_sim_client->robot_position).rotate(-the_sim_client->robot_heading)+noise_level*n2random(), VisibleObject::ball));

  // Hindernisse eintragen:
  std::vector<Vec>::iterator oit = the_sim_client->obstacle_positions.begin();
  std::vector<Vec>::iterator oit_end = the_sim_client->obstacle_positions.end();
  while (oit<oit_end) {
    if (!brandom (mis_probability)) 
      vol.objectlist.push_back (VisibleObject (((*oit)-the_sim_client->robot_position).rotate(-the_sim_client->robot_heading)+noise_level*n2random(), VisibleObject::obstacle, 500));  // Hindernisbreite pauschal mit 500mm angenommen
    oit++;
  }

  // Tore eintragen:
  Vec blue_goal (0,-0.5*fg.field_length-fg.goal_length);
  if (!brandom (goal_mis_probability))
    vol.objectlist.push_back (VisibleObject ((blue_goal-the_sim_client->robot_position).rotate(-the_sim_client->robot_heading)+noise_level*n2random(), VisibleObject::blue_goal));
  Vec yellow_goal (0, 0.5*fg.field_length+fg.goal_length);
  if (!brandom (goal_mis_probability))
    vol.objectlist.push_back (VisibleObject ((yellow_goal-the_sim_client->robot_position).rotate(-the_sim_client->robot_heading)+noise_level*n2random(), VisibleObject::yellow_goal));

  // Linien eintragen:
  try{
    deque<Line> beams;
    for (double a=0; a<180; a+=15)
      beams.push_back (Line (the_sim_client->robot_position, the_sim_client->robot_position+Vec::unit_vector (the_sim_client->robot_heading+Angle::deg_angle(a))));
    deque<Line>::iterator bit = beams.begin();
    deque<Line>::iterator bit_end = beams.end();
    deque<LineSegment>::iterator lit;
    deque<LineSegment>::iterator lit_end = lines.end();
    deque<Arc>::iterator ait;
    deque<Arc>::iterator ait_end = arcs.end();
    double lvr2 = line_vision_radius*line_vision_radius;
    while (bit<bit_end) {
      lit=lines.begin();
      while (lit<lit_end) {
	vector<Vec> pvec = intersect (*lit, *bit);
	if (pvec.size()>0) {
	  if ((lit->distance (pvec[0])<0.01) && (pvec[0]-the_sim_client->robot_position).squared_length()<lvr2)
	    if (!brandom (mis_probability))
	      vol.objectlist.push_back (VisibleObject ((pvec[0]-the_sim_client->robot_position).rotate(-the_sim_client->robot_heading)+noise_level*n2random(), VisibleObject::white_line));
	}  // kein Schnittpunkt
	lit++;
      }
      ait=arcs.begin();
      while (ait<ait_end) {
	try{
	  vector<Vec> p = intersect (*ait, *bit);
	  for (unsigned int i=0; i<p.size(); i++)
	    if ((p[i]-the_sim_client->robot_position).squared_length()<lvr2)
	      if (!brandom (mis_probability))
		vol.objectlist.push_back (VisibleObject ((p[i]-the_sim_client->robot_position).rotate(-the_sim_client->robot_heading)+noise_level*n2random(), VisibleObject::white_line));
	}catch(exception&){;}  // kein Schnittpunkt oder bad_alloc
	ait++;
      }
      bit++;
    }
  }catch(std::bad_alloc){;}  // ignorieren
  WorldModel::get_main_world_model().set_visual_information (vol);
}

