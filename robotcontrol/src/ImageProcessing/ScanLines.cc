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

#include "ScanLines.h"
#include "RobotMask.h"
#include <vector>

namespace Tribots {

  using namespace std;

  // Linien an der Maske und am Bildrand abschneiden bzw. aufteilen. Wurde eine
  // RobotMask geladen, wird diese verwendet, um sowohl nicht maskierte Pixel
  // als auch Pixel ausserhalb des Bildbereiches zu finden (isValid macht 
  // beides). Wurde keine Maske geladen, wird die Position auf der Scanlinie
  // nur daraufhin ueberprueft, ob sie im Bildbereich liegt.

  void ScanLines::insertScanLine(const LineSegment& line, 
				 int width, int height)
  {
    Vec direction = line.p2 - line.p1;
    int maxSteps = (int) direction.length();
    int steps = 0;

    direction = direction.normalize();
    const RobotMask* robotMask = RobotMask::getMask(); // aktuelle Robotermaske

    Vec pos=line.p1;           // Startpunkt
    for (; steps <= maxSteps; steps++) {// Ersten gueltigen Punkt suchen
      if ((robotMask != 0 && robotMask->isValid((int)pos.x, (int)pos.y)) ||
	  (robotMask == 0 && pos.x >= 0 && pos.y >= 0 && 
	   pos.x < width && pos.y < height)) {
	break;                 // Punkt ist gueltig
      }
      pos = pos + direction;
    }
    if (steps >= maxSteps) {   // Linie maximal ein Punkt groß -> uninteressant
      return;
    }
    Vec startPos = pos;        // Anfang merken
    for (; steps <= maxSteps; steps++) {// vorangehen, bis Punkt ungueltig
      if ((robotMask != 0 && ! robotMask->isValid((int)pos.x, (int)pos.y)) ||
	  (robotMask == 0 && 
	   (pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height))) {
        scanLines.push_back(LineSegment(startPos, pos + -1. * direction));
	if (robotMask != 0) {  // Punkt kann noch im Bildbereich sein,
	                       // daher auch den Rest ueberpruefen
	  insertScanLine(LineSegment(pos, line.p2), width, height); 
	}
	return ;               // Fertig!
      }
      pos = pos + direction;
    }
    scanLines.push_back(LineSegment(startPos, line.p2)); // Linie bis Ende ok
  }

  ScanLines::ScanLines(const Vec& middle, int innerRadius, int outerRadius, 
		       int width, int height, int n) 
  {    

    
    int distance = (int)(innerRadius * 1.4);
    for (int i=0; i < 5; i++) {
      LineSegment horizontal(Vec(-distance,0), Vec(distance, 0));
      LineSegment vertical(Vec(0,-distance), Vec(0, distance));
      insertScanLine(horizontal.translate(Vec(0,-distance)).translate(middle), 
		     width, height);
      insertScanLine(horizontal.translate(Vec(0, distance)).translate(middle), 
		     width, height);
      insertScanLine(vertical.translate(Vec(-distance, 0)).translate(middle), 
		     width, height);
      insertScanLine(vertical.translate(Vec( distance, 0)).translate(middle), 
		     width, height);
      distance+= 15;
    }




    Angle stepAngle = Angle::deg_angle(360. / n); // Schrittweite berechnen
    Angle halfStepAngle = stepAngle;             // Schrittweite für die kurze
    halfStepAngle*=0.5;                          // Zwischenlinie

    LineSegment baseLine1(Vec(0, innerRadius), Vec(0, outerRadius));
    LineSegment baseLine2(Vec(0, (2*innerRadius+outerRadius)/3),
			  Vec(0, outerRadius));
    baseLine2.s_rotate(halfStepAngle);

    // Radiale Scanlinien:
    // Vorgehensweise (i*Mal):
    // 1. Standardlinie von (0,innerRadius) nach (0,outerRadius) und zweite
    //    Line von (0, (innerRadius+outerRadius)/2) nach (o, outerRadius),
    //    die um stepAnlge/2 Grad rotiert wurde (Verdichtung in den Aussen-
    //    bereichen des Bildes)
    // 2. Rotation um stepAngle * i (Schrittweite)
    // 3. Translation zum Mittlepunkt middle

    // hierarchisches einfuegen der langen Scanlinien
    vector<bool> line_set (n);
    for (int i=0; i<n; i++)
      line_set[i]=false;
    int num_lines_set=0;
    
    // 0. Linie einfuegen
    insertScanLine (baseLine1.translate (middle),width,height);
    line_set[0]=true;
    num_lines_set++;

    // weitere Linien hierarchisch einfuegen
    int base=2;
    while (num_lines_set<n) {
      for (int i=1; i<base; i+=2) {
	int j = (n*i)/base;
	if (!line_set[j]) {
	  insertScanLine (baseLine1.rotate (j*stepAngle).translate (middle),width,height);
	  line_set[j]=true;
	  num_lines_set++;
	}
      }
      base*=2;
    }
    
    // hierarchisches einfuegen der kurzen Scanlinien
    // 0. Linie einfuegen
    insertScanLine (baseLine2.translate (middle),width,height);
    num_lines_set--;
    line_set[0]=false;

    // weitere Linien hierarchisch einfuegen
    base=2;
    while (num_lines_set>0) {
      for (int i=1; i<base; i+=2) {
	int j = (n*i)/base;
	if (line_set[j]) {
	  insertScanLine (baseLine2.rotate (j*stepAngle).translate (middle),width,height);
	  line_set[j]=false;
	  num_lines_set--;
	}
      }
      base*=2;
    }

  }

  ScanLines::ScanLines(const Vec& start, const Vec& end, int width, int height)
  {
    insertScanLine(LineSegment(start, end), 
		   width <= 0 ? 999999 : width, 
		   height <= 0 ? 999999 : height);
  }
}
