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

#ifndef tribots_user_interface_type_h
#define tribots_user_interface_type_h


namespace Tribots {

  /** Schnittstelle der Bedienungs- und Kommunikationsschnittstelle
      Aufgaben: Anzeige relevanter Informationen, Abfrage von Benutzerinteraktionen 
      Schnittstelle dient auch zur Anbindung eines Kommunikationsmuls zur Kommunikation
      mit Zentralrechner oder zwischen verschiedenen Robotern */
  class UserInterfaceType {
  public:
    virtual ~UserInterfaceType() throw () {;}

    /** Verarbeitung von Nachrichten, sammeln von Informationen, Benutzinteraktion
	Rueckgabewert: false, wenn Programm beendet werden soll, true sonst */
    virtual bool process_messages () throw () =0;
  };
  
}

#endif