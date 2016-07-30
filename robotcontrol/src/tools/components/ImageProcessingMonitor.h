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

#ifndef _ImageProcessingMonitor_h_
#define _ImageProcessingMonitor_h_

#include "imagewidget.h"
#include "../../ImageProcessing/ScanLines.h"
#include "../../ImageProcessing/LineScanning.h"
#include "../../Fundamental/ConfigReader.h"


namespace Tribots {

  class ImageProcessingMonitor : public CImageWidget
  {
  Q_OBJECT

  public:
    ImageProcessingMonitor(const ConfigReader& config,
			   const Image& image, QWidget* parent = 0,
	   		   WFlags f = WType_TopLevel | WNoAutoErase);

    virtual ~ImageProcessingMonitor();
    
  public slots:
    void setScanLines(const ScanLines* scanLines);
    void setScanResults(const ScanResultList* scanResults);


  protected:
    void paintEvent(QPaintEvent *ev);

    const ScanLines* scanLines;
    const ScanResultList* scanResults;

    ColorClassInfoList* colClass;
  };			      
}

#endif
