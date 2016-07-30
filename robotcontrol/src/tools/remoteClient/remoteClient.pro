TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

INCLUDEPATH	+= ../../Communication/

HEADERS	+= ./fieldWidget.h \
	./valplotwidget.h

SOURCES	+= main.cpp

FORMS	= remoteClientForm.ui \
	FieldWidgetDialog.ui \
	JoystickDialog.ui \
	realrobotdataview.ui

IMAGES	= images/filenew \
	images/fileopen \
	images/filesave \
	images/print \
	images/undo \
	images/redo \
	images/editcut \
	images/editcopy \
	images/editpaste \
	images/searchfind

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


SOURCES += ../../Communication/tribotsUdpCommunication.cc \
	../../Communication/udpsocket.cc \
	../../Structures/DriveVector.cc \
	../../Structures/Journal.cc \
	../../Structures/TribotsException.cc \	
	../../Fundamental/Time.cc \
	../../Fundamental/ConfigReader.cc \
	../../Fundamental/Angle.cc \
	../../Fundamental/Joystick.cc \
	../../Structures/FieldGeometry.cc \
	../../Structures/RobotProperties.cc \
        ../../Structures/RobotData.cc \
	./fieldWidget.cpp \
        ./valplotwidget.cpp
	
FORMS	= remoteClientForm.ui \
	FieldWidgetDialog.ui \
	JoystickDialog.ui \
	realrobotdataview.ui
