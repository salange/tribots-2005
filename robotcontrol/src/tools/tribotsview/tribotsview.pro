
TEMPLATE        = app
LANGUAGE        = C++
INCLUDEPATH    += .
CONFIG         += qt warn_on release
LIBS           += -L../../Libs/jpeg-6b/ -ljpeg
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

# Input
HEADERS += CycleContainer.h \
           CycleInfo.h \
           FieldOfPlay.h \
           imageview.ui.h \
           PaintPreferences.h \
           RotateView.h \
           SLErrorWidget.h \
           tribotsview.ui.h \
           MonoLED.h \
           ../components/imagewidget.h
INTERFACES += imageview.ui \
           tribotsview.ui
SOURCES += CycleContainer.cpp \
           CycleInfo.cpp \
           FieldOfPlay.cpp \
           main.cpp \
           RotateView.cpp \
           SLErrorWidget.cpp \
           MonoLED.cpp \
           ../../Structures/BallLocation.cc \
           ../../Structures/ObstacleLocation.cc \
           ../../Structures/VisibleObject.cc \
           ../../Fundamental/Angle.cc \
           ../../Fundamental/Vec.cc \
           ../../Fundamental/Time.cc \
           ../../Fundamental/stringconvert.cc \
           ../../Structures/FieldGeometry.cc \
           ../../Fundamental/geometry.cc \
           ../../Fundamental/ConfigReader.cc \
           ../../Structures/TribotsException.cc \
           ../../WorldModel/FieldLUT.cc \
           ../../WorldModel/VisualPositionOptimiser.cc \
           ../../WorldModel/VisualContainer.cc \
           ../../ImageProcessing/ImageSources/FileSource.cc \
           ../../ImageProcessing/ImageIO.cc \
           ../../ImageProcessing/JPEGIO.cc \
           ../components/imagewidget.cc \
           ../../ImageProcessing/ImageBuffer.cc \
           ../../ImageProcessing/Image.cc \
           ../../ImageProcessing/Images/RGBImage.cc \
           ../../ImageProcessing/Images/ImageConversion.cc \
           ../../ImageProcessing/ColorClassifier.cc \
           ../../Structures/RobotLocationReadWriter.cc \
           ../../Structures/BallLocationReadWriter.cc \
           ../../Structures/ObstacleLocationReadWriter.cc \
           ../../Structures/VisibleObjectReadWriter.cc \
           ../../Structures/GameStateReadWriter.cc \
           ../../Fundamental/Frame2D.cc
