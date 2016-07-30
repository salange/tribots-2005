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

#ifndef _IIDC_H_
#define _IIDC_H_

#include <string>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <vector>

#include "../ImageSource.h"


// Uses libd11 as standard. Support for the incompatible libd10 is not included anymore.

namespace Tribots {

/**
 * IIDC camera driver.
 *
 * Static factory methods are used to provide controlled access to several
 * camera instances. getCamera() creates and manages all camera instances which
 * are connected to the system. This makes sure, that each IIDC object exists
 * only once.
 * On the first call of getCamera() for an UID, the camera with the specified UID
 * ist initialized and started. If no UID is specified, the first camera node is
 * used instead (backwards compatible with the singleton version of this class).
 */
class IIDC : public ImageSource
{

public:

  /* --- STATIC METHODS --- */

  /**
   * IIDC Camera factory. Use this method to get hold of the IIDC_DMA instance.
   * On the first call, all cameras at any port are initialized and stored into
   * the multicam vector. The Camera instance specified by the given arguments
   * (esp. its UID number) is started and retruned. On subsequent calls given
   * framerate and format arguments are ignored and a pointer to the already
   * running instance is returned.
   *
   * \attention Never call delete on the returned pointer! This will cause
   *            a segmentation fault, if you try to get a new instance later.
   *            Use IIDC_DMA::destroy_camera() instead.
   *
   * @param framerate desired framerate (FRAMERATE_[30,15,7_5]).
   * @param device_name video1394 device
   * @param port The port to use. !! Ignored (all ports are scanned), included
   *             for backward compatibility only !!
   * @param uid the uid number of the requested device as hex-string.
   *            This is the same as the UID string in the CAMERA INFO message.
   *            Default: first camera on bus
   * @param delay the average delay of the camera in ms
   */
  static IIDC* getCamera(const char* device_name= "/dev/video1394/0",
			 int port = 0,
			 int framerate = FRAMERATE_30,
			 int format    = MODE_640x480_YUV422,
			 std::string uid = "",
			 bool blocking = 1,
			 int delay = 0);

  /**
   * Destroys all existing instances of the IIDC_DMA class. If theres no
   * instance, it does nothing.
   * Use this method instead of the camera's destructor.
   */
  static void destroyCamera();


  /** Static bus reset handler that is called by the
   * __iidc_bus_reset_handler_static() every time there was a bus reset (camera
   * plugged or unplugged). Finds the existing camera with the right handle
   * and calls its instance resetHandler().
   *
   * There should be no reason to ever call this method manually.
   */
  static int bus_reset_handler_static(raw1394handle_t handle,
				      unsigned int generation);


  /* --- INSTANCE METHODS --- */

  /** 
   * Destructor. Is called by destroy_camera via delete.
   */
  virtual ~IIDC();

  /**
   * Read the next image from the camera. 
   * This call blocks, until a new image has completely arrived at the camera
   * driver. In case of a problem with the camera this method returns a 
   * NULL pointer after about 200ms. To repair the camera, simply remove and
   * replug it and call this method again.
   *
   * \return returns a pointer to an image buffer. The image is encoded in 
   * the yuv color space using the 4:2:2 subsampling scheme.
   */
  virtual ImageBuffer getImage() throw (HardwareException);

  virtual int getWidth() const { return camera.frame_width; }
  virtual int getHeight() const { return camera.frame_height; }

  /**
   * loads the camera settings from the specified memory channel.
   *
   * \param channel the memory channel to load. 0: factory settings
   *                (in most cases), default: 1
   */
  void loadSettings(int channel = 1);
  /**
   * saves the camera settings to the specified memory channel
   *
   * \param channel the memory channel to load. 0: factory settings
   *                (in most cases, only read access), default: 1
   */
  void saveSettings(int channel = 1);

  virtual bool setFramerate(int fps);

  void setWhiteBalance (unsigned int ub, unsigned int vr);
  void getWhiteBalance (unsigned int& ub, unsigned int& vr);
  
  unsigned int getBrightness ();
  void setBrightness (unsigned int);

  unsigned int getSharpness ();
  void setSharpness (unsigned int);

  unsigned int getExposure ();
  void setExposure (unsigned int);
  
  unsigned int getHue ();
  void setHue (unsigned int);
  
  unsigned int getSaturation ();
  void setSaturation (unsigned int);
  
  unsigned int getGammaCorrection ();
  void setGammaCorrection (unsigned int);
  
  unsigned int getShutter ();
  void setShutter (unsigned int);

  unsigned int getGain ();
  void setGain (unsigned int);

  int getDelay () const throw ();

  /**
   * Extern C callback function that is used as bus reset handler and passed
   * to the raw1394 driver. This function simply calls the reset handler
   * of the IIDC singleton (if there is no instance, it creates one).
   */
  friend int  __iidc_bus_reset_handler_static(raw1394handle_t, unsigned int);

 protected:

  /// Static variables that are used by all instances :
  ///

  static std::vector<IIDC*> multicam;      ///< holds started camera instances

  static unsigned int bandwidth_used; ///< holds the bandwidth used by all running cameras so far
  static unsigned int reset_count;    ///< counts multiple calls to the reset function.
  static int num_ports;               ///< number of ports on this aachine

  /// Instance fields of one camera

  dc1394_cameracapture camera;

  raw1394handle_t handle;
  int numNodes;

  dc1394_feature_set features;

  bool isValid;               ///< true if camera is up and running
  int firstFrame;             ///< true before first frame has been captured
  int failure;                ///< increases with consequent failures

  char device_name[32];       ///< remembers the given device name for a reset
  int framerate;              ///< remembers the given framerate for a reset
  int format;                 ///< remembers the given format for a reset
  int port;                   ///< remembers the given port for a reset
  int channel;                ///< remembers the given dma channel
  std::string uid;	      ///< remembers the given uid
  bool isBlocking;            ///< specifies if the camera uses blocking or
                              ///  polling capture mode.
  bool isStarted;



  /**
   * Initialize the first DC camera on the bus at port 0. Never call this one
   * by hand! Use get_camera() instead.
   *
   * \param framerate desired framerate (FRAMERATE_[30,15,7_5]).
   * \param devic_name video1394 device.
   */
 IIDC ( raw1394handle_t handle_param,
 	dc1394_cameracapture camera_param,
	int channel,
	const char* device_name,
	int port,
	int delay
      );


  /**
   * init()
   *
   * Does constructor stuff but can also be called after bus reset to update the
   * camera's properties.
   *
   */
  int init(raw1394handle_t handle_param,
 	   dc1394_cameracapture camera_param,
	   int channel,
  	   int port
          );


  /**
   * (Re-)starts this camera with the given pixel format and framerate;
   * If the camera is already running, it is stopped first.
   *
   * Starting cameras is handled by getCamera().
   *
   * Throws an exception, if the firewire-bus bandwidth does not allow
   *   an other camera to run.
   *
   * @args framerate : the cameras framerate constant (e.g. FRAMERATE_30)
   * @args mode : the cameras resolution and pixel format (e.g.MODE_640x480_YUV422)
   * @returns success
   */
  int startTransmission(int framerate=FRAMERATE_30, int mode=MODE_640x480_YUV422, bool blocking=0);


  /**
   * The reset handler is called, whenever a device is plugged in or
   * removed from the bus.
   */
  int bus_reset_handler(nodeid_t *camera_nodes, int cameracount);



  /**
   * Makes a rough estimation of the bandwidth needed for this camera settings.
   * Calculated values heuristics based on trial&error tests.
   *
   * @returns an estimation of the needed bandwidth in bit/sec
   */
  static unsigned int estimateBandwidth(int framerate, int format);

  const int camera_delay;
};

}

#endif
