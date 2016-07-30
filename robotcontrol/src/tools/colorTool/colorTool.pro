TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

LIBS	+= -lraw1394 -ldc1394_control -L../../Libs/jpeg-6b/ -ljpeg

HEADERS	+= ColorTools.h \
	../components/imagewidget.h \
	GrabbingThread.h

SOURCES	+= ColorTools.cpp \
	../components/imagewidget.cc \
	../../ImageProcessing/YUVLookupTable.cc \
	../../ImageProcessing/ColorClassifier.cc \
	../../ImageProcessing/ColorClasses.cc \
	../../ImageProcessing/ImageIO.cc \
	../../ImageProcessing/JPEGIO.cc \
	../../ImageProcessing/ImageProducer.cc \
	../../ImageProcessing/ImageBuffer.cc \
	../../ImageProcessing/Image.cc \
	../../ImageProcessing/Images/RGBImage.cc \
	../../ImageProcessing/Images/YUVImage.cc \
	../../ImageProcessing/Images/UYVYImage.cc \
	../../ImageProcessing/Images/ImageConversion.cc \
	../../ImageProcessing/ImageSources/FileSource.cc \
	../../ImageProcessing/ImageSources/IIDC.cc \
	../../ImageProcessing/ImageMonitors/FileMonitor.cc \
	../../Structures/TribotsException.cc \
	../../Fundamental/ConfigReader.cc \
	../../Fundamental/Time.cc \
	main.cpp \
	GrabbingThread.cpp

FORMS	= colorTool.ui

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

