CPP    = g++
WINDRES= windres
RM     = rm -f
OBJS   = paint.o \
         AppResource.res

LIBS   = -mwindows -static -lglut32 -lglu32 -lopengl32 -lwinmm -lgdi32
CFLAGS = -std=c++0x -DGLUT_STATIC

.PHONY: paint.exe clean clean-after

all: paint.exe

clean:
	$(RM) $(OBJS) paint.exe

clean-after:
	$(RM) $(OBJS)

paint.exe: $(OBJS)
	$(CPP) -Wall -s -o $@ $(OBJS) $(LIBS)

paint.o: paint.cpp glut_text.h
	$(CPP) -Wall -s -c $< -o $@ $(CFLAGS)

AppResource.res: AppResource.rc
	$(WINDRES) -i AppResource.rc -J rc -o AppResource.res -O coff

