######################################################################
# Automatically generated by qmake (1.06c) Tue Mar 23 10:35:38 2004
######################################################################

TEMPLATE = app
INCLUDEPATH += .
CONFIG += debug
# Input
HEADERS +=  \ 
           ../components/ImageProcessingMonitor.h \
           ../components/imagewidget.h \
           ../../ImageProcessing/Image.h \
           ../../ImageProcessing/ImageMonitors\FileMonitor.h \
           ../../ImageProcessing/ImageIO.h \
           ../../ImageProcessing/JPEGIO.h \
           ../../ImageProcessing/ImageSources/IIDC.h \
           ../../ImageProcessing/ImageSources/FileSource.h \
           ../../ImageProcessing/Images/UYVYImage.h \
           ../../ImageProcessing/Images/RGBImage.h \
           ../../ImageProcessing/Images/YUVImage.h \
           ../../ImageProcessing/Images/ImageConversion.h \ 
           ../../ImageProcessing/ImageBuffer.h \ 
           ../../ImageProcessing/ImageProducer.h \ 
           ../../ImageProcessing/MultiImageProducer.h \ 
           ../../ImageProcessing/ColorClassifier.h \ 
           ../../ImageProcessing/Image2WorldMapping.h \ 
           ../../ImageProcessing/ColorClasses.h \ 
           ../../ImageProcessing/YUVLookupTable.h \ 
           ../../ImageProcessing/RobotMask.h \ 
           ../../Fundamental/ConfigReader.h \ 
           ../../Fundamental/Vec.h \ 
           ../../Fundamental/geometry.h \ 
           ../../Fundamental/Angle.h \ 
           ../../Fundamental/Frame2D.h \ 
           ../../Fundamental/Time.h \ 
           ../../ImageProcessing/ScanLines.h \ 
           ../../ImageProcessing/LineScanning.h \ 
           ../../Structures/TribotsException.h 
SOURCES += \ 
           main.cc \
           ../components/ImageProcessingMonitor.cc \
           ../components/imagewidget.cc \
           ../../ImageProcessing/Image.cc \
           ../../ImageProcessing/ImageIO.cc \
           ../../ImageProcessing/JPEGIO.cc \
           ../../ImageProcessing/ImageMonitors/FileMonitor.cc \
           ../../ImageProcessing/ImageSources/IIDC.cc \
           ../../ImageProcessing/ImageSources/FileSource.cc \
           ../../ImageProcessing/Images/UYVYImage.cc \
           ../../ImageProcessing/Images/RGBImage.cc \
           ../../ImageProcessing/Images/YUVImage.cc \
           ../../ImageProcessing/Images/ImageConversion.cc \
           ../../ImageProcessing/ImageBuffer.cc \ 
           ../../ImageProcessing/ImageProducer.cc \ 
           ../../ImageProcessing/MultiImageProducer.cc \ 
           ../../ImageProcessing/ColorClassifier.cc \ 
           ../../ImageProcessing/Image2WorldMapping.cc \ 
           ../../ImageProcessing/ColorClasses.cc \ 
           ../../ImageProcessing/YUVLookupTable.cc \ 
           ../../ImageProcessing/RobotMask.cc \ 
           ../../Fundamental/ConfigReader.cc\ 
           ../../Fundamental/Vec.cc \ 
           ../../Fundamental/geometry.cc \ 
           ../../Fundamental/Angle.cc \ 
           ../../Fundamental/Frame2D.cc \ 
           ../../Fundamental/Time.cc \ 
           ../../ImageProcessing/ScanLines.cc \ 
           ../../ImageProcessing/LineScanning.cc \ 
           ../../Structures/TribotsException.cc \
           ../../ImageProcessing/Painter.cc

LIBS += -lraw1394 -ldc1394_control -L../../Libs/jpeg-6b/ -ljpeg 
