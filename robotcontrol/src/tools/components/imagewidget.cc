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

/*
 * Description: Implementation of the ImageWidget.
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

#include "imagewidget.h"
#include <qpainter.h>
#include <qimage.h>
#include "../../ImageProcessing/Images/RGBImage.h"

Tribots::CImageWidget::CImageWidget(QWidget* parent,
				    const char * name,
				    WFlags f)
  : QWidget(parent, name, f), w(-1), h(-1), buf(0)
{
  RGBImage image(640,480);  // black image

  this->setMinimumSize(40, 40);
  this->setBackgroundMode(NoBackground);
  setImage(image);
}

Tribots::CImageWidget::CImageWidget(const Image& image, 
				    QWidget* parent,
				    WFlags f)
  : QWidget(parent, "ImageWidget", f), w(-1), h(-1), buf(0)
{
  this->setMinimumSize(60, 40);
  this->setBackgroundMode(NoBackground);
  setImage(image);
}

Tribots::CImageWidget::~CImageWidget()
{
  free(buf);
}
void Tribots::CImageWidget::mousePressEvent(QMouseEvent* ev)
{
  emit mousePressed(ev);
}
void Tribots::CImageWidget::paintEvent(QPaintEvent*)
{
  QPainter p(this);
  p.drawPixmap(0,0,image);
}

void Tribots::CImageWidget::setImage(const Image& image)
{
  bool resized = false;

  if (image.getWidth() != w || image.getHeight() != h) {
    w=image.getWidth();
    h=image.getHeight();

    this->image.create(w,h, 32);
    resized = true;
  }
  unsigned char* dst = this->image.scanLine(0);
  RGBTuple rgb;
  QRgb* ptr;
  
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      image.getPixelRGB(x,y, &rgb);
      ptr = reinterpret_cast<QRgb*>(dst);
      *ptr = qRgb (rgb.r, rgb.g, rgb.b);
      dst+=4;
    }
  }  

  
  
  if (resized) {
    resize(w, h);
    setMaximumSize(w, h);
  }
  else {
    repaint();            // unfortunately update's to slow!
  }
}








