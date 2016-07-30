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
 */

#include "IIDC.h"
#include <iostream>

#include <unistd.h>
#include <libraw1394/csr.h>

#include <signal.h>
#include <sys/time.h>

#include <unistd.h>

#ifndef INFO_OUT
#define INFO_OUT cerr
#endif

#ifndef ERROR_OUT
#define ERROR_OUT cerr
#endif

namespace Tribots {

using namespace std;

/* Initialize static variables */

vector<IIDC*> IIDC::multicam = vector<IIDC*>();
unsigned int IIDC::bandwidth_used = 0;
unsigned int IIDC::reset_count = 0;
//int IIDC::numCameras = 0;
//nodeid_t* IIDC::camera_nodes = NULL;
//raw1394handle_t* IIDC::handles =  NULL;
int IIDC::num_ports = 0;


/**
 * Signal hanlder for the timer "trick". Does nothing, but enables us to
 * return from the dc1394_dma_capture library call, even if the wait modus
 * is used.
 */
static
void sighandler_nop(int) {   //  blocking call
  ERROR_OUT << "IIDC: sighandler_nop called. Call to "
	    << "dc1394_dma_single_capture() "
	    << "did not return." << endl;
}


/**
 * Reads a quadlet from a given ieee1394 port. Used to detect bus resets.
 * Inspired by the cooked_read of coriander and gscanbus.
 */
static
void* dummy_read(void* handle)
{
  quadlet_t quadlet;
  int retval = -1;

  retval = raw1394_read(*(raw1394handle_t*)handle,
			0xffc0 |
			raw1394_get_local_id(*(raw1394handle_t*)handle),
			CSR_REGISTER_BASE + CSR_CYCLE_TIME, 4,
			&quadlet);

  return NULL;
}


/**
 * Static C bus reset hanlder passed to the raw1394 system.
 * Calls bus_reset_handler_static, which passes the reset to the right camera instance.
 *
 * @returns Return value of the instance resetter.
 */
int __iidc_bus_reset_handler_static( raw1394handle_t handle,
				     unsigned int generation )
{
//  ERROR_OUT << " raw1394: Bus reset handler called!" << endl;
  return IIDC::bus_reset_handler_static(handle, generation);
}


/* Uses the handle to find out which camera got reset and calls its bus_reset_handler. */
int IIDC::bus_reset_handler_static(raw1394handle_t handle,
			           unsigned int generation )
{
  // increase reset count
  ++reset_count;

  INFO_OUT << "#INTERUPT : Reset handler called. Count: "<<reset_count << endl;
   // if this was the first reset count: perform reset. Otherwise do nothing (and jump back to
   //  the already executed reset.
  if(reset_count==1){
     int result=0;
     usleep(1000*200);  // safety delay

     // cleanup everything to get a fresh start

     for(unsigned int i=0; i<multicam.size(); i++){

        if(multicam[i]->isValid){
           dc1394_stop_iso_transmission ( multicam[i]->handle, multicam[i]->camera.node );
           // update bandwidth usage
           IIDC::bandwidth_used -= IIDC::estimateBandwidth(multicam[i]->framerate, multicam[i]->format);

//           INFO_OUT << "done\ndma_unlisten... ";
           dc1394_dma_unlisten( multicam[i]->handle, &(multicam[i]->camera) );
//           INFO_OUT << "done\ndma_release... ";
           dc1394_dma_release_camera( multicam[i]->handle, &(multicam[i]->camera));
//           INFO_OUT << "done" << endl;
	   multicam[i]->isValid = false;

	}
     }

       // no cameras should be running anymore: no bandwidth used
     bandwidth_used = 0;


       // This loop is repeated if more than one bus reset was detected, to make
       // sure the latest changes (e.g. a camera has been replugged) are taken
       // into account.
       // If all cameras have been restarted, however, the loop is broken.
     while(reset_count>0){

        //Remember how often the bus has been resettet.
        raw1394_update_generation(handle, generation);

        //
        // Check for right number of cameras on port...
        //
        raw1394handle_t raw_handle;
        nodeid_t *camera_nodes=NULL;

        int camCount=0;

        for(int port=0; port<num_ports; port++){
            int camsOnPort;
            //new dc1394 handle
            raw_handle = dc1394_create_handle(port);

            // get number of cameras on the current port.
            // camera_nodes is the nodeid_t* array on the current port, camsOnPort the number of cameras found.
            // if the third parameter is "1", camera infos are printed to std::out
            camera_nodes = dc1394_get_camera_nodes(raw_handle, &camsOnPort, port);
            camCount+=camsOnPort;

            raw1394_destroy_handle(raw_handle);
        }

        INFO_OUT << "reset_handler_static : "<< camCount << " cameras found.";

	// If not all cameras have been replugged yet: Continue with loop if more resets have been called.
        if((unsigned int)camCount < multicam.size()){
           INFO_OUT << ".. waiting for all cameras to be replugged." << endl;
           --reset_count;
	   continue;
        }

        INFO_OUT << " Cameras will be restarted now ...";

        // If all camaras are present again: restart them.
        result = 1;
        for(unsigned int i=0; i<multicam.size(); i++) {
           if(multicam[i]->bus_reset_handler(camera_nodes, camCount) <= 0)
	     result = 0;
	}

        if(result==1){
           INFO_OUT << " done." << endl;
	   reset_count=0;
           break;
	}
     }
     return (result>0);
  }

  return 0;
}

/** this reset handler is called to restart the camera after a bus reset. */
int IIDC::bus_reset_handler( nodeid_t *camera_nodes, int camCount)
{

   //
   // Pick Camera with right uid -> reinitialize and restart it.
   //
   dc1394_cameracapture new_camera;
   raw1394handle_t new_handle;

   int correctCam;
   for(correctCam=0; correctCam<camCount; correctCam++){

      new_handle = dc1394_create_handle(this->port);

      // Store an accessible version of the uid.
      char id_buf[20];   //buffer for the hex string representing the uid.
      dc1394_camerainfo info;
      dc1394_get_camera_info(new_handle, camera_nodes[correctCam], &info);

      // from libdc_print_camera_node:
      // building of the correct uid hex string.
      quadlet_t value[2];
      value[0]= info.euid_64 & 0xffffffff;
      value[1]= (info.euid_64 >>32) & 0xffffffff;
      sprintf(id_buf, "0x%08x%08x", value[1], value[0]);

      if(string(id_buf) == this->uid){
         break;
      }
   }

   if(correctCam>=camCount){
      ERROR_OUT << "\nBus_Reset failed : Camera with correct UID not found!" << endl;
      return 0;
   }

   new_camera.node = camera_nodes[correctCam];

   if (new_camera.node == numNodes - 1) {
      ERROR_OUT << "\nBus_Reset failed : Camera became the root node. Replug." << endl;
      return 0;
   }

   // Right camera found: initialize and restart transmission!
   try {

      init(new_handle, new_camera, this->channel, this->port);
      if(this->isStarted){
        if(startTransmission(framerate, format, isBlocking) == DC1394_SUCCESS){
            return 1;
        } else {
            ERROR_OUT << "\nBus_Reset failed : could not (re-)start camera transmission." << endl;
            return 0;
        }
      }
      else  //Cam was never running so it is not restarted.
         return 1;

     // Catch Hardware exception: Try again later
   } catch( BadHardwareException e ){
      ERROR_OUT << "\nBus_Reset failed : Could not initialize or restart camera: " << e.what() << endl;
      return 0;
   }




}


//Tobias und Christopher: Neue Multicammethode.
IIDC*
IIDC::getCamera( const char* device_name,
       int port,       // For backward compatibility only
       int framerate,
       int format,
       string uid,
       bool blocking,
       int delay )
{

     //  If cameras are not yet initialized, search for cameras on all ports and
     //   register them in the multicam vector.
     //
   if (multicam.empty()){
 
      int numCameras = 0;            //total amount of cameras (all ports)

      dc1394_cameracapture new_camera;
      raw1394handle_t dc_handle;

      nodeid_t *camera_nodes;
      raw1394handle_t raw_handle;

      //Get handle to the firewire bus.
      raw_handle = raw1394_new_handle();
      if (raw_handle == NULL) {
         ERROR_OUT << "Unable to create raw1394_handle. Did you load the drivers?";
         throw BadHardwareException(__FILE__
                ": Could not create handle during setup");
      }
      //Get the number of used ports on the bus.
      num_ports = raw1394_get_port_info(raw_handle, NULL, 0);   // Dont print portinfos.
      //destroy current handle --> Why? Do we have to?
      raw1394_destroy_handle(raw_handle);


      //for all ports initialize all cameras on that port
      for (int p = 0; p < num_ports; ++p) {
         int camCount;

         //new dc1394 handle
         dc_handle = dc1394_create_handle(p);

         //get number of cameras on the current port.
         //camera_nodes is the nodeid_t* array on the current port, camCount the number of cameras found.
         //the "1" prints the camera infos on std::out
	 camera_nodes = dc1394_get_camera_nodes(dc_handle, &camCount, 1);

	 //destroy current handle. --> Why? Do we have to?
         dc1394_destroy_handle(dc_handle);

         //for all cameras found on that port
         //Set them up for capture and count their total number (numCameras)
         for(int i = 0; i < camCount; ++numCameras, ++i){

            //get handle to port and remember it for the current camera.
            //remember that numCameras is the total amount of so far initialized cameras.
            dc_handle = dc1394_create_handle(p);
            if (dc_handle == NULL) {
               ERROR_OUT << "\nUnable to aquire a raw1394 handle\n\n"
	       		 << "Please check \n"
	       		 << "  - if the kernel modules video1394, "
	      		 << "`ieee1394',`raw1394' and `ohci1394'"
	      		 << " are loaded \n"
	      		 << "  - if you have read/write access to /dev/raw1394\n\n";
               throw BadHardwareException(__FILE__
                           ": Could not create handle during setup");
            }

            //Remember the nodeid for the current camera.
            new_camera.node = camera_nodes[i];

            //Construct the IIDC Object for the current camera.
            //Register the new IIDC Object in the static multicam vector.
            // The camera will not be sending data until started via startTransmission() was called;
            multicam.push_back(new IIDC(dc_handle, new_camera, i+1 /*channel*/, device_name, p, delay));

         } //end for all cameras on that port

      dc1394_free_camera_nodes(camera_nodes);      //frees camera nodes array
      }//end for all ports


         // If still no camera has been found: return exception
      if(multicam.empty()){
         ERROR_OUT << "Sorry, no camera was found on any firewire port." << endl;
         throw BadHardwareException(__FILE__
                 ": No cameras found.");
      }

   }

      // If UID was specified, look for cam with correct uid, start and return it.
      //

   if(uid!=""){
      for (unsigned int i = 0; i < multicam.size(); i++){
      if ((multicam[i]->uid) == uid){
          if(!multicam[i]->isValid) // Not started yet: start
             multicam[i]->startTransmission(framerate, format, blocking);
          return multicam[i];
      }

      }
      ERROR_OUT << "Didn't find camera with uid "<< uid << ". Returning first camera on bus." << endl;
   }

       // If uid not found, start and return camera node 0
       //
   if(!multicam.front()->isValid) // Not started yet: start
      multicam.front()->startTransmission(framerate, format, blocking);

   return multicam.front();
}


/**Deletes all cameras.*/
void
IIDC::destroyCamera()
{
  while(!multicam.empty()){
	delete multicam.back();
 	multicam.pop_back();
  }

}


IIDC::IIDC (raw1394handle_t handle_param, dc1394_cameracapture camera_param, int channel, const char* device_name, int port, int delay)
  : ImageSource() /*super constructor*/, framerate(0), format(0), camera_delay (delay)
{
   isStarted = false;
   strcpy(this->device_name, device_name);

   this->init(handle_param, camera_param, channel, port);

      // Store an accessible version of the uid.
   char id_buf[20];   //buffer for the hex string representing the uid.
   dc1394_camerainfo info;
   dc1394_get_camera_info(handle, camera.node, &info);

   // from libdc_print_camera_node:
   // building of the correct uid hex string.
   quadlet_t value[2];
   value[0]= info.euid_64 & 0xffffffff;
   value[1]= (info.euid_64 >>32) & 0xffffffff;
   sprintf(id_buf, "0x%08x%08x", value[1], value[0]);
   this->uid = string(id_buf);

}


int IIDC::init(raw1394handle_t handle_param,
 	       dc1394_cameracapture camera_param,
	       int channel,
  	       int port
              ){

   // Camera is not working yet
   isValid = false;
   this->channel = channel;
   this->port = port;

   // Remember arguments as instance fields.
   handle = handle_param;
   camera = camera_param;

   ///////////////// reset mechanism  //////////////////
   raw1394_set_bus_reset_handler(handle, __iidc_bus_reset_handler_static);
   // register a handler that is called, whenever the bus is resetted


   if (dc1394_init_camera (handle, camera.node) == DC1394_FAILURE)
    {
      ERROR_OUT << "Error initializing camera on node " << camera.node <<
	std::endl;
      throw BadHardwareException(__FILE__
				  ": Error initializing camera.");

    }

   // set trigger mode
   if (dc1394_set_trigger_mode (handle, camera.node, TRIGGER_MODE_0) !=
      DC1394_SUCCESS)
    {
      INFO_OUT << "unable to set camera trigger mode\n";
    }


   // Load camera settings stored in (hardware-) channel 1 of the camera
   usleep(100000);
   dc1394_memory_load (handle, camera.node, (unsigned int) 1);

   return 1;

}


/* bandwidth usage estimation */
unsigned int
IIDC::estimateBandwidth(int framerate, int format){

   unsigned int bandwidth=0;
   switch(format){
      case MODE_640x480_YUV411 : bandwidth = 640*480*16; break;	// Expects ca 1 byte of protocol/pixel...
      case MODE_320x240_YUV422 : bandwidth = 320*240*24; break;
      case MODE_640x480_YUV422 :
      default:                   bandwidth = 640*480*24; break;
   }

   switch(framerate){
      case FRAMERATE_7_5 : bandwidth*=7;  break;
      case FRAMERATE_15  : bandwidth*=15; break;
      case FRAMERATE_30  :
      default:             bandwidth*=30; break;
   }

   return bandwidth;
}




/** (re)start a camera */
int
IIDC::startTransmission(int framerate, int format, bool blocking){

     // First clean up, if the cam is still running ...
     //
   dc1394bool_t isOn;

   if (dc1394_get_iso_status (handle, camera.node, &isOn) ==
       DC1394_FAILURE)
   {
       ERROR_OUT << "Capture::initialize: Can't querry iso status\n" << std::
	flush;
      throw BadHardwareException(__FILE__
				  ": Can't querry iso status.");
   }
   else if (isOn == DC1394_TRUE){
      dc1394_stop_iso_transmission (handle, camera.node);
   }

      // if camera was started before: free bandwidth
   if(isValid)
      bandwidth_used -= estimateBandwidth(this->framerate, this->format);
   isValid = false;


     // Check if enough bus bandwidth is available ..
     //
   unsigned int bandwidth_needed = estimateBandwidth(framerate, format);

   if(bandwidth_used + bandwidth_needed > 400*1024*1024){
      ERROR_OUT << "Could not start camera capture due to limited bandwidth." << std::endl;
      ERROR_OUT << "A total bandwidth of approx. "<< (bandwidth_used+bandwidth_needed)/(1024.0*1024.0);
      ERROR_OUT << " Mbit/sec would be required." << std::endl;

      throw BadHardwareException(__FILE__
			         ": Error : bandwidth.");
   }



     //Initialize the cameras capture behavior
     //
   if (dc1394_dma_setup_capture (handle,
                                camera.node, channel, FORMAT_VGA_NONCOMPRESSED, format,
                                SPEED_400, framerate,
                                5,		// Buffers  //5 buffers is ok 2 will lead to hangups
                                1,		// drop frames
                                device_name, &camera) == DC1394_FAILURE)
   {
      ERROR_OUT << "Error initializing dma-capture" << std::endl;
      throw BadHardwareException(__FILE__
                                      ": Error initializing dma-capture.");
   }

     // Start iso transmission
     //
   if (dc1394_start_iso_transmission(handle, camera.node) != DC1394_SUCCESS)
   {
      ERROR_OUT << "Start of iso transmission failed" << std::endl;
         throw BadHardwareException(__FILE__
                                    ": Error starting iso transmission.");
    }



     // Update instance fields...
     //
   bandwidth_used += bandwidth_needed;

   this->isValid = true;	// Camera is running
   this->format = format;
   this->framerate = framerate;
   this->firstFrame = true;
   this->isBlocking = blocking;
   this->isStarted = true;

   return DC1394_SUCCESS;
}


IIDC::IIDC::~IIDC()
{
  //TODO: If more than one camera is destructed, are the handles
  //      valid through the destruction process?
  if (this->isStarted){
     if (dc1394_stop_iso_transmission (handle, camera.node) != DC1394_SUCCESS)
       ERROR_OUT << "couldn't stop the camera? in IIDC::~IIDC()\n";
     dc1394_dma_unlisten(handle, &camera);
     dc1394_dma_release_camera(handle, &camera);
  }
  dc1394_destroy_handle (handle);
}


ImageBuffer
IIDC::getImage () throw (HardwareException)
{
  int returnval = DC1394_FAILURE;          // hold the return value of dc calls




//  dc1394_dma_single_capture_poll(&camera);

  if (isValid) {                           // try to capture is camera object
                                           // is valid

     // TODO: If in non-blocking mode no new frame is captured this time, the old
     //       buffer is used again, but will be already released: causes problems?
    if (firstFrame) {
       //firstFrame = 0;
    } else {
       dc1394_dma_done_with_buffer(&camera);
    }

    // To get an image, the dma_sinlge_capture call is used in the blocking
    // (waiting) mode. This is more efficient than using the polling method,
    // but has a general problem: It's not possible to specify a timeout,
    // after that the call should return in case of a problem. Therefore,
    // when occuring a hardware problem, the capture call would never return.
    //
    // To handle this problem, we register a SIGALRM signal handler, that
    // guarantees us a return after 90 ms (suggested by Martin Lauer).

    signal(SIGALRM, &sighandler_nop);      // register empty signal handler
    itimerval t1, t2;
    t1.it_interval.tv_sec  = 0;
    t1.it_interval.tv_usec = 0;
    t1.it_value.tv_sec     = 0;
    t1.it_value.tv_usec    = 900000;       // set 90 ms timer
    setitimer (ITIMER_REAL, &t1, &t2) ;    // to return from blocking call

    if(isBlocking)
       returnval = dc1394_dma_single_capture(&camera) ;  //  blocking call
    else
       returnval = dc1394_dma_single_capture_poll(&camera); // non-blocking


    t1.it_interval.tv_sec  =
    t1.it_interval.tv_usec =
    t1.it_value.tv_sec     =
    t1.it_value.tv_usec    = 0;            // set to zero to stop timer
    setitimer (ITIMER_REAL, &t2, NULL);    // stop timer

  }

  // If no new frame was ready (in polling mode): firstFrame = true
  //   -> dont release buffer next time (would cause error)
  // And check if the reason was a bus reset (eg camera has been unplugged).
  // This is neccessary, because neither a bus reset nor a DC_FAILURE is
  // produced, if a camera in polling mode is unplugged.
  if(returnval == DC1394_NO_FRAME){
     dummy_read(&handle);    // check if a bus reset is the reason for the
                             // unsuccessful capture.

       // If the reset handler was called (caused by dummy_read), the camera
       // is set to non-valid. In this case, tell the ImageProducer that there
       // was an error.
     if(isValid==false || firstFrame){
	usleep(10000);
        throw HardwareException(__FILE__ ": DC1394 Failure getting image.");
     }

     firstFrame = true;
  } else firstFrame = false;

  // in most cases the dma_single_capture fails, the connection to the camera
  // was physically lost. to repair those cases, the method simply waits for
  // 100ms before it tries to do a dummy read on the bus. If a device
  // has been replugged in the meantime, such a read would cause a
  // bus-reset-event to be emitted.
  //
  // On such an event, our own registered reset handler is called. The reset
  // handler does everything necessary to restart the replugged device.

  if ( returnval == DC1394_FAILURE ) {     // capture didn't succeed (or was
    firstFrame = true;

    usleep(100000);                        // sleep 100 ms
    dummy_read(&handle);                   // try to detect a bus reset

    throw HardwareException(__FILE__ ": DC1394 Failure getting image.");

  } else {                                 // DC1394_SUCCESS

    // determin output format
    int outputFormat;
    double bytePP;    // byte per pixel

    // transform the libdc1394 format id to the corresponding ImageBuffer's
    // format id
    switch (format) {
    case MODE_160x120_YUV444:
      outputFormat = ImageBuffer::FORMAT_UYV; bytePP = 3.; break;
    case MODE_320x240_YUV422:
    case MODE_640x480_YUV422:
      outputFormat = ImageBuffer::FORMAT_YUV422; bytePP = 2.; break;
    case MODE_640x480_YUV411:
      outputFormat = ImageBuffer::FORMAT_YUV411; bytePP = 1.5; break;
    case MODE_640x480_RGB:
      outputFormat = ImageBuffer::FORMAT_RGB; bytePP = 3.; break;
    case MODE_640x480_MONO:
      outputFormat = ImageBuffer::FORMAT_MONO; bytePP = 1.; break;
    default:
      outputFormat = ImageBuffer::FORMAT_MONO16; bytePP = 2.; break;
    }

    // wrap an ImageBuffer around libdc1394_control's memory that holds the
    // image
    return ImageBuffer(camera.frame_width,
		       camera.frame_height,
		       outputFormat,
		       (unsigned char *) camera.capture_buffer,
		       (int)(camera.frame_width*camera.frame_height*bytePP));
  }
}




void 
IIDC::setWhiteBalance (unsigned int ub, unsigned int vr) {
   dc1394_set_white_balance (handle, camera.node, ub, vr);
}

void
IIDC::getWhiteBalance (unsigned int& ub, unsigned int& vr) {
  dc1394_get_white_balance (handle, camera.node, &ub, &vr);
}

unsigned int
IIDC::getBrightness () {
  unsigned int s;
  dc1394_get_brightness (handle, camera.node, &s);
  return s;
}

void 
IIDC::setBrightness (unsigned int s) {
  dc1394_set_brightness (handle, camera.node, s);
}

unsigned int
IIDC::getSharpness () {
  unsigned int s;
  dc1394_get_sharpness (handle, camera.node, &s);
  return s;
}

void 
IIDC::setSharpness (unsigned int s) {
  dc1394_set_sharpness (handle, camera.node, s);
}

unsigned int 
IIDC::getExposure () {
  unsigned int s;
  dc1394_get_exposure (handle, camera.node, &s);
  return s;
}

void 
IIDC::setExposure (unsigned int s) {
  dc1394_set_exposure (handle, camera.node, s);
}

unsigned int 
IIDC::getHue () {
  unsigned int s;
  dc1394_get_hue (handle, camera.node, &s);
  return s;
}

void 
IIDC::setHue (unsigned int s) {
  dc1394_set_hue (handle, camera.node, s);
}

unsigned int 
IIDC::getSaturation () {
  unsigned int s;
  dc1394_get_saturation (handle, camera.node, &s);
  return s;
}

void 
IIDC::setSaturation (unsigned int s) {
  dc1394_set_saturation (handle, camera.node, s);
}

unsigned int 
IIDC::getGammaCorrection () {
  unsigned int gamma;
  dc1394_get_gamma (handle, camera.node, &gamma);
  return gamma;
}

void
IIDC::setGammaCorrection (unsigned int gamma) {
  dc1394_set_gamma (handle, camera.node, gamma);
}

unsigned int 
IIDC::getShutter () {
  unsigned int s;
  dc1394_get_shutter (handle, camera.node, &s);
  return s;
}
  
void 
IIDC::setShutter (unsigned int s) {
  dc1394_set_shutter (handle, camera.node, s);
}

unsigned int 
IIDC::getGain () {
  unsigned int s;
  dc1394_get_gain (handle, camera.node, &s);
  return s;
}

void 
IIDC::setGain (unsigned int s) {
  dc1394_set_gain (handle, camera.node, s);
}

void
IIDC::loadSettings(int channel)
{
  dc1394_memory_load (handle, camera.node, channel);
}

void 
IIDC::saveSettings(int channel) 
{
  dc1394_memory_load (handle, camera.node, channel);
}

bool
IIDC::setFramerate(int fps)
{
  int framerate = FRAMERATE_3_75;
  if (fps <= 3) framerate = FRAMERATE_3_75;
  else if (fps <= 7) framerate = FRAMERATE_7_5;
  else if (fps <= 15) framerate = FRAMERATE_15;
  else framerate = FRAMERATE_30;
  dc1394_stop_iso_transmission (handle, camera.node);
  dc1394_set_video_framerate(handle, camera.node, framerate);  // error check
  dc1394_start_iso_transmission (handle, camera.node);
  return true;
}

int IIDC::getDelay () const throw () {
  return camera_delay;
}


} /* namespace Tribots */

