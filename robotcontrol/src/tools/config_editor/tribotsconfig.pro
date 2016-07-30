TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= release warn_on thread qt
HEADERS += TribotsSyntax.cpp 
SOURCES	+= main.cpp \
	TribotsSyntax.cpp \
	../../Fundamental/ConfigReader.cc

FORMS	= mainWidget.ui

TARGET = tribotsconfig
