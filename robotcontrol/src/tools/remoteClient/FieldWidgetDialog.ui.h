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

void FieldWidgetDialog::resizeEvent( QResizeEvent * ev )
{
    the_fieldWidget->resize(ev->size().width(), ev->size().height() - 40); 
}


void FieldWidgetDialog::init()
{
    connect(pushButtonSLHint , SIGNAL(pressed()), the_fieldWidget, SLOT( setSlHintMode() ) );
    //Durchschleifen der Signale zum setzen der SLHints
    connect(the_fieldWidget, SIGNAL(SigSetSLHint2( float, float)), SLOT(emit_SigSetSLHint2( float, float)));
    connect(the_fieldWidget, SIGNAL(SigSetSLHint3( float, float,float)), SLOT(emit_SigSetSLHint3( float, float,float)));
}



void FieldWidgetDialog::emit_SigSetSLHint3( float x_mm, float y_mm, float heading_rad )
{
    emit SigSetSLHint3( x_mm, y_mm, heading_rad );
}


void FieldWidgetDialog::emit_SigSetSLHint2( float x_mm, float y_mm )
{
    emit SigSetSLHint2( x_mm, y_mm ); 
}
