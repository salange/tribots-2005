#! /bin/tcsh -f

if ( $#argv < 1 ) then
    echo "Erstinstallation von robotcontrol"
    echo "Modus1: Installation mit Simulatoranbindung simsrv2:"
    echo "Aufruf: $0 <simsrv2-Verzeichnis> [all]"
    echo "Modus2: Installation mit Simulatoranbindung odesim:"
    echo "Aufruf: $0 <odesim-Verzeichnis> [all]"
    echo "Modus3: Installation ohne Simulatoranbindung:"
    echo "Aufruf: $0 none [all]"
    echo "Das 2. Argument all erzeugt auch die Kalibrierungstools"
else
    set mode = 0
    if ( $1:h:t == "simsrv2" || $1:t == "simsrv2" ) then
	set mode = 1
    else if ( $1:h:t == "odeserver" || $1:t == "odeserver") then
	set mode = 2
    endif

    if ( $mode == 0 ) then
    	\cp makefile.wo_simulator makefile
    else if ( $mode == 1 ) then
    	\cp makefile.simulator makefile
	echo "-------------------------------------------"
	echo "creating symbolic link to simsrv2 directory"
	echo "-------------------------------------------"
	set simlink = $1
	cd Simulator
	if (! -e $simlink ) then
	    set simlink = "../$simlink"
	endif
	ln -s $simlink
	cd ..
	ln -s $HOME/.simsrv/config/robots/tribot.cfg
	ln -s $HOME/.simsrv/config/robots/tribot_goalie.cfg
   else if ( $mode == 2 ) then
    	\cp makefile.odesim makefile
	echo "-------------------------------------------"
	echo "creating symbolic link to odesim directory"
	echo "-------------------------------------------"
	set simlink = $1
	cd Simulator
	if (! -e $simlink ) then
	    set simlink = "../$simlink"
	endif
	ln -s $simlink
	cd ..
    endif
    echo "-------------------------------------------"
    echo "compiling the robot control library"
    echo "-------------------------------------------"
    make -C Libs/robotCtrLib/
    echo "-------------------------------------------"
    echo "compiling the robotcontrol main program"
    echo "-------------------------------------------"
    make depend
    make
#    echo "-------------------------------------------"
#    echo "compiling the remoteClient"
#    echo "-------------------------------------------"
#    cd tools/remoteClient
#    qmake remoteClient.pro
#    make
#    cd ../..
    echo "-------------------------------------------"
    echo "compiling tribotsview"
    echo "-------------------------------------------"
    cd tools/tribotsview
    qmake tribotsview.pro
    make
    cd ../..
    echo "-------------------------------------------"
    echo "compiling teamcontrol"
    echo "-------------------------------------------"
    cd tools/teamcontrol
    qmake teamcontrol.pro
    make
    cd ../..

    if ( $#argv >= 2 ) then 
	if ( $2 == "all" ) then
	    echo "-------------------------------------------"
	    echo "compiling ImageMask"
	    echo "-------------------------------------------"
	    cd tools/ImageMask
	    qmake ImageMask.pro
	    make
	    cd ../../
	    echo "-------------------------------------------"
	    echo "compiling colorTool"
	    echo "-------------------------------------------"
	    cd tools/colorTool
	    qmake colorTool.pro
	    make
	    cd ../../
	    echo "-------------------------------------------"
	    echo "compiling ImageProcessingMonitor"
	    echo "-------------------------------------------"
	    cd tools/ImageProcessingMonitor
	    qmake ImageProcessingMonitor.pro
	    make
	    cd ../../
	endif
    endif
    
    echo "-------------------------------------------"
    echo "basic installation finished"
    echo "-------------------------------------------"
endif

    
