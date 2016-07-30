######################################################################
# Automatically generated by qmake (1.06c) Tue Mar 23 10:35:38 2004
######################################################################

TEMPLATE = app
INCLUDEPATH += .

# Input
HEADERS +=  \ 
           ../components/imagewidget.h \
           ../../ImageProcessing/Image.h \
           ../../ImageProcessing/ImageSources/IIDC.h \
           ../../ImageProcessing/Images/UYVYImage.h \
           ../../ImageProcessing/Images/RGBImage.h \
           ../../ImageProcessing/Images/YUVImage.h \
           ../../ImageProcessing/Images/ImageConversion.h \ 
           ../../ImageProcessing/ImageBuffer.h \ 
           ../../ImageProcessing/ColorClassifier.h \ 
           ../../ImageProcessing/YUVLookupTable.h \ 
           ../../Fundamental/ConfigReader.h \ 
           ../../Fundamental/Time.h \ 
           ../../Structures/TribotsException.h 
SOURCES += \ 
           IIDCMonitor.cc \
           ../components/imagewidget.cc \
           ../../ImageProcessing/Image.cc \
           ../../ImageProcessing/ImageSources/IIDC.cc \
           ../../ImageProcessing/Images/UYVYImage.cc \
           ../../ImageProcessing/Images/RGBImage.cc \
           ../../ImageProcessing/Images/YUVImage.cc \
           ../../ImageProcessing/Images/ImageConversion.cc \
           ../../ImageProcessing/ImageBuffer.cc \ 
           ../../ImageProcessing/ColorClassifier.cc \ 
           ../../ImageProcessing/YUVLookupTable.cc \ 
           ../../Fundamental/ConfigReader.cc\ 
           ../../Fundamental/Time.cc \ 
           ../../Structures/TribotsException.cc 
LIBS += -lraw1394 -ldc1394_control