/***************************************************************************
                          V2d.cpp  -  description
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

#include "v2d.h"
#include <iostream>


/** No descriptions */
V2d * V2d::operator= (V2d * _pEuklidianPoint)
{
  this->x = _pEuklidianPoint->x;
  this->y = _pEuklidianPoint->y;
  return this;
 }

V2d *
V2d::operator= (V2d _EuklidianPoint)
{
  this->x = _EuklidianPoint.x;
  this->y = _EuklidianPoint.y;
  return this;
return &_EuklidianPoint;

}

V2d::V2d (const V2d & _EuklidianPoint)
{
  this->x = _EuklidianPoint.x;
  this->y = _EuklidianPoint.y;
}


V2d
V2d::operator+ (const V2d & v) const
{
  return V2d (x + v.x, y + v.y);
}

V2d
V2d::operator- (const V2d & v) const
{
  return V2d (x - v.x, y - v.y);
}

V2d
V2d::operator- (V2d *v) const
{
  return V2d (this->x - v->x, this->y - v->y);
}



V2d
V2d::operator* (double scalar) const
{
  return V2d (x * scalar, y * scalar);
}

void
V2d::operator+= (const V2d & v)
{
  this->x += v.x;
  this->y += v.y;
}


void
V2d::operator-= (const V2d & v)
{
  x -= v.x;
  y -= v.y;
}

void
V2d::operator*= (const V2d & v)
{
  x *= v.x;
  y *= v.y;
}


void
V2d::operator/= (const V2d & v)
{
  x /= v.x;
  y /= v.y;
}




V2d
V2d::scale (double scalar)
{
  return V2d (x * scalar, y * scalar);
}




void 
V2d::multiply_with_minus_i()
{    //  Rotates by -90 deg
  double dum=x;
  x=y;y=-dum;
}

void
V2d::multiply_with_i()
{     //Rotates by 90
  double dum=x;
  x=-y;y=dum;
}


V2d
V2d::orthogonal ()
{
  return V2d (-y, x);
}



double
V2d::angle_RAD ()
{
  return angle () / M_PI;
}



double
V2d::angle (V2d that)
{
  double winkel = (that.angle () - this->angle ());
  if (winkel < M_PI)
    winkel = winkel + 2 * M_PI;
  if (winkel > M_PI)
    winkel = winkel - 2 * M_PI;

  return winkel;
}



double
V2d::angle_DEG (V2d that)
{
  return angle (that) * 180 / M_PI;
}

double
V2d::angle_RAD (V2d that)
{
  return angle (that) / M_PI;
}


bool
V2d::is_left_of (V2d that)
{

  if (angle (that) > 0)
    return false;
  else
    return true;
}


V2d
V2d::rotate_DEG (double theta)
{

  return this->rotate (theta * M_PI / 180);


}






void
V2d::einheitsVektor ()
{
  this->x = 1;
  this->y = 0;


}



V2d
V2d::mirrorx ()
{
    return V2d(-x,y);

}

V2d
V2d::mirrory ()
{
  return V2d(x,-y);

}





V2d
V2d::calculateLineIntersection (V2d location1, V2d direction1, V2d location2,
				V2d direction2)
{
  double x1, x2, y1, y2, a1, a2, b1, b2;
  double k;
  x1 = location1.x;
  x2 = location1.y;
  y1 = direction1.x;
  y2 = direction1.y;
  a1 = location2.x;
  a2 = location2.y;
  b1 = direction2.x;
  b2 = direction2.y;
  //cout << " | "<< x1<< " | " <<x2<< " | "<<y1<< " | "<<y2<< " | "<<a1<< " | "<<a2<< " | "<<b1<< " | "<<b2<<endl;
  if (y1 == 0)
    y1 = 0.00001;
  if (y2 == 0)
    y2 = 0.00001;
  if (x1 == 0)
    x1 = 0.00001;
  if (x2 == 0)
    x2 = 0.00001;
  if (a1 == 0)
    a1 = 0.00001;
  if (a2 == 0)
    a2 = 0.00001;
  if (b1 == 0)
    b1 = 0.00001;
  if (b2 == 0)
    b2 = 0.00001;

  k =
    ((a1 / y1) + ((x2 * b1) / (b2 * y1)) - ((a2 * b1) / (b2 * y1)) -
     (x1 / y1));
//cout <<"ka : "<< k <<"  --"<< endl;
  k = k / (1.0f - (y2 * b1) / (b2 * y1));
//k ist jetzt der faktor mit dem eine linie skaliert werden muss!!!
//cout <<"ka : "<< k <<"  --"<< endl;

  return (location1 + direction1.scale (k));

}


V2d
V2d::calculateCircleCenter (V2d l1, V2d l2, V2d l3)
{
  // zuerst die gerade einer mittelsenkrechten zwischen l1 und l2 herausfinden
  V2d ov1, rv1;
  ov1 = l1 + (l2 - l1).scale (0.5);
  rv1 = (l2 - l1).orthogonal ();
  // jetzt die mittelsenkrechte zwischen l2 und l3 :
  V2d ov2, rv2;
  ov2 = l2 + (l3 - l2).scale (0.5);
  rv2 = (l3 - l2).orthogonal ();
  //dann die geraden schneiden!!

  return this->calculateLineIntersection (ov1, rv1, ov2, rv2);
}





std::ostream & operator<< (std::ostream & o, const V2d & v)
{
  //return o << "#V[" << v.x << "," << v.y << "]" ;
  return o << "(" << v.x << "/" << v.y << ")";
}


/** Eingabe sind Polarkoordinaten , Ausgabe ist ein V2d mit den entsprechenden
Koordinaten im Kamerabild */
V2d
V2d::transform (double _phi, int _radius)
{
  return V2d ((_radius * cos (_phi)), (_radius * sin (_phi)));
}



bool
V2d::equalTo (V2d _EuklidianPoint, double range)
{

//  cout<<"equalTo method in EP input: "<<this->toString()<<" , "<<_EuklidianPoint.toString()<<endl;
  V2d tmp = V2d (*this - _EuklidianPoint);

  bool cool = (tmp.length () <= range);
//  cout<<"returned bool: "<<cool<<endl;
  return cool;
}




double
V2d::dotProduct (V2d * p)
{
  return (this->x * p->x) + (this->y * p->y);
}



double
V2d::distanceToLine (V2d p1, V2d p2)
{
  V2d v = p2 - p1;
  V2d w = *this - p1;
  double c1 = w.dotProduct (&v);
  if (c1 <= 0)
    return (p1 - this).length ();
  double c2 = v.dotProduct (&v);
  if (c2 <= c1)
    return (p2 - this).length ();
  double b = c1 / c2;
  V2d intersect = p1 + v.scale (b);
  return (intersect - this).length ();
}


double
V2d::distanceToCircle (V2d middle, V2d radius)
{
  V2d intersect;
  double dist=(*this-middle).length();
  double dradius=radius.length();
  if (dist<dradius) return dradius-dist;
  else return dist-dradius;
}


/*
int main()
{
V2d a,b,c;
vector<Shape*>  lineset;
a.setPoint(0,0);
b.setPoint(100);
c.setPoint(250,0);

Line line(a,b);
Circle circle(a,b);
std::cout<< "Grösse des linesets"<<lineset.size()<<std::endl;
lineset.push_back(&line);
std::cout<< "Grösse des linesets"<<lineset.size()<<std::endl;
lineset.push_back(&circle);
std::cout<< "Grösse des linesets"<<lineset.size()<<std::endl;


std::cout<<"Linedistance"<<" "<< lineset[0]->distanceTo(c)  <<std::endl;
std::cout<<"Linedistance"<<" "<< lineset[1]->distanceTo(c)  <<std::endl;

     for (int f=0;f<lineset.size();f++)
     {
  //       std::cout<< "Hier"<<std::endl;
       std::cout<<lineset[f]->distanceTo(c) <<std::endl;
       }

  }



*/
