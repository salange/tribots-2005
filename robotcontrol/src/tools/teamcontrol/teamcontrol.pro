
TEMPLATE        = app
LANGUAGE        = C++
INCLUDEPATH    += .
CONFIG         += qt warn_on release
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

# Input
HEADERS += \
 	RefboxClient.h \
	RefboxWidget.ui.h \
	RemoteRobotState.h \
	RemoteTeamState.h \
        	RemoteRobotWidget.ui.h \
           	TeamControlWidget.ui.h \
	CoachWidget.ui.h \
           	../tribotsview/FieldOfPlay.h \
	../remoteClient/valplotwidget.h
INTERFACES += \
	RefboxWidget.ui \
	RemoteRobotWidget.ui \
	TeamControlWidget.ui \
	CoachWidget.ui \
	../remoteClient/JoystickDialog.ui \
	../remoteClient/realrobotdataview.ui
SOURCES += \
	main.cpp \
           	RefboxClient.cpp \
	Coach.cpp \
           	../../Fundamental/Time.cc \
           	../../WorldModel/RefereeStateMachine.cc \
           	../../Fundamental/Vec.cc \
           	../../Fundamental/Angle.cc \
           	../../Fundamental/ConfigReader.cc \
           	../../Communication/tribotsUdpCommunication.cc \
           	../../Communication/tribotsUdpCommunicationBuffer.cc \
           	../../Structures/BallLocation.cc \
           	../../Communication/udpsocket.cc \
           	../../Structures/DriveVector.cc \
           	../../Structures/FieldGeometry.cc \
           	../../Structures/RobotProperties.cc \
           	../../Structures/RobotData.cc \
           	../../Structures/TribotsException.cc \
           	../tribotsview/FieldOfPlay.cpp \
           	../tribotsview/CycleInfo.cpp \
	../remoteClient/valplotwidget.cpp \
           	../../Structures/ObstacleLocation.cc \
           	../../Structures/VisibleObject.cc \
	../../Structures/MessageBoard.cc \
           	../../Fundamental/geometry.cc \
	../../Structures/Journal.cc \
	../../Fundamental/stringconvert.cc \
	../../Fundamental/Joystick.cc \
	../../Fundamental/Frame2D.cc \
        Policy.cpp
