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

#include "ColorClasses.h"

namespace Tribots {

  ColorClassInfoList* ColorClassInfoList::theColorClassInfoList = 0;

  const ColorClassInfoList* 
  ColorClassInfoList::getColorClassInfoList(const ConfigReader* config)
  {
    if (!theColorClassInfoList) {
      if (config) {
	theColorClassInfoList = new ColorClassInfoList(*config);
      }
      else {
	theColorClassInfoList = new ColorClassInfoList();
      }
    }
    return theColorClassInfoList;
  }
  
  ColorClassInfo:: ColorClassInfo(int id, 
				  std::string name, 
				  const RGBTuple& color,
				  bool findTransitions, 
				  bool findPoints,
				  bool findOnlyFirst)
    : id(id), name(name), color(color), findTransitions(findTransitions), findOnlyFirst(findOnlyFirst),findPoints(findPoints)
  {}

  ColorClassInfoList::ColorClassInfoList()
  {
    std_init ();
  }

  ColorClassInfoList::~ColorClassInfoList()
  {
    for (unsigned int i=0; i < classList.size(); i++) {
      delete classList[i];
    }
    classList.clear();
  }

  ColorClassInfoList::ColorClassInfoList(const ConfigReader&)
  {
    std_init ();
  }

  void ColorClassInfoList::std_init()
  {
    RGBTuple black  = {   0,   0,   0 };
    RGBTuple white  = { 255, 255, 255 };
    RGBTuple grey   = { 100, 100, 100 };
    RGBTuple red    = { 255,   0,   0 };
    RGBTuple green  = {   0, 255,   0 };
    RGBTuple blue   = {   0,   0, 255 };
    RGBTuple yellow = { 188, 188,  60 };

    RGBTuple cyan   = {  50,  50,  50 };
    RGBTuple magenta= {  50,  50,  50 };

    // create color infos according to COLOR_ enum:
    classList.resize(9);
    
    // CLASS: TRANSITIONS, POINTS

    // IGNORE: no, no
    classList[COLOR_IGNORE]= new ColorClassInfo(COLOR_IGNORE,
						"IGNORE", 
						black,
						false, false);    
    // LINE: yes, no
    classList[COLOR_LINE]  = new ColorClassInfo(COLOR_LINE,
						"LINE", 
						white,
						true, false);    
    // BALL: no, yes
    classList[COLOR_BALL]  = new ColorClassInfo(COLOR_BALL,
						"BALL", 
						red,
						false, true, true);    
    // FIELD: no, no
    classList[COLOR_FIELD] = new ColorClassInfo(COLOR_FIELD,
						"FIELD", 
						green,
						false, false);    
    // OBSTACLE: no, no
    classList[COLOR_OBSTACLE] = new ColorClassInfo(COLOR_OBSTACLE,
						   "OBSTACLE", 
						   grey,
						   true, false, true);    
    // BLUE: no, no
    classList[COLOR_BLUE]  = new ColorClassInfo(COLOR_BLUE,
						"BLUE", 
						blue,
						false, false);    
    // YELLOW: no, no
    classList[COLOR_YELLOW]= new ColorClassInfo(COLOR_YELLOW,
						"YELLOW", 
						yellow,
						false, false);    

    // Next two classes are added for compatibility reasons only (
    // want to be compatible to Stephan Welkers Tool at the moment)

    // MAGENTA: no, no
    classList[COLOR_MAGENTA]=new ColorClassInfo(COLOR_MAGENTA,
						"MAGENTA", 
						magenta,
						false, false);    

    // CYAN: no, no
    classList[COLOR_CYAN]  = new ColorClassInfo(COLOR_CYAN,
						"cyan", 
						cyan,
						false, false);    

  }
    

}
