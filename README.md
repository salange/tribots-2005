This is the trbots software of the German Open 2005 which was released by
the Machine Learning Group of the University of Osnabrück shortly after
winning the competition.

'doc'   contains an inclomplete documentation of the software in German
'odeserver'   contains a simple ODE-based simulator
'robotcontrol'   contains the main program and further debugging and calibration tools

Installation:
1. build the simulator calling 'qmake' and 'make' in the odeserver directory
2. build the main system calling 'install.csh ../../odeserver' in the robotcontrol/src directory

The software runs on Linux systems. You need QT3, libraw1394 and libdc1394_control 
being installed on your computer.

To start the software with the simulator:
1. start the simulator
2. start 'robotcontrol' in the robotcontrol/src directory
3. type 'g'=go
4. type ' ' to stop the robot, type 'q' to quit

The configuration of the software can be changed modifying the config files in robotcontrol/config_files.
The main file is 'robotcontrol.cfg', other files are hierarchically read in.

The robotcontrol/src/tools directory contains a couple of debugging and calibration tools:
'IIDCMonitor'   simply grabs images via IEEE1394 from your camera and displays them
'ImageMask'   is a tool to evaluate which areas of the camera image refer to parts of the robot 
'ImageProcessingMonitor'   can be used to visualize some aspects of image processing
'LogCutter'   cuts long logfiles
'MultiCamMonitor'   grabs images from several cameras
'colorTool'   can be used for color calibration
'config_editor'   is an editor for config files
'remoteClient'   is an outdated tool
'teamcontrol'   is a remote control and debugging tool
'tribotsview'   is a viewer to visualize recorded logfiles step-by-step

