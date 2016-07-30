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

void tribotsview::init_field_and_streams( const char * info_praefix, const char * config_file )
{
  cycle_container = new TribotsTools::CycleContainer (info_praefix);
  cycle_info = cycle_container->get();
  Tribots::FieldGeometry* fgeometry = NULL;
  Tribots::ConfigReader reader;
  try{
    reader.append_from_file (config_file);
    fgeometry = new Tribots::FieldGeometry (reader);
  }catch(std::exception&){
    // keine Config-File Feldgeometrie-> versuche die Geometrie aus dem .log-File zu lesen
    fgeometry = new Tribots::FieldGeometry;
    std::deque<std::string> parts;
    Tribots::split_string (parts, cycle_info.logmsg);
    if (parts.size()>=14 && parts[0]=="FieldGeometry:") {
      Tribots::string2double (fgeometry->field_length, parts[1]);
      Tribots::string2double (fgeometry->field_width, parts[2]);
      Tribots::string2double (fgeometry->side_band_width, parts[3]);
      Tribots::string2double (fgeometry->goal_band_width, parts[4]);
      Tribots::string2double (fgeometry->goal_area_length, parts[5]);
      Tribots::string2double (fgeometry->goal_area_width, parts[6]);
      Tribots::string2double (fgeometry->penalty_area_length, parts[7]);
      Tribots::string2double (fgeometry->penalty_area_width, parts[8]);
      Tribots::string2double (fgeometry->center_circle_radius, parts[9]);
      Tribots::string2double (fgeometry->corner_arc_radius, parts[10]);
      Tribots::string2double (fgeometry->penalty_marker_distance, parts[11]);
      Tribots::string2double (fgeometry->line_thickness, parts[12]);
      Tribots::string2double (fgeometry->border_line_thickness, parts[13]);
      Tribots::string2double (fgeometry->goal_width, parts[14]);
      Tribots::string2double (fgeometry->goal_length, parts[15]);
      Tribots::string2double (fgeometry->goal_height, parts[16]);
      Tribots::string2double (fgeometry->pole_height, parts[17]);
      Tribots::string2double (fgeometry->pole_diameter, parts[18]);
      Tribots::string2double (fgeometry->pole_position_offset_x, parts[19]);
      Tribots::string2double (fgeometry->pole_position_offset_y, parts[20]);
      Tribots::string2double (fgeometry->ball_diameter, parts[21]);
      Tribots::string2uint (fgeometry->lineset_type, parts[22]);
    }
  }
  play_mode = 0;
  wait_msec = 100;
  
  field_of_play->init (this, *fgeometry, Qt::DockTop);
  rotate_view->init (field_of_play);

  slwidget = new TribotsTools::SLErrorWidget;
  slwidget->init_pointers (reader, *fgeometry);
  
  imageviewDialog = new imageview;
  
  connect (&play_control, SIGNAL(timeout()), this, SLOT(play_on()));
  connect (field_of_play, SIGNAL(unresolvedKeyPressEvent(QKeyEvent*)), this, SLOT(unresolvedKeyPressEvent(QKeyEvent*)));
  connect (field_of_play, SIGNAL(unresolvedMouseRect(Tribots::Vec, Tribots::Vec)), this, SLOT(showSLError(Tribots::Vec, Tribots::Vec)));
  connect (slwidget, SIGNAL(robot_update()),this,SLOT(sl_pos_changed()));

  cycleChanged();
  delete fgeometry;
}

void tribotsview::cycleChanged()
{
  if (cycle_container) {
    cycle_info = cycle_container->get();
    field_of_play->next_cycle (cycle_info);
    imageviewDialog->showImage(cycle_info.cycle_num);
  }
  
  displayChanged();
}

void tribotsview::sl_pos_changed()
{
  field_of_play->next_cycle (cycle_info);
  displayChanged();
}

void tribotsview::displayChanged()
{
  unsigned int reference_robot = field_of_play->get_preferences().reference_robot;
  QString s;
 
  // Info-Felder ausfuellen: Zyklusnummer, Programmzeit
  cycle_num->setText(s.setNum(cycle_info.cycle_num));
  prog_time->setText(s.setNum(cycle_info.time_msec));
  // Ballinformationsfelder ausfuellen
  const std::vector<Tribots::BallLocation>& bloc (field_of_play->get_preferences().use_exec_time ? cycle_info.bloc_exec : cycle_info.bloc_vis );
  if (bloc.size()>reference_robot) {
    ball_fuzzyness->setText(s.setNum(bloc[reference_robot].quality, 'f', 2));
    if (bloc[reference_robot].pos_known==Tribots::BallLocation::known)
      ball_known->setText("ja");
    else if (bloc[reference_robot].pos_known==Tribots::BallLocation::unknown)
      ball_known->setText("nein");
    else if (bloc[reference_robot].pos_known==Tribots::BallLocation::raised)
      ball_known->setText("hoch");
    else if (bloc[reference_robot].pos_known==Tribots::BallLocation::communicated)
      ball_known->setText("komm.");    
    ball_velocity->setText(s.setNum(bloc[reference_robot].velocity.length(),'f',2));
  } else {
    ball_velocity->setText(" ----- ");
    ball_known->setText(" ----- ");
    ball_fuzzyness->setText(" ----- ");
  }
  // Roboterinformationsfelder ausfuellen
  const std::vector<Tribots::RobotLocation>& rloc (field_of_play->get_preferences().use_exec_time ? cycle_info.rloc_exec : cycle_info.rloc_vis );
  QString s1, s2;
  if (rloc.size()>reference_robot) {
    s1.setNum (rloc[reference_robot].vtrans.length(),'f',2);
    s2.setNum (rloc[reference_robot].vrot,'f',2);
    robot_velocity->setText (s1+QString("; ")+s2);
    textfeld->setText (cycle_info.logmsg.c_str());
    stuck_led->setOn (rloc[reference_robot].stuck.robot_stuck);
  } else {
    textfeld->setText (" ----- ");
    stuck_led->setOn (false);
  }
  if (static_cast<double>(cycle_container->size())/static_cast<double>(cycle_slider->maxValue())>0.95)
    cycle_slider->setMaxValue(static_cast<int>(cycle_container->size()/0.95));
  cycle_slider->setValue (cycle_info.cycle_num);
  refereeState->setText(Tribots::referee_state_names [cycle_info.gs.refstate]);
  in_game_edit->setText(cycle_info.gs.in_game ? "Roboter aktiviert" : "Roboter deaktiviert");
  refereeState->setCursorPosition (0);
}


// Slots zum Vorwaerts- und Rueckwaertsschalten: ------------------------------------
void tribotsview::nextCycle()  // Einzelschritt vor
{
  play_mode=0;
  play_control.stop();
  if (cycle_container)
    cycle_container->step(1);
  cycleChanged();
}

void tribotsview::prevCycle()  // Einzelschritt zurueck
{
  play_mode=0;
  play_control.stop();
  if (cycle_container) 
    cycle_container->step(-1);
  cycleChanged();
}

void tribotsview::play_on()  // einen Schritt automatisch weiterschalten
{
  if (cycle_container) {
    long int cn = cycle_container->cycle_num();
    if (cn == cycle_container->step(play_mode)) 
      play_mode=0;  // am Anfang/Ende angekommen, stoppen
    else
      cycleChanged();
  }
}

void tribotsview::start_play() // vorwaerts abspielen
{
  play_mode=1;
  play_control.start(wait_msec);
}

void tribotsview::start_rew() // rueckwaerts abspielen
{
  play_mode=-1;
  play_control.start(wait_msec);  
}

void tribotsview::start_ffw()  // schnell vorwaerts
{
  play_mode=5;
  play_control.start(wait_msec);
}

void tribotsview::start_frew() // schnell rueckwaerts
{
  play_mode=-5;
  play_control.start(wait_msec);
}

void tribotsview::stop_play()  // anhalten
{
  play_control.stop();
  play_mode=0;
}

void tribotsview::setCycleNum()  // Iteration direkt setzen
{
  play_mode=0;
  play_control.stop();
  cycle_container->set (cycle_num->text().toUInt());
  cycleChanged();
}

void tribotsview::setTime() // Programmzeit direkt setzen
{
  play_mode=0;
  play_control.stop();
  cycle_container->set_time (prog_time->text().toUInt());
  cycleChanged();  
}

void tribotsview::goto_start() { // an Start springen
  cycle_container->set (0);
  cycleChanged();
}


void tribotsview::goto_end() { // an Ende springen
  cycle_container->set (1000000);
  cycleChanged();
}

// sonstige Slots ------------------------------------------------
void tribotsview::toogleImageView( bool b )
{
  if (b) {
    imageviewDialog->show();
    imageviewDialog->showImage( cycle_info.cycle_num );
  }
  else {
    imageviewDialog->hide();
  }
}

void tribotsview::change_display_frequency( int v )
{
  int g [] = { 800, 400, 200, 150, 100, 70, 50, 40, 30 };
  wait_msec = g[v-1];
  play_control.changeInterval (wait_msec);
}

void tribotsview::cycle_slider_moved(int i)
{
  play_mode=0;
  play_control.stop();
  cycle_container->set (i);
  cycleChanged();
}

void tribotsview::cycle_slider_value_changed( int i )
{
  if (i!=static_cast<int>(cycle_info.cycle_num))
    cycle_slider_moved (i);
}

void tribotsview::fileExit()
{
  qApp->quit();
}

void tribotsview::revert_file()
{
  play_mode=0;
  play_control.stop();
  if (!cycle_container->revert())
    QMessageBox::warning (this, "tribotsview", "Fehler beim Dateizugriff\nBehalte alte Datei bei", QMessageBox::Ok,  QMessageBox::NoButton);
  cycleChanged();
}

void tribotsview::loadImages()
{
  play_mode=0;
  play_control.stop();
  QString filterstring = "*.log";
  QString filename = QFileDialog::getOpenFileName(QString::null, filterstring, this);
  if ( ! filename.isEmpty() ) {
    imageviewDialog->loadImages(filename, cycle_info.cycle_num);
  }
}

void tribotsview::reload_file()
{
  play_mode=0;
  play_control.stop();
  QString filterstring= "*.log";
  QString fn = QFileDialog::getOpenFileName( QString::null, filterstring, this);
  if ( !fn.isEmpty() ) {
    std::string pp (fn.latin1());
    std::string ppp = pp.substr (0,pp.length()-4);
    if (!cycle_container->revert(ppp.c_str()))
      QMessageBox::warning (this, "tribotsview", "Fehler beim Dateizugriff\nBehalte alte Datei bei", QMessageBox::Ok,  QMessageBox::NoButton);
  }    
  cycleChanged();
}

void tribotsview::showSLError( Tribots::Vec p1, Tribots::Vec p2 )
{
  slwidget->update_error (cycle_info, p1, p2, field_of_play->get_preferences().zoom.own_half);
  slwidget->show();
}

void tribotsview::replaceCycleInfo()
{
  cycle_container->replace (field_of_play->get_cycle_info ());
}

void tribotsview::keyPressEvent( QKeyEvent * ev)
{
  unresolvedKeyPressEvent (ev);
}

void tribotsview::unresolvedKeyPressEvent( QKeyEvent * event )
{
  switch (event->key()) {
  case Key_N:
    nextCycle ();
    break;
  case Key_P:
    prevCycle ();
    break;
  case Key_G:
    start_play ();
    break;
  case Key_B:
    start_rew ();
    break;
  case Key_Space:
    stop_play ();
    break;
  case Key_F:
    start_ffw ();
    break;
  case Key_R:
    start_frew ();
    break;
  }
}

