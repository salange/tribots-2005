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

#ifndef _imagewidget_h_
#define _imagewidget_h_

/*
 * Description: Qt widget, that holds and displays an image.
 * Created:     2004-05-10
 * Author:      Sascha Lange
 * Mail:        Sascha.Lange@Uni-Osnabrueck.De
 *
 * Copyright (C) 2001, 2004 Sascha Lange
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA.
 *
 * --------------------------------------------------------------------
 */

#include <qapplication.h>
#include <qimage.h>
#include "../../ImageProcessing/Image.h"

namespace Tribots {

  class CImageWidget : public QWidget
  {
  Q_OBJECT

  public:
    CImageWidget(QWidget* parent = 0, const char* name = "ImageWidget",
		 WFlags f=0);

    CImageWidget(const Image& image, QWidget* parent = 0, 
		 WFlags f = WType_TopLevel);
    virtual ~CImageWidget();

  signals:
    void mousePressed(QMouseEvent*);

  public slots:
    void setImage(const Image& image);

  protected:
    void paintEvent(QPaintEvent *ev);
    virtual void mousePressEvent(QMouseEvent*);

  protected:
    int w, h;
    unsigned int* buf;
    QImage image;
    QPixmap pixmap;
  };
}

#endif

