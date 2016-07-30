/***************************************************************************
                          V2d.h  -  description
                             -------------------
    begin                : Mon Jan 27 2003
    copyright            : (C) 2003 by PG425
    email                : pg425@ls1.cs.uni-dortmund.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef V2d_H
#define V2d_H


#include <math.h>
#include <iostream>
#include "angle.h"
/**
  *@author PG425
  */


class V2d {

public:
  ///Konstruktor
  V2d (double _x, double _y):x(_x),y(_y) {}
  V2d () {x=0;    y=0;    }
    V2d (const V2d & _EuklidianPoint);
  //~V2d();


  V2d *operator= (V2d *);

  V2d *operator= (V2d);

  V2d operator+ (const V2d &) const;

  V2d operator- (const V2d &) const;

  V2d operator- (V2d *) const;

  void operator+= (const V2d &);
  void operator-= (const V2d &);
  void operator*= (const V2d &);
  void operator/= (const V2d &);




  /// Skaliert die Länge des Vektors um einen Double-Wert
  V2d operator* (double) const;
  /// Skaliert die Länge des Vektors um einen Double-Wert
  V2d scale (double);
  double length () const {
    return sqrt (x * x + y * y);
  }
  double sqr_length () const {
    return (x * x + y * y);
  }

  double sqr_norm() const {
    return x*x+y*y;
  }

  double norm() const {
    return sqrt( sqr_norm() );
  }
  
  double sqr_distance( V2d orig) const {
    return (*this - orig).sqr_norm() ;
  }
  
  /// Normalisieren des Vektors
  V2d normalize () const {
    double _length = length ();
    if (_length == 0)
      return V2d (0.0f, 0.0f);
    else
      return V2d (x / _length, y / _length);
  }

  void self_normalize(double len) {
    double n= len/norm();
    x *= n;
    y *= n;
  }
  
  double dot_product(const V2d& orig) const {return x*orig.x + y*orig.y;}
  
  /// umdrehen des Vektors
  V2d mirror () {return V2d (-x, -y);}

void multiply_with_minus_i();  //rotates -90 deg
void multiply_with_i();   // rotates 90 deg

  /// liefert einen rechten Winkel zu dem Vektor
  V2d orthogonal ();

  bool equalTo (V2d, double  range);


  V2d mirrorx ();
  V2d mirrory ();


  V2d transform (double _phi, int _radius);

  void init_polar(const double & n, const Angle& a) {
    x= n*cos(a);
    y= n*sin(a);
  }
  
  double angle (){if ((x == 0) && (y == 0))return 0.0f; return atan2 (y, x);}


  double angle_DEG (){  return angle () * 180 / M_PI;}
  double angle_RAD ();
  double angle (V2d);
  V2d angle_as_EP (V2d *that){  return V2d(1,0).rotate(angle(*that));}
  double angle_DEG (V2d);
  double angle_RAD (V2d);
  bool is_left_of (V2d);
  /** No descriptions */
  void setPoint (double _x, double _y){x=_x;y=_y;}
  void einheitsVektor ();
  // zwei Geraden schneiden ( 4 EP zur angabe von orts und richtungsvektoren)
  V2d calculateLineIntersection (V2d location1, V2d direction1, V2d location2,  V2d direction2);
  // Mitte eines Kreises aus 3 eps berechnen
  V2d calculateCircleCenter (V2d l1, V2d l2, V2d l3);
  double distanceToLine (V2d p1, V2d p2);
  double distanceToCircle (V2d p1, V2d p2);
  double dotProduct (V2d * p);
  V2d rotate (double theta){  double c = cos (theta);double s = sin (theta);  return V2d (c * x - s * y, s * x + c * y);}

  V2d rotate_DEG (double);


  /// DeltaX und Deltay Werte des Vektors
  double x;
  double y;
};





std::ostream & operator<< (std::ostream & o, const V2d & v);




class Line{
public:
  V2d from;
  V2d to;

  Line () {}

  Line (V2d _a, V2d _b) {
    from = _a;
    to = _b;
  }
  ~Line () {}

  double distanceTo (V2d point) {
    //  std::cout<<"Line!"<<std::endl;
   return point.distanceToLine (from, to);

  }
  V2d getFrom ()  {

    return this->from;

  }
  V2d getTo ()  {

    return this->to;

  }

};

class Circle {
public:
  V2d from;
  V2d to;
 Circle () {
     }

  Circle (V2d _a, V2d _b) {
    from = _a;
    to = _b;
  }

  ~Circle () {}

  double distanceTo (V2d point) {
    //   std::cout<<"Circle!!"<<std::endl;
    return point.distanceToCircle (from, to);
  }


  V2d getFrom ()  {

    return this->from;

  }
  V2d getTo ()  {

    return this->to;

  }

};


#endif
