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

#include <iostream>

#include "../../tools/components/ImageProcessingMonitor.h"
#include "../../tools/components/imagewidget.h"
#include "../../ImageProcessing/ImageProducer.h"
#include "../../ImageProcessing/MultiImageProducer.h"
#include "../../ImageProcessing/Image.h"
#include "../../ImageProcessing/Images/RGBImage.h"
#include "../../ImageProcessing/Images/YUVImage.h"
#include "../../Fundamental/ConfigReader.h"
#include "../../ImageProcessing/ScanLines.h"
#include "../../ImageProcessing/Image2WorldMapping.h"
#include "../../Fundamental/Vec.h"


using namespace Tribots;
using namespace std;

#define WARP_WIDTH 10000
#define WARP_HEIGHT 10000

int main(int argc, char* argv[])
{
  bool useLut = false;
  bool deWarp = false;

  if (argc<=1) {
    cerr << "Aufruf: " << argv[0] << " Konfigurationsdatei" << endl;
    return -1;
  }

  if (argc > 2) {
    if (string(argv[2]) == "c") {      // show segmentation
      useLut = true;
    }
    else if (string(argv[2]) == "w") { // de-warp
      deWarp = true;
    }
  }

  try{
    ConfigReader config(0);
    config.append_from_file(argv[1]);

    //Support for multiple image sources:
    MultiImageProducer producer(config);
    int num_sources = producer.numSources();

    //---Initializations
    Image* image[num_sources];
    CImageWidget* widgets[num_sources];
    //Initialize QApplication
    QApplication app(argc, argv);	//only one QApplication is allowed.

    for (int i = 0; i < num_sources; i++){
    	image[i] = producer.nextImage(i);	//load first images from different sources
	widgets[i] = new CImageWidget(*image[i]);
	widgets[i]->show();
    }
    app.setMainWidget(widgets[0]);

    QObject::connect( &app, SIGNAL(lastWindowClosed()),
		      &app, SLOT(quit()) );

    //Main loop that shows the images.
    //Active until window is closed.

    //multiple images
    Image* newImage[num_sources];

    while (app.mainWidget()->isVisible()) {

      for (int i = 0; i < num_sources; i++){
    	newImage[i] = producer.nextImage(i);	//load image from currentsources
	//war vorher am Ende:
	widgets[i]->setImage(*newImage[i]);
	delete newImage[i];

      	app.processEvents();

      }//end for all images
    }  //end while loop

  }catch(Tribots::InvalidConfigurationException& e){
    cerr << e.what() << "\n\r" << flush;
  }catch(Tribots::TribotsException& e){
    cerr << e.what() << "\n\r" << flush;
  }catch(std::exception& e){
    cerr << e.what() << "\n\r" << flush;
  }
  return 0;
}
