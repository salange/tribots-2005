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
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void Form1::init()
{
    
    Tribots::Journal::the_journal.set_stream_mode(std::cout);
    Tribots::Journal::the_journal.set_verbosity (3);
    
    
    
    gameState.refstate=Tribots::stopRobot;
    gameState.in_game=false;
    ownHalf=0;
    comEstablished = false;
    comInitiated      = false;
    the_communication = 0;
    port = 6012;
    comCyclusTime = 200;
    sprintf(playerType, "---");
    
    for (int i=0; i< Tribots::num_referee_states; i++)
 comboBoxGameStates->insertItem(Tribots::referee_state_names[i]);
    comboBoxGameStates->insertItem("Unknown");
    
    comboBoxOwnHalf->insertItem("Blau");
    comboBoxOwnHalf->insertItem("Gelb");
    comboBoxOwnHalf->insertItem("UNBEKANNT");
    
    comboBoxPlayer->insertItem("---");
    
    comboBoxIP->insertItem("DISCONNECT");
    comboBoxIP->insertItem("127.0.0.1");
    comboBoxIP->insertItem("10.0.0.1");
    comboBoxIP->insertItem("neuro14");
    comboBoxIP->insertItem("localhost");
    comboBoxIP->insertItem("");
    groupBox3->setPaletteBackgroundColor(QColor(255,0,0));
    
    QStringList header;
    header   << "Max Vel" 
     << "Max Acc" 
     << "Max Deacc"
     << "Max Rot Vel"
     << "Max Rot Acc"
     << "Max Rot Deacc"
     << "Max Robot Radius"
     << "Min Robot Radius";
    tableRP->setRowLabels(header);
    
    
    comTimer = new QTimer(this); 
    
    connect( comTimer, SIGNAL(timeout()), SLOT(comm()) );
 
    
    checkBoxRemoteControl-> setChecked (false); 
   
    
    fwd = new FieldWidgetDialog(this, "FieldWidgetDialog", FALSE);
    fwd->the_fieldWidget->hide();
    fwd->the_fieldWidget->init_widget(&fieldGeometry, 
          &ownHalf, 
          &ballLocation, 
          &robotLocation, 
          &robotProperties);
    fwd->show();
    
    connect(fwd, SIGNAL(SigSetSLHint2(float,float)), SLOT(sendSLHint2(float,float)));
    connect(fwd, SIGNAL(SigSetSLHint3(float,float,float)), SLOT(sendSLHint3(float,float,float)));
    
    jsd = new JoystickDialog(this, "JoystickDialog", FALSE);
    rdv = new RealRobotDataView(this, "RobotData", FALSE);
    update_view();
} 


void Form1::destroy()
{
    if (the_communication!=0) delete the_communication;
}


void Form1::comm()
{ 
  bool u_form=false;
  
    if (comInitiated)
    {
 if (comEstablished)
 // do normal communication
 {
     the_communication->clear_to_send();
     the_communication->clearRequests();
     the_communication->setGameStateRequest();
     the_communication->setOwnHalfRequest();
     the_communication->setBallLocationRequest();
     the_communication->setRobotLocationRequest();
     the_communication->setPlayerTypeRequest();
     the_communication->setRobotDataRequest();
     the_communication->putRequests();
 }
 else
     // communication was interrrupted, so ask some more information
 {
     the_communication->clear_to_send();
     
     the_communication->clearRequests();
     the_communication->setGameStateRequest();
     the_communication->setOwnHalfRequest();
     the_communication->setFieldGeometryRequest();
     the_communication->setRobotPropertiesRequest();
     the_communication->setPlayerTypeRequest();
     the_communication->setRobotDataRequest(); 
     the_communication->putRequests();
     the_communication->setPlayerTypeListRequest();
 }
 
 if (jsd->hasJoystick() && checkBoxRemoteControl->isChecked ())
 {
     Tribots::DriveVector remoteDrv;
     jsd->getDriveVectorFromJoystick(remoteDrv);
     the_communication->putRemoteCtr(remoteDrv);
 }
    
 the_communication->send();
    
 if (comEstablished=the_communication->receive_all())
 {
     Tribots::GameState gs;
     if (the_communication->getGameState(gs)) gameState=gs;
     else gameState.refstate = Tribots::errorState;
    
     int oh;
     if (the_communication->getOwnHalf(oh)) ownHalf = oh;
     else ownHalf = 0;
     
    u_form |= the_communication->getPlayerTypeList(ptl);
     
     the_communication->getFieldGeometry(fieldGeometry);
     the_communication->getBallLocation(ballLocation);
     the_communication->getRobotProperties(robotProperties);
     the_communication->getRobotLocation(robotLocation);
     the_communication->getPlayerType(playerType);
     the_communication->getRobotData(rd);
 }
 else
 {
     gameState.refstate = Tribots::errorState;
     ownHalf     =  0;
 }
    }
    if (u_form) update_form();
    update_view();
}



void Form1::update_view()
{    
    if (gameState.refstate >= 0 && gameState.refstate < Tribots::num_referee_states)
 comboBoxGameStates->setCurrentItem(gameState.refstate); 
    else 
 comboBoxGameStates->setCurrentItem(Tribots::num_referee_states);
    
    checkBoxInGame->setChecked(gameState.in_game);

    if (ownHalf <0)
 comboBoxOwnHalf->setCurrentItem(0);
    else if (ownHalf >0)
 comboBoxOwnHalf->setCurrentItem(1);
    else  comboBoxOwnHalf->setCurrentItem(2);
    
     comboBoxPlayer->setCurrentText(QString(playerType));
     
     rdv->update(rd);
 
     // Zurücksetzen der Menüs wenn Fenster manuell geschlossen wurden
    fensterSpielfeld_VisualisierungAction->setOn(fwd->isVisible());
    fensterJoystick_DialogAction->setOn(jsd->isVisible());
    fensterRobot_DataAction->setOn(rdv->isVisible());
    
     if (!jsd->hasJoystick()) 
    {
 checkBoxRemoteControl->setDisabled(true);
 checkBoxRemoteControl->setChecked(false);
    }
    else
 checkBoxRemoteControl->setDisabled(false);
    
    // update robot properties
    tableRP->setText ( 0, 0, QString::number(robotProperties.max_velocity,'f',2));
    tableRP->setText ( 0, 1, QString::number(robotProperties.max_acceleration,'f',2));
    tableRP->setText ( 0, 2, QString::number(robotProperties.max_deceleration,'f',2));
    tableRP->setText ( 0, 3, QString::number(robotProperties.max_rotational_velocity,'f',2));
    tableRP->setText ( 0, 4, QString::number(robotProperties.max_rotational_acceleration,'f',2));
    tableRP->setText ( 0, 5, QString::number(robotProperties.max_rotational_deceleration,'f',2));
    tableRP->setText ( 0, 6, QString::number(robotProperties.max_robot_radius,'f',2));
    tableRP->setText ( 0, 7, QString::number(robotProperties.min_robot_radius,'f',2));
    tableRP->repaint();
    
    groupBox1->setEnabled(comInitiated);
    groupBox2->setEnabled(comInitiated);
    groupBoxPort->setEnabled(!comInitiated);
    
    if (comInitiated)
    { 
 if (comEstablished)
     groupBox3->setPaletteBackgroundColor(QColor(0,255,0));
 else
     groupBox3->setPaletteBackgroundColor(QColor(0,0,255));
    }
    else 
    {
 groupBox3->setPaletteBackgroundColor(QColor(255,0,0));
    }
    
    
    if (comInitiated && comEstablished)
    { 
 fwd->the_fieldWidget->show();
 fwd->the_fieldWidget->repaint();
    }
    else
    {
 fwd->the_fieldWidget->hide();
    }
}

void Form1::sendNewGameState(int _gs)
{
    std::cout << "Wantto send gs: " << _gs << "\n";
    if (0<=_gs && _gs<Tribots::num_referee_states)
    {
      Tribots::GameState gs2 (gameState);
      gs2.refstate = Tribots::RefereeState (_gs);
 the_communication->clear_to_send(); 
 the_communication->putGameState(gs2);
 the_communication->send();
    }
}

void Form1::setNewGameStateGO()
{
  sendNewGameState(Tribots::freePlay);
}

void Form1::setNewGameStateStop()
{
  sendNewGameState(Tribots::stopRobot);
}

void Form1::send_in_game()
{
  Tribots::GameState gs2 (gameState);
  gs2.in_game = checkBoxInGame->isChecked();
 the_communication->clear_to_send(); 
 the_communication->putGameState(gs2);
 the_communication->send();  
}


void Form1::sendNewOwnHalf( int _oh )
{
    int tmp_oh;
    if (_oh==0) tmp_oh=-1;
    else if (_oh==1) tmp_oh=1;
    else (tmp_oh=0);
    
    the_communication->clear_to_send(); 
    the_communication->putOwnHalf(tmp_oh);
    the_communication->send();
}


void Form1::initiateCom( const QString &ip )
{
    if (ip == "DISCONNECT")
 stopCom();
    else
    {
    
 if (the_communication!=0)
     delete the_communication;
    
 the_communication = new Tribots::TribotsUdpCommunication (false,  
          port,
          ip);
    
 if (the_communication!=0) 
 {
     comInitiated = true;
     comTimer->start(comCyclusTime);
 }
    }   
}




void Form1::stopCom()
{
    if (the_communication!=0)
    {
 delete the_communication;
 the_communication=0;
    }
    comInitiated = false;
    comEstablished = false;
    comTimer->stop();
    update_view();
}


void Form1::portChanged( int _port)
{
    port = _port;
}


void Form1::changeComCyclusTime( int ms )
{
    comCyclusTime = ms;
    if (comInitiated)
    {
 comTimer->stop();
 comTimer->start(comCyclusTime);
    }
}


void Form1::showFieldVis( bool s )
{
    if (s)
 fwd->show();
    else 
 fwd->hide();
}




void Form1::showJoyDiag( bool s )
{
    if (s)
 jsd->show();
    else 
 jsd->hide();
}


void Form1::sendNewPlayerType( const QString & pt )
{
    std::cout << "Want to send new Player Type : " << pt << "\n";
    the_communication->clear_to_send(); 
    the_communication->putPlayerType(pt);
    the_communication->send();
}


void Form1::showRobotDataVis( bool s )
{
    if (s)
 rdv->show();
    else 
 rdv->hide();
}


void Form1::sendSLHint3( float x_mm, float y_mm, float heading_rad )
{
    the_communication->clear_to_send(); 
    the_communication->putSLHint(x_mm, y_mm, heading_rad);
    the_communication->send();
}

void Form1::sendSLHint2( float x_mm, float y_mm )
{
    the_communication->clear_to_send(); 
    the_communication->putSLHint(x_mm, y_mm);
    the_communication->send();
}






void Form1::update_form()
{
    comboBoxPlayer->clear();
    for (unsigned int i=0; i<ptl.size(); i++)
      comboBoxPlayer->insertItem(ptl[i]);
    comboBoxPlayer->insertItem("---");
}
