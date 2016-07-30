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


void RefboxWidget::init() {
  refstateChanged (Tribots::errorState);
  team_color=1;
  own_half=1;
  TribotsGoalColor->setBackgroundColor (Qt::yellow);
  TribotsGoalColor->setText ("gelb");
  OpponentGoalColor->setBackgroundColor (Qt::blue);
  OpponentGoalColor->setText ("blau");
  TribotsLabelColor->setBackgroundColor (Qt::cyan);
  TribotsLabelColor->setText ("cyan");
  OpponentLabelColor->setBackgroundColor (Qt::magenta);
  OpponentLabelColor->setText ("magenta");
}

void RefboxWidget::refstateChanged(Tribots::RefereeState rs) {
  refstate = rs;
  CurrentRefereeState->setText (Tribots::referee_state_names[rs]);
}

void RefboxWidget::labelChanged(int n) {
  team_color = n;
  if (n>0) {
    TribotsLabelColor->setBackgroundColor (Qt::cyan);
    TribotsLabelColor->setText ("cyan");
    OpponentLabelColor->setBackgroundColor (Qt::magenta);
    OpponentLabelColor->setText ("magenta");
  } else {
    TribotsLabelColor->setBackgroundColor (Qt::magenta);
    TribotsLabelColor->setText ("magenta");
    OpponentLabelColor->setBackgroundColor (Qt::cyan);
    OpponentLabelColor->setText ("cyan");
  }
}

void RefboxWidget::ownHalfChanged(int n) {
  own_half = n;
  if (n>0) {
    TribotsGoalColor->setBackgroundColor (Qt::yellow);
    TribotsGoalColor->setText ("gelb");
    OpponentGoalColor->setBackgroundColor (Qt::blue);
    OpponentGoalColor->setText ("blau");
  } else {
    TribotsGoalColor->setBackgroundColor (Qt::blue);
    TribotsGoalColor->setText ("blau");
    OpponentGoalColor->setBackgroundColor (Qt::yellow);
    OpponentGoalColor->setText ("gelb");
  }    
}

void RefboxWidget::changeOwnHalfPressed() {
  int n = (own_half>0 ? -1 : +1);
  emit (ownHalfSelect(n));
}

void RefboxWidget::changeLabelPressed() {
  int n (team_color>0 ? -1 : +1);
  emit (labelSelect(n));
}

void RefboxWidget::stopPressed() {
  emit (refboxSignal(Tribots::SIGstop));
}

void RefboxWidget::readyPressed() {
  emit (refboxSignal(Tribots::SIGready));
}

void RefboxWidget::startPressed() {
  emit (refboxSignal(Tribots::SIGstart));
}

void RefboxWidget::kickOffOwnPressed() {
  if (team_color>0)
    emit (refboxSignal(Tribots::SIGcyanKickOff));
  else
    emit (refboxSignal(Tribots::SIGmagentaKickOff));
}

void RefboxWidget::throwInOwnPressed() {
  if (team_color>0)
    emit (refboxSignal(Tribots::SIGcyanThrowIn));
  else
    emit (refboxSignal(Tribots::SIGmagentaThrowIn));
}

void RefboxWidget::goalKickOwnPressed() {
  if (team_color>0) 
    emit (refboxSignal(Tribots::SIGcyanGoalKick));
  else
    emit (refboxSignal(Tribots::SIGmagentaGoalKick));
}

void RefboxWidget::cornerKickOwnPressed() {
  if (team_color>0)
    emit (refboxSignal(Tribots::SIGcyanCornerKick));
  else
    emit (refboxSignal(Tribots::SIGmagentaCornerKick));
}

void RefboxWidget::freeKickOwnPressed() {
  if (team_color>0)
    emit (refboxSignal(Tribots::SIGcyanFreeKick));
  else
    emit (refboxSignal(Tribots::SIGmagentaFreeKick));
}

void RefboxWidget::penaltyKickOwnPressed() {
  if (team_color>0)
    emit (refboxSignal(Tribots::SIGcyanPenalty));
  else
    emit (refboxSignal(Tribots::SIGmagentaPenalty));
}

void RefboxWidget::kickOffOpponentPressed() {
  if (team_color<0)
    emit (refboxSignal(Tribots::SIGcyanKickOff));
  else
    emit (refboxSignal(Tribots::SIGmagentaKickOff));
}

void RefboxWidget::throwInOpponentPressed() {
  if (team_color<0)
    emit (refboxSignal(Tribots::SIGcyanThrowIn));
  else
    emit (refboxSignal(Tribots::SIGmagentaThrowIn));
}

void RefboxWidget::goalKickOpponentPressed() {
  if (team_color<0) 
    emit (refboxSignal(Tribots::SIGcyanGoalKick));
  else
    emit (refboxSignal(Tribots::SIGmagentaGoalKick));
}

void RefboxWidget::cornerKickOpponentPressed() {
  if (team_color<0)
    emit (refboxSignal(Tribots::SIGcyanCornerKick));
  else
    emit (refboxSignal(Tribots::SIGmagentaCornerKick));
}

void RefboxWidget::freeKickOpponentPressed() {
  if (team_color<0)
    emit (refboxSignal(Tribots::SIGcyanFreeKick));
  else
    emit (refboxSignal(Tribots::SIGmagentaFreeKick));
}

void RefboxWidget::penaltyKickOpponentPressed() {
  if (team_color<0)
    emit (refboxSignal(Tribots::SIGcyanPenalty));
  else
    emit (refboxSignal(Tribots::SIGmagentaPenalty));
}

