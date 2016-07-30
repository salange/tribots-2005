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


#include "FieldGeometry.h"
#include <cmath>

Tribots::FieldGeometry::FieldGeometry () throw () {;}

Tribots::FieldGeometry::FieldGeometry (const ConfigReader& vread) throw (InvalidConfigurationException) {
  if (vread.get ("FieldGeometry::field_length", field_length)<=0 || field_length<0)
    throw Tribots::InvalidConfigurationException ("field_length");
  if (vread.get ("FieldGeometry::field_width", field_width)<=0 || field_width<0)
    throw Tribots::InvalidConfigurationException ("field_width");
  if (vread.get ("FieldGeometry::side_band_width", side_band_width)<=0 || side_band_width<0)
    throw Tribots::InvalidConfigurationException ("side_band_width");
  if (vread.get ("FieldGeometry::goal_band_width", goal_band_width)<=0 || goal_band_width<0)
    throw Tribots::InvalidConfigurationException ("goal_band_width");
  if (vread.get ("FieldGeometry::goal_area_length", goal_area_length)<=0 || goal_area_length<0)
    throw Tribots::InvalidConfigurationException ("goal_area_length");
  if (vread.get ("FieldGeometry::goal_area_width", goal_area_width)<=0 || goal_area_width<0)
    throw Tribots::InvalidConfigurationException ("goal_area_width");
  if (vread.get ("FieldGeometry::penalty_area_length", penalty_area_length)<=0 || penalty_area_length<0)
    throw Tribots::InvalidConfigurationException ("penalty_area_length");
  if (vread.get ("FieldGeometry::penalty_area_width", penalty_area_width)<=0 || penalty_area_width<0)
    throw Tribots::InvalidConfigurationException ("penalty_area_width");
  if (vread.get ("FieldGeometry::center_circle_radius", center_circle_radius)<=0 || center_circle_radius<0)
    throw Tribots::InvalidConfigurationException ("center_circle_radius");
  if (vread.get ("FieldGeometry::corner_arc_radius", corner_arc_radius)<=0 || corner_arc_radius<0)
    throw Tribots::InvalidConfigurationException ("corner_arc_radius");
  if (vread.get ("FieldGeometry::penalty_marker_distance", penalty_marker_distance)<=0 || penalty_marker_distance<0)
    throw Tribots::InvalidConfigurationException ("penalty_marker_distance");
  if (vread.get ("FieldGeometry::line_thickness", line_thickness)<=0 || line_thickness<0)
    throw Tribots::InvalidConfigurationException ("line_thickness");
  if (vread.get ("FieldGeometry::border_line_thickness", border_line_thickness)<=0 || border_line_thickness<0)
    throw Tribots::InvalidConfigurationException ("border_line_thickness");
  if (vread.get ("FieldGeometry::goal_width", goal_width)<=0 || goal_width<0)
    throw Tribots::InvalidConfigurationException ("goal_width");
  if (vread.get ("FieldGeometry::goal_length", goal_length)<=0 || goal_length<0)
    throw Tribots::InvalidConfigurationException ("goal_length");
  if (vread.get ("FieldGeometry::goal_height", goal_height)<=0 || goal_height<0)
    throw Tribots::InvalidConfigurationException ("goal_height");
  if (vread.get ("FieldGeometry::pole_height", pole_height)<=0 || pole_height<0)
    throw Tribots::InvalidConfigurationException ("pole_height");
  if (vread.get ("FieldGeometry::pole_diameter", pole_diameter)<=0 || pole_diameter<0)
    throw Tribots::InvalidConfigurationException ("pole_diameter");
  if (vread.get ("FieldGeometry::pole_position_offset_x", pole_position_offset_x)<=0 || pole_position_offset_x<0)
    throw Tribots::InvalidConfigurationException ("pole_position_offset_x");
  if (vread.get ("FieldGeometry::pole_position_offset_y", pole_position_offset_y)<=0 || pole_position_offset_y<0)
    throw Tribots::InvalidConfigurationException ("pole_position_offset_y");
  if (vread.get ("FieldGeometry::ball_diameter", ball_diameter)<=0 || ball_diameter<0)
    throw Tribots::InvalidConfigurationException ("ball_diameter");
  std::string lineset;
  if (vread.get ("FieldGeometry::lineset_type", lineset)<=0)
    lineset_type=0;
  else {
    if (lineset == "center_circle")
      lineset_type=0;
    else if (lineset == "center_triangle")
      lineset_type=1;
    else if (lineset == "center_triangle_inv")
      lineset_type=2;
    else
      throw Tribots::InvalidConfigurationException ("lineset_type");
  }
}

bool Tribots::FieldGeometry::operator== (const FieldGeometry& fg) const throw () {
  if (std::abs(field_length-fg.field_length)>10) return false;
  if (std::abs(field_width-fg.field_width)>10) return false;
  if (std::abs(side_band_width-fg.side_band_width)>10) return false;
  if (std::abs(goal_band_width-fg.goal_band_width)>10) return false;
  if (std::abs(goal_area_length-fg.goal_area_length)>10) return false;
  if (std::abs(goal_area_width-fg.goal_area_width)>10) return false;
  if (std::abs(penalty_area_length-fg.penalty_area_length)>10) return false;
  if (std::abs(penalty_area_width-fg.penalty_area_width)>10) return false;
  if (std::abs(center_circle_radius-fg.center_circle_radius)>10) return false;
  if (std::abs(corner_arc_radius-fg.corner_arc_radius)>10) return false;
  if (std::abs(penalty_marker_distance-fg.penalty_marker_distance)>10) return false;
  if (std::abs(line_thickness-fg.line_thickness)>10) return false;
  if (std::abs(border_line_thickness-fg.border_line_thickness)>10) return false;
  if (std::abs(goal_width-fg.goal_width)>10) return false;
  if (std::abs(goal_length-fg.goal_length)>10) return false;
  if (std::abs(goal_height-fg.goal_height)>10) return false;
  if (std::abs(pole_diameter-fg.pole_diameter)>10) return false;
  if (std::abs(pole_height-fg.pole_height)>10) return false;
  if (std::abs(pole_position_offset_x-fg.pole_position_offset_x)>10) return false;
  if (std::abs(pole_position_offset_y-fg.pole_position_offset_y)>10) return false;
  if (std::abs(ball_diameter-fg.ball_diameter)>10) return false;
  if (lineset_type!=fg.lineset_type) return false;
  return true;
}

bool Tribots::FieldGeometry::operator!= (const FieldGeometry& fg) const throw () {
  return !operator== (fg);
}

