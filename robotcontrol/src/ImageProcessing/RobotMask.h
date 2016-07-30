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

#ifndef _robotmask_h_
#define _robotmask_h_

#include <string>
#include <iostream>

/**
 * Stellt eine Maske bereit, in der ungültige Bildbereiche (Aufbauten) 
 * markiert sind. Hierzu kann ein PPM Bild (Binär, Typ 6) eingelesen 
 * werden. Gültige Bereiche müssen hier weiss (255,255,255), ungültige
 * schwarz markiert sein. Nachdem eine Maske geladen wurde, erhält man
 * mittels der statischen Methode getMask() Zugriff auf sie; derzeit
 * existiert nur eine einzige Maske im Programm.
 *
 * Diese Klasse verwendet _nicht_ die Implementierung der Bildklasse,
 * da hier für die isValid - Methode eine möglichst niedrige Rechenzeit
 * erzielt werden sollte. Intern werden nicht die eingelesenen RGB-Werte
 * sondern nur true / false gespeichert. Diese Klasse ist aber zu den vom 
 * der PPMIO erzeugten Bildern kompatibel.
 */
class RobotMask {
public:
  ~RobotMask();

  static void load(std::string filename);

  /**
   * Liefert einen Zeiger auf eine initialisierte Maske oder 0, wenn noch 
   * keine Maske geladen wurde.
   */
  inline static const RobotMask* getMask();

  inline bool isValid(int x, int y) const;

protected:
  RobotMask(std::string filename);
  static const RobotMask* instance;

  bool* mask;
  int width;
  int height;
};

// inlines ////////////////////////////////////////////////////////////////////

bool
RobotMask::isValid(int x, int y) const
{
  if (x>=0 && y>=0 && x<width && y < height) {  
    return mask[x+y * width];
  }
  else {
    return false;
  }
}

const RobotMask* 
RobotMask::getMask()
{
  return instance; 
}


#endif


