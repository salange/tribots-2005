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


#ifndef TribotsTools_PaintPreferences_h
#define TribotsTools_PaintPreferences_h

#include "../../Fundamental/Vec.h"
#include "../../Fundamental/geometry.h"
#include <deque>


namespace TribotsTools {

  /** Struktur zum Zwischenspeichern alter Zoominformation */
  struct ZoomInfo {
    Tribots::Vec image_center;   ///< Mittelpunkt des gezeichneten Spielfeldes
    double scaling;     ///< Skalierungsfaktor
    int own_half;       ///< Orientierung links/rechts (aus -1,1)
  };


  /** Struktur, um Zeichenoptionen fuer FieldOfPlay zu verwalten */
  struct PaintPreferences {
    bool show_wm_robot;           ///< Roboter zeichnen?
    bool show_wm_ball;            ///< Ball zeichnen?
    bool show_wm_obstacles;       ///< Hindernisse einzeichnen?
    bool show_vis_lines;          ///< Liniensegmente einzeichnen?
    bool show_vis_ball;           ///< gesehenen Ball einzeichnen?
    bool show_vis_obstacles;      ///< gesehene Hindernisse einzeichnen?
    bool show_vis_goals;          ///< gesehene Tore einzeichnen?
    bool show_sl_pos;             ///< Suchpositionen der Selbstlokalisierung einzeichnen?
    bool show_aux_lines;          ///< Hilfslinien einzeichnen?
    
    bool show_robot_ids;          ///< Robotername einzeichnen?
    bool show_robot_ball_links;   ///< Roboter-Ball-Linie einzeichnen?

    bool show_robot_trace;        ///< Spur des Roboters anzeigen
    bool show_ball_trace;         ///< Spur des Balls anzeigen

    bool use_exec_time;           ///< true => Roboter- und Ballpositionen zur Ausfuehrungszeit, false => zur Kamerabildzeit

    ZoomInfo zoom;                ///< aktuell Bildschirmzoom und -lage
    int field_heading;            ///< Feld richtigrum (+1) oder umgekehrt (-1) aufzeichnen?

    unsigned int reference_robot; ///< der Roboter (unter u.U. mehreren, auf den sich bestimmte Aktionen (z.B. Versetzen beziehen)

    std::deque<ZoomInfo> old_zooms;             ///< alte Bildschirmzoom und -lage
    std::deque<ZoomInfo> redo_zooms;            ///< Bildausschnitte fuer Redo aufgehoben

    std::deque<Tribots::RobotLocation> rtrace;  ///< Spurinformation fuer Roboter
    std::deque<Tribots::BallLocation> btrace;   ///< Spurinformation fuer Ball

    std::deque<Tribots::LineSegment> mem;       ///< Markierungslinien
  };

  /** Struktur, um eine bestimmte Auswahl an Anzeigeoptionen festzulegen */
  struct PaintActionSelect {
    bool show_wm_robot;           ///< Roboter zeichnen?
    bool show_wm_ball;            ///< Ball zeichnen?
    bool show_wm_obstacles;       ///< Hindernisse einzeichnen?
    bool show_vis_lines;          ///< Liniensegmente einzeichnen?
    bool show_vis_ball;           ///< gesehenen Ball einzeichnen?
    bool show_vis_obstacles;      ///< gesehene Hindernisse einzeichnen?
    bool show_vis_goals;          ///< gesehene Tore einzeichnen?
    bool show_sl_pos;             ///< Suchpositionen der Selbstlokalisierung einzeichnen?
    bool show_aux_lines;          ///< Hilfslinien einzeichnen?
    bool show_robot_ids;          ///< Robotername einzeichnen?
    bool show_robot_ball_links;   ///< Roboter-Ball-Linie einzeichnen?
    bool show_robot_trace;        ///< Spur des Roboters anzeigen
    bool show_ball_trace;         ///< Spur des Balls anzeigen
    bool clear_lines;             ///< eingezeichnete Linien loeschen
    bool zoom_in;                 ///< vergroessern
    bool zoom_out;                ///< verkleinern
    bool zoom_all;                ///< ganzes Feld anzeigen
    bool zoom_undo;               ///< letzter Auschnitt
    bool zoom_redo;               ///< naechster Ausschnitt
    bool flip_sides;              ///< Seitenwechsel
    bool flip_goals;              ///< Spielfeld drehen
    bool use_exec_time;           ///< Zeitwauswahl Bild-/Ausfuehrungszeit
    bool next_refrobot;           ///< Referenzspieler weiterschalten
  };

}

#endif
