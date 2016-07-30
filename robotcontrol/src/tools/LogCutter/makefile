# Name des Zielprogramms:
TRGT = LogCutter

SRCS = \
	LogCutter.cc \
	../../Fundamental/Angle.cc \
	../../Fundamental/Vec.cc \
	../../Fundamental/Time.cc \
	../../Fundamental/stringconvert.cc \
	../../Structures/RobotLocationReadWriter.cc \
	../../Structures/BallLocationReadWriter.cc \
	../../Structures/ObstacleLocationReadWriter.cc \
	../../Structures/VisibleObjectReadWriter.cc \
	../../Structures/DriveVectorReadWriter.cc \
	../../Structures/GameStateReadWriter.cc \
	../../Structures/ObstacleLocation.cc \
	../../Structures/VisibleObject.cc \
	../../Structures/BallLocation.cc \
	../../Structures/DriveVector.cc

OBJS = $(filter %.o,$(SRCS:.cc=.o))

CC = g++
LC = g++
LIB_PATH = 
LIBS = 
INCLUDE_PATH = 
CFLAGS = -Wall -O



# TARGETS:

$(TRGT): $(OBJS)
	$(LC) $(CFLAGS) $(LIB_PATH) -o $(TRGT) $(OBJS) $(LIBS)

%.o : %.cc
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -c -o $@ $<

clean:
	rm $(OBJS) $(TRGT)

depend:
	makedepend $(INCLUDE_PATH) $(SRCS)



# DO NOT DELETE THIS LINE -- make depend depends on it

