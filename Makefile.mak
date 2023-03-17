CPP    = g++
WINDRES= windres
RM     = rm -f
OBJS   = main.o \
         AppResource.res

LIBS   = -static -lglut32 -lglu32 -lopengl32 -lwinmm -lgdi32
CFLAGS = -std=c++0x -DGLUT_STATIC

.PHONY: snake_3d.exe clean clean-after

all: snake_3d.exe

clean:
	$(RM) $(OBJS) snake_3d.exe

clean-after:
	$(RM) $(OBJS)

snake_3d.exe: $(OBJS)
	$(CPP) -Wall -s -o $@ $(OBJS) $(LIBS)

main.o: main.cpp
	$(CPP) -Wall -s -c $< -o $@ $(CFLAGS)

AppResource.res: AppResource.rc
	$(WINDRES) -i AppResource.rc -J rc -o AppResource.res -O coff

