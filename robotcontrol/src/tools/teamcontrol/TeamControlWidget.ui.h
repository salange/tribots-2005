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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void TeamControlWidget::init() {  // zunaechst muss init(ConfigReader&) aufgerufen werden
  field_widget=NULL;
  refbox_client=NULL;
  refbox_widget=NULL;
  joystick_widget=NULL;
  rrd_widget=NULL;
  coach_widget=NULL;
  refstate_machine=NULL;
  cycle_timer=NULL;
  robots.clear();
  dir_of_play.own_half=1;
  dir_of_play.team_color=1;
  refrobot=0;
  cyclerobot=0;
}

void TeamControlWidget::destroy() {
  if (field_widget) delete field_widget;
  if (refbox_client) delete refbox_client;
  if (refbox_widget) delete refbox_widget;
  if (joystick_widget) delete joystick_widget;
  if (coach_widget) delete coach_widget;
  if (rrd_widget) delete rrd_widget;
  if (refstate_machine) delete refstate_machine;
  if (cycle_timer) delete cycle_timer;
  for (unsigned int i=0; i<robots.size(); i++)
    delete robots[i];
}

void TeamControlWidget::init(const Tribots::ConfigReader& cfg) {
  Tribots::FieldGeometry fg (cfg);
  fieldgeometry = fg;

  // RefboxClient:
  refbox_client = new TribotsTools::RefboxClient ("refbox_client.log");
  std::string text;
  if (cfg.get ("refbox_IP", text))
    refbox_ip_edit->setText(text.c_str());
  int pt;
  if (cfg.get("refbox_port", pt))
    refbox_port = pt;
  else 
    refbox_port = 28097;
  
  // RefstateMachine:
  refstate_machine = new Tribots::RefereeStateMachine (fieldgeometry);
  
  // RefboxWidget:
  refbox_widget  = new RefboxWidget;
  refbox_widget->refstateChanged (Tribots::errorState);
  int own_half=1;
  if (cfg.get ("own_half", text)) {
    if (text=="yellow")
      own_half=1;
    else if (text=="blue")
      own_half=-1;
  }
  refbox_widget->ownHalfChanged (own_half);
  int team_color=1;
  if (cfg.get ("team_color", text)) {
    if (text=="cyan")
      team_color=1;
    else
      team_color=-1;
  }
  connect (fensterRefereeStatesAction, SIGNAL(activated()), refbox_widget, SLOT(show()));
  connect (refbox_widget, SIGNAL(refboxSignal(Tribots::RefboxSignal)), this, SLOT(refboxSignal(Tribots::RefboxSignal)));
  connect (refbox_widget, SIGNAL(ownHalfSelect(int)), this, SLOT(ownHalfChanged(int)));
  connect (refbox_widget, SIGNAL(labelSelect(int)), this, SLOT(teamColorChanged(int)));
  refbox_widget->show();

  // JoystickWidget:
  joystick_widget = new JoystickDialog;
  connect (fensterJoystickAction, SIGNAL(activated()), joystick_widget, SLOT(show()));

  // RealRobotDataView:
  rrd_widget = new RealRobotDataView;
  connect (fensterRoboterdatenAction, SIGNAL(activated()), rrd_widget, SLOT(show()));
  
  // RoboterWidgets anlegen:
  std::vector<std::string> words;
  std::vector<std::string> robot_names;
  cfg.get ("robots", words);
  if (words.size()==0)
    throw Tribots::InvalidConfigurationException ("robots");
  for (unsigned int i=0; i<words.size(); i++) {
    RemoteRobotWidget* rw = new RemoteRobotWidget;
    rw->init_robot (cfg, fieldgeometry, words[i].c_str());
    rw->direction_of_play_changed (dir_of_play);
    robots.push_back (rw);
    robot_names.push_back (rw->get_state ().name);
    QAction* ra = new QAction (NULL, "", false);
    ra->setText (rw->get_state ().name.c_str());
    ra->addTo (Roboter);
    connect (ra, SIGNAL(activated()), rw, SLOT(show()));
    bool b;
    if (cfg.get ((words[i]+string("::widget")).c_str(), b) && b)
      rw->show();
  }
  
  // Feldwidget anlegen:
  field_widget = new TribotsTools::FieldOfPlay;
  TribotsTools::PaintPreferences ppref;
  TribotsTools::PaintActionSelect psel;
  
  ppref.show_wm_robot=ppref.show_wm_ball=ppref.show_robot_ids=ppref.show_robot_ball_links=true;
  ppref.show_wm_obstacles=ppref.show_vis_lines=ppref.show_vis_ball=ppref.show_vis_obstacles=false;
  ppref.show_vis_goals=ppref.show_sl_pos=ppref.show_aux_lines=false;
  ppref.show_robot_trace=ppref.show_ball_trace=false;
  ppref.use_exec_time=false;
  ppref.reference_robot=0;
  ppref.zoom.scaling=ppref.zoom.own_half=ppref.field_heading=1;

  psel.show_wm_robot=psel.show_wm_ball=psel.show_robot_ids=psel.show_robot_ball_links=true;
  psel.show_wm_obstacles=true;
  psel.show_vis_lines=psel.show_vis_ball=psel.show_vis_obstacles= psel.show_vis_goals=true;
  psel.show_sl_pos=psel.show_aux_lines=false;
  psel.show_robot_trace=psel.show_ball_trace=true;
  psel.use_exec_time=false;
  psel.clear_lines=true;
  psel.zoom_in=psel.zoom_out=psel.zoom_all=psel.zoom_undo=psel.zoom_redo=psel.flip_sides=true;
  psel.flip_goals=false;
  psel.next_refrobot=true;

  field_widget->init (this, fg, ppref, psel);
  field_widget->robot_names () = robot_names;
  connect (fensterSpielfeldAction, SIGNAL(activated()), field_widget, SLOT(show()));
  connect (field_widget, SIGNAL(slDisplacement()), this, SLOT(slDisplacement()));
  connect (field_widget, SIGNAL(robotDisplacement(Tribots::Vec, Tribots::Angle)), this, SLOT(robotDisplacement(Tribots::Vec, Tribots::Angle)));
  field_widget->setCaption ("Spielfeld");
  field_widget->show();
  
  // der Coach:
  coach_widget = new CoachWidget;
  coach_widget->init (cfg);
  connect (fensterTrainerAction, SIGNAL(activated()), coach_widget, SLOT(show()));
  coach_widget->show();

  // TeamcontrolWidget selbst:
  for (unsigned int i=0; i<robot_names.size(); i++) {
    QString name = robot_names[i].c_str();
    reference_select->insertItem (name);
  }
  reference_select->setCurrentItem (0);
  cycle_timer = new QTimer;
  cycle_timer->start (comm_rate_spin->value());
  connect (cycle_timer, SIGNAL(timeout()), this, SLOT(cycle_task()));
  connect (connect_refbox_check, SIGNAL(toggled(bool)), this, SLOT(refboxConnectionChanged(bool)));
  connect (comm_rate_spin, SIGNAL(valueChanged(int)), this, SLOT(cycleRateChanged(int)));
}

void TeamControlWidget::cycle_task() {
  unsigned int old_cyclerobot=cyclerobot;
  cyclerobot= (cyclerobot+1)%robots.size();
  
  // Spannungsabfrage:
  if (!rrd_widget->isVisible() || old_cyclerobot!=refrobot)
    robots[old_cyclerobot]->robot_data (false);
  robots[cyclerobot]->robot_data (true);

  // Refbox abfragen
  if (refbox_client) {
    Tribots::RefboxSignal sig = refbox_client->listen();
    refboxSignal (sig);
    if (refbox_client->is_okay())
      refbox_ip_edit->setBackgroundColor(Qt::green);
    else if (refbox_client->is_connected())
      refbox_ip_edit->setBackgroundColor(Qt::blue);
    else
      refbox_ip_edit->setBackgroundColor(Qt::red);
  }

  // Joystick abfragen:
  if (joystick_check->isChecked() && joystick_widget->hasJoystick()) {
    Tribots::DriveVector dv;
    joystick_widget->getDriveVectorFromJoystick(dv);
    robots[refrobot]->joystick (dv);
    //zusätzliche TASTEN:
   std::vector<bool> buttons;
   buttons = joystick_widget->js->get_button_state();
   if (buttons[4]==true&&buttons[5]==true) 
     robots[refrobot]->refboxSignal (Tribots::SIGstop);
     
      
   if (buttons[6]==true&&buttons[7]) 
   robots[refrobot]->refboxSignal (Tribots::SIGstart);
     
  }

  // Message Board ausgeben:
  if (messageboard_check->isChecked()) {
    std::cout << "MessageBoard Ausgang:\n";
    for (unsigned int i=0; i<robots[refrobot]->get_message_board().get_outgoing().size(); i++)
      std::cout << robots[refrobot]->get_message_board().get_outgoing()[i] << '\n';
  }

/*
//FIXME, zuviele Daten mit höchster prio die versendet werden?
  // Not-Start und Not-Stop, falls Refbox-Signale im WLAN verloren gehen
  for (unsigned int i=0; i<robots.size(); i++) {
    if (refstate_machine->get_state()==Tribots::stopRobot)
      robots[i]->refboxSignal (Tribots::SIGstop);
    if (refstate_machine->get_state()==Tribots::freePlay)
      robots[i]->refboxSignal (Tribots::SIGstart);
  }
*/

  for (unsigned int i=0; i<robots.size(); i++)
    robots[i]->communicate ();

  // Hindernisdaten und gesehene Objekte aktualisieren:
  const TribotsTools::PaintPreferences& ppref (field_widget->get_preferences());
  bool show_obstacles = ppref.show_wm_obstacles;
  bool show_vis_objects = ppref.show_vis_obstacles || ppref.show_vis_lines || ppref.show_vis_ball || ppref.show_vis_goals;
  for (unsigned int i=0; i<robots.size(); i++) {
    robots[i]->obstacle_data (show_obstacles && i==ppref.reference_robot);
    robots[i]->visible_object_request (show_vis_objects && i==ppref.reference_robot);
  }

  // Feldwidget aktualisieren
  TribotsTools::CycleInfo ci;
  ci.rloc_vis.resize(robots.size());
  ci.bloc_vis.resize(robots.size());
  for (unsigned int i=0; i<robots.size(); i++) {
    const TribotsTools::RemoteRobotState& rrs (robots[i]->get_state());
    if (rrs.comm_okay) {
      ci.rloc_vis[i]=rrs.robot_pos;
      ci.bloc_vis[i]=rrs.ball_pos;
    } else {
      ci.rloc_vis[i].pos=Tribots::Vec (100000,100000);
      ci.rloc_vis[i].vtrans=Tribots::Vec(0,0);
      ci.rloc_vis[i].stuck.robot_stuck=false;
      ci.bloc_vis[i].pos=Tribots::Vec (100000,100000);
      ci.bloc_vis[i].velocity=Tribots::Vec(0,0);
      ci.bloc_vis[i].pos_known = Tribots::BallLocation::unknown;
    }
  }
  ci.oloc = robots[ppref.reference_robot]->get_state().obstacles;
  ci.vloc = robots[ppref.reference_robot]->get_state().visible_objects;

  field_widget->next_cycle (ci);

  // Roboterdaten aktualisieren:
  if (rrd_widget->isVisible()) {
    robots[refrobot]->robot_data (true);
    rrd_widget->update (robots[refrobot]->get_state ().robot_data);
  } else {
    robots[refrobot]->robot_data (false);
  }

  // Trainer befragen:
  TribotsTools::RemoteTeamState teamstate (robots.size());
  for (unsigned int i=0; i<robots.size(); i++) {
    teamstate.robot_states[i] = &robots[i]->get_state();
    teamstate.message_boards[i] = &robots[i]->get_message_board();
  }
  if (refstate_machine)
    teamstate.refstate = refstate_machine->get_state();
  if (coach_widget)
    coach_widget->update (teamstate);

  // Feldgeometrie der Spieler gegenchecken:
  for (unsigned int i=0; i<robots.size(); i++) {
    if (robots[i]->get_state().comm_okay)
      if (robots[i]->get_state().fieldgeometry!=fieldgeometry) 
	std::cerr << "ACHTUNG: Feldgeometrie von " << robots[i]->get_state().name << " und Teamcontrol passen nicht zusammen!\n";
  }

  // Message Board ausgeben:
  if (messageboard_check->isChecked()) {
    std::cout << "MessageBoard Eingang:\n";
    for (unsigned int i=0; i<robots[refrobot]->get_message_board().get_incoming().size(); i++)
      std::cout << robots[refrobot]->get_message_board().get_incoming()[i] << '\n';
  }
}

void TeamControlWidget::refboxSignal(Tribots::RefboxSignal sig) {
  refstate_machine->update (sig, Tribots::Vec::zero_vector, false);
  refstate_machine->update (Tribots::Vec::zero_vector, Tribots::Vec::zero_vector, false);
  refbox_widget->refstateChanged(refstate_machine->get_state());
  if (sig!=Tribots::SIGnop) {
    for (unsigned int i=0; i<robots.size(); i++)
      robots[i]->refboxSignal (sig);
  } 
}

void TeamControlWidget::connectRefboxClient() {
  refbox_client->connect (refbox_ip_edit->text().ascii(), refbox_port);
}

void TeamControlWidget::disconnectRefboxClient() {
  refbox_client->disconnect();
}

void TeamControlWidget::ownHalfChanged(int n) {
  refbox_widget->ownHalfChanged(n);
  dir_of_play.own_half=n;
  for (unsigned int i=0; i<robots.size(); i++)
    robots[i]->direction_of_play_changed (dir_of_play);
}

void TeamControlWidget::teamColorChanged(int n) {
  refbox_widget->labelChanged(n);
  dir_of_play.team_color=n;
  refstate_machine->set_team_color (n);
  for (unsigned int i=0; i<robots.size(); i++)
    robots[i]->direction_of_play_changed (dir_of_play);
}


void TeamControlWidget::reference_robot_changed( int n ) {
  robots[refrobot]->no_joystick();
  robots[refrobot]->robot_data (false);
  refrobot=n;
}

void TeamControlWidget::slDisplacement() {
  const TribotsTools::CycleInfo& ci = field_widget->get_cycle_info ();
  unsigned int ref = field_widget->get_preferences().reference_robot;
  robots[ref]->slDisplacement (ci.rloc_vis[ref].pos, ci.rloc_vis[ref].heading);
}

void TeamControlWidget::joycontrol() {
  if (!joystick_check->isChecked()) {
    robots[refrobot]->no_joystick();
  }
}

void TeamControlWidget::refboxConnectionChanged( bool b ) {
  if (b) {
    connectRefboxClient();
  } else
    disconnectRefboxClient();
}

void TeamControlWidget::cycleRateChanged(int n) {
  cycle_timer->changeInterval (n);
}

void TeamControlWidget::robotDisplacement(Tribots::Vec p, Tribots::Angle h) {
  unsigned int ref = field_widget->get_preferences().reference_robot;
  robots[ref]->get_message_board().publish_stream () << "GotoPos: " << static_cast<int>(p.x) << ' ' << static_cast<int>(p.y) << ' ' << static_cast<int>(h.get_deg()) << '\n';
}

