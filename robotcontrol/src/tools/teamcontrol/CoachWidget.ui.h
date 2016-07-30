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


void CoachWidget::init() {
  coach = NULL;
}

void CoachWidget::destroy() {
  if (coach) delete coach;
}

void CoachWidget::init(const Tribots::ConfigReader& cfg) {
  std::vector<std::string> rids;
  cfg.get ("robots", rids);
  coach = new TribotsTools::Coach (cfg, rids);
  comm_ball_mode_check->setChecked (coach->get_comm_ball_mode());
  broadcast_mode_check->setChecked (coach->get_broadcast_mode());
  owns_ball_mode_check->setChecked (coach->get_owns_ball_mode());
  std::vector<std::string> policy_ids = coach->get_list_of_policies();
  for (unsigned int i=0; i<policy_ids.size(); i++)
    policy_combo->insertItem (policy_ids[i].c_str());
  std::vector<std::string> roles = coach->get_list_of_roles();
  if (coach->dynamic_role_change_mode ().length()==0)
      policy_combo->setCurrentText ("---");
    else
      policy_combo->setCurrentText (coach->dynamic_role_change_mode ());
  std::vector<std::string> all_ko_policies = coach->get_list_of_kick_off_procedures ();
}

void CoachWidget::update( TribotsTools::RemoteTeamState& ts) {
  if (coach) coach->update (ts);
  message_edit->setText ((coach->get_role_description ()).c_str());
}

void CoachWidget::comm_ball_mode_toggled(bool b) {
  if (coach) coach->set_comm_ball_mode (b);
}

void CoachWidget::broadcast_mode_toggled(bool b) {
  if (coach) coach->set_broadcast_mode (b);
}

void CoachWidget::owns_ball_mode_toggled(bool b) {
  if (coach) coach->set_owns_ball_mode (b);
}

void CoachWidget::policyChange(const QString& qs) {
  if (coach) {
    coach->dynamic_role_change_mode (qs.ascii());
    if (coach->dynamic_role_change_mode ().length()==0)
      policy_combo->setCurrentText ("---");
    else
      policy_combo->setCurrentText (coach->dynamic_role_change_mode ());
  }
}


