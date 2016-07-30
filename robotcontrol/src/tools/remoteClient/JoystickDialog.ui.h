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


void JoystickDialog::init()
{
  comboBoxJoyDev->setCurrentItem(0);
  groupBoxJoyDev->setPaletteBackgroundColor(QColor(0,255,0));
  
  JoyDev=comboBoxJoyDev->currentText();
  js=0;
  init_joystick();
  
  update_view();
}

bool JoystickDialog::init_joystick()
{
  if (js!=0)
    delete js;
  
  try { 
    js = new Tribots::Joystick(JoyDev);
  } 
  catch (std::invalid_argument)
    { 
      js=0;
      return false;
    }
  catch(std::bad_alloc)
    {
      js=0;
      return false;
    }
  
  return true;
}

void JoystickDialog::setJoyDev( const QString & s )
{
  JoyDev = s;
  init_joystick();
  update_view();
}

void JoystickDialog::update_view()
{
  if (js!=0)
    groupBoxJoyDev->setPaletteBackgroundColor(QColor(0,255,0));
  else
    groupBoxJoyDev->setPaletteBackgroundColor(QColor(255,0,0));
}

bool JoystickDialog::hasJoystick()
{
  return (js!=0);
}

void JoystickDialog::getDriveVectorFromJoystick( Tribots::DriveVector &drv )
{
  if (js!=0)
    {
      std::vector<double> axis;
      axis = js->get_axis_state();
      
      std::vector<bool> buttons;
      buttons = js->get_button_state();
      
      double act_x = axis[spinBoxXAxisIdx->value()];
      double act_y = axis[spinBoxYAxisIdx->value()];
      double act_phi = axis[spinBoxPhiAxisIdx->value()];

      double ignore_x = static_cast<double>(spinBoxXAxisNull->value())/100.0;
      double ignore_y = static_cast<double>(spinBoxYAxisNull->value())/100.0;
      double ignore_phi = static_cast<double>(spinBoxPhiAxisNull->value())/100.0;
      
      // ungenaue Nullstellung beruecksichtigen
      if (std::abs(act_x)<ignore_x) act_x=0.0;
      else if (act_x>0.0) act_x = (act_x-ignore_x)/(1.0-ignore_x);
      else act_x = (act_x+ignore_x)/(1.0-ignore_x);

      if (std::abs(act_y)<ignore_y) act_y=0.0;
      else if (act_y>0.0) act_y = (act_y-ignore_y)/(1.0-ignore_y);
      else act_y = (act_y+ignore_y)/(1.0-ignore_y);

      if (std::abs(act_phi)<ignore_phi) act_phi=0.0;
      else if (act_phi>0.0) act_phi = (act_phi-ignore_phi)/(1.0-ignore_phi);
      else act_phi = (act_phi+ignore_phi)/(1.0-ignore_phi);

      // Koppelung von x und y beruecksichtigen
      double act_x2 = act_x*act_x;
      double act_y2 = act_y*act_y;
      double len = act_x2+act_y2;
      if (act_x2>=act_y2) len = std::sqrt (len/act_x2);
      else len = std::sqrt (len/act_y2);
      if (act_x2+act_y2>0) {
	act_x/=len;
	act_y/=len;
      }
      
      int pt = (radioButtonLin->isChecked() ? 1 : 3);
      double scale_x = 1e-3*static_cast<double>(spinBoxXAxisMaxV->value())*(checkBoxXInvert->isChecked() ? -1 : 1);
      double scale_y = 1e-3*static_cast<double>(spinBoxYAxisMaxV->value())*(checkBoxYInvert->isChecked() ? -1 : 1);
      double scale_phi = 1e-1*static_cast<double>(spinBoxPhiAxisMaxV->value())*(checkBoxPhiInvert->isChecked() ? -1 : 1);

      drv.vtrans.x = scale_x*pow(act_x, pt);
      drv.vtrans.y = scale_y*pow(act_y, pt);
      drv.vrot = scale_phi*pow(act_phi, pt );

      spinBoxKickButtonIdx->setMinValue(0);
      spinBoxKickButtonIdx->setMaxValue(buttons.size()-1);
      drv.kick = buttons[spinBoxKickButtonIdx->value()];  
 
    }
} 
