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


#ifndef TribotsTools_FieldOfPlay_h
#define TribotsTools_FieldOfPlay_h

#include <qwidget.h>
#include "CycleInfo.h"
#include "PaintPreferences.h"
#include "../../Structures/FieldGeometry.h"
#include <qaction.h>
#include <qtoolbar.h>
#include <qdockarea.h>

namespace TribotsTools {

  /** Widget, um ein Midsize-Spielfeld mit einem oder mehreren Robotern darzustellen.
      Das Widget kann in jede Umgebung eingebaut werden, es erzeugt keine Seiteneffekte.
      Die Information wird mittels CycleInfo-Strukturen im slot next_cycle uebertragen.
      Die Auswahl der darzustellenden Informationen wird mittels Struktur PaintPreferences festgelgt.
      Es koennen einer oder mehrere Roboter und Baelle dargestellt werden.
      Mit CTRL-linkeMaustaste kann der Roboter versetzt werden.
      Bei mehreren Robotern wird die Wirkung der Funktionen "Roboter versetzten", "Roboterspur" und
      "Ballspur" mittels set_reference_robot festgelgt.
      Den Robotern koennen mittels robot_names Identifikationsnamen/-nummern zugeordnet werden.
      Mit CTRL-rechteMaustaste kann ein rechteckiger Bildausschnitt ausgewaehlt, der nicht mit einer
      festen Funktionalitaet verbunden ist. Daher kann ein anderes Modul darauf reagieren, es
      wird ein Signal emitiert.
      Ebenso werden alle nicht verarbeiteten Tastendruecke als Signal nach ausen weitergegeben.
      Die beiden Toolbars des Widgets koennen in einer DoackArea innerhalb des Widgets verarbeitet
      werden, aber auch in einer DockArea eines umgebenden Hauptfensters. */
  class FieldOfPlay : public QWidget {
    Q_OBJECT

   public:
    /** Konstruktor; Achtung: vor Verwendung muss init() aufgerufen werden 
	Argumente wie normale Widgets */    
    FieldOfPlay ( QWidget* =0, const char* = 0, WFlags = 0 );
    /** Destruktor */
    ~FieldOfPlay ();

    /** Initialisierung 
	Arg1: das Hauptfenster der Anwendung
	Arg2: die Feldgeometrie
	Arg3: Angabe, ob die Toolbar selbst verwaltet werden sollen (default) oder vom Hauptfenster */
    void init (QMainWindow*, const Tribots::FieldGeometry&, Qt::Dock =Qt::DockUnmanaged);

    /** Initialisierung 
	Argumente wie oben
	Zusaetzliches Argument: die initialen PaintPreferences 
	und die Auswahl der Aktionsknoepfe, die erzeugt werden sollen */
    void init (QMainWindow*, const Tribots::FieldGeometry&, const PaintPreferences&, const PaintActionSelect&, Qt::Dock =Qt::DockUnmanaged);

    /** PaintPreferences einsehen */
    const PaintPreferences& get_preferences () const;

    /** dargestelltes CycleInfo */
    const CycleInfo& get_cycle_info () const;

    /** Referenzroboter aendern 
	Arg1: Nummer des Roboters (Nummerierung ab 0)
	Konvention: der Roboter Nr. 0 ist der erste in der CycleInfo::rloc, CycleInfo::bloc und robot_ids */
    void set_reference_robot (unsigned int);

    /** Liste mit Roboterbezeichnern setzen oder aendern */
    std::vector<std::string>& robot_names ();

    /** Liste mit Roboterbezeichnern einsehen */
    const std::vector<std::string>& robot_names () const ;
    

  signals:
    void slDisplacement ();                                 ///< Roboterlokalisierungs-Tipp
    void robotDisplacement (Tribots::Vec, Tribots::Angle);  ///< Anforderung fuer Roboter-Repositionierung
    void preferencesChanged ();                             ///< die PaintPreferences wurden veraendert
    void cycleChanged ();                                   ///< Zyklus weitergeschaltet
    void unresolvedKeyPressEvent ( QKeyEvent * );           ///< KeyPressEvent an tribotsview weiterleiten
    void unresolvedMouseRect (Tribots::Vec, Tribots::Vec);  ///< mit CTRL-rechteMaustaste wurde ein Fenster mit Eckpunkten arg1, arg2 in Weltkoordinaten ausgeawehlt

  public slots:  // Anzeigen bestimmter Informationen ein/ausschalten oder Bildausschnitt veraendern
    void next_cycle (const CycleInfo&);                     ///< naechster Zyklus

    void move_clipping (Tribots::Vec);
    void zoom_rect (Tribots::Vec, Tribots::Vec);
    void zoom_in ();
    void zoom_out ();
    void zoom_all ();
    void zoom_undo ();
    void zoom_redo ();
    void flip_side ();
    void flip_goals ();
    void show_wm_robot (bool);
    void show_wm_ball (bool);
    void show_wm_obstacles (bool);
    void show_vis_lines (bool);
    void show_vis_ball (bool);
    void show_vis_obstacles (bool);
    void show_vis_goals (bool);
    void show_sl_pos (bool);
    void show_robot_trace (bool);
    void toggle_robot_trace ();
    void show_ball_trace (bool);
    void show_robot_ids (bool);
    void show_robot_ball_links (bool);
    void show_aux_lines (bool);
    void clear_lines ();
    void use_exec_time (bool);
    void next_refrobot ();

  protected slots:
    void paintEvent(QPaintEvent *);
    void resizeEvent (QResizeEvent*);
    void keyPressEvent(QKeyEvent *);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void clippingChanged ();

    
  private:
    CycleInfo cinfo;
    PaintPreferences ppref;
    Tribots::FieldGeometry fgeometry;
    std::vector<std::string> robot_ids;

    int mouse_mode;
    Tribots::Vec p_click;

    unsigned int dock_height;

    QDockArea* dock_area;
    QToolBar* toolbar_zoom;
    QToolBar* toolbar_show;
    QAction* zoom_in_act;
    QAction* zoom_out_act;
    QAction* zoom_all_act;
    QAction* zoom_undo_act;
    QAction* zoom_redo_act;
    QAction* flip_sides_act;
    QAction* flip_goals_act;
    QAction* show_wm_robot_act;
    QAction* show_wm_ball_act;
    QAction* show_wm_obstacles_act;
    QAction* show_vis_lines_act;
    QAction* show_vis_ball_act;
    QAction* show_vis_obstacles_act;
    QAction* show_vis_goals_act;
    QAction* show_sl_pos_act;
    QAction* show_robot_trace_act;
    QAction* show_ball_trace_act;
    QAction* show_robot_ids_act;
    QAction* show_robot_ball_links_act;
    QAction* show_aux_lines_act;
    QAction* clear_lines_act;
    QAction* use_exec_time_act;
    QAction* next_refrobot_act;


  protected:
    /** Umrechnen von Widget-Koordinaten in Tribots-Feldkoordinaten */
    Tribots::Vec widget2field (Tribots::Vec) const;
    /** Umrechnen von Tribots-Feldkoordinaten in Widget-Koordinaten */
    Tribots::Vec field2widget (Tribots::Vec) const;
  };

}

#endif

