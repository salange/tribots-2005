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


#ifndef tribots_terminal_user_interface_h
#define tribots_terminal_user_interface_h

#include "../UserInterfaceType.h"
#include "../../Player/Player.h"
#include "../../Fundamental/Time.h"
#include "../../Fundamental/ConfigReader.h"
#include "../../ImageProcessing/Image.h"
#include "../../ImageProcessing/ImageProcessing.h"
#include <curses.h>
#include <string>

namespace Tribots {

  using std::string;

  /** einfache MMI auf Grundlage eines Terminalfensters mit Funktionalitaet zum 
      Starten/Stoppen des Roboters und Beenden des Programms 
      Seiteneffekte: greift auf das Weltmodell zu (lesend und schreibend) */
  class TerminalUserInterface : public UserInterfaceType {
  public:
    /** Konstruktor */
    TerminalUserInterface (const ConfigReader&, Player&, ImageProcessing&) throw ();

    /** Destruktor */
    virtual ~TerminalUserInterface () throw ();

    /** Tastaturabfrage */
    virtual bool process_messages () throw ();

  protected:
    /** Initialen Bildschirm aktualisieren */
    void init_window () throw ();

    /** Bildschirmanzeige aktualisieren */
    void update_window () throw ();

    /** Bild speichern */
    void save_image(const Image*) const throw(TribotsException);

    Player& the_player;
    ImageProcessing& the_image_processing;
    WINDOW* window;
    unsigned int single_step_mode;   ///< Einzelschrittmodus, wieviele Schritte noch?
    Time manual_start_timer;         ///< Timer, um ggf. bei manuellem Start verzoegert loszufahren
    bool wait_for_manual_start;      ///< manueller Start aktiviert?
    int manual_start_sec;            ///< bei manuellem Start wie lange warten?
    bool requestedImage;             ///< wurde ein Bild angefragt?
    string debug_image_filename;
  };
  
}

#endif

