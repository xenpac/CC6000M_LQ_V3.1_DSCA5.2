
# Makefile for GP32

CC	=	arm-linux-g++
#CFLAGS	=	-O2 -W -Wall -ffast-math -fstrict-aliasing -fomit-frame-pointer -mcpu=xscale -mtune=xscale
#CFLAGS	=	-pipe -O3 -fno-rtti -fexpensive-optimizations -ffast-math -fstrict-aliasing -fomit-frame-pointer -mcpu=xscale -mtune=xscale
CFLAGS	=	-O2 -ffast-math -fstrict-aliasing -fomit-frame-pointer -mcpu=xscale -mtune=xscale -D_ZAURUS -DVAR_CYCLES -DCPU_SHUTDOWN -DNO_INLINE_SET_GET
INCPATH	=	

OBJECTS =	.obj/apu.o .obj/spc700.o .obj/soundux.o \
			.obj/cpuexec.o .obj/cpuops.o \
			.obj/cpu.o .obj/tile.o .obj/gfx.o .obj/clip.o .obj/ppu.o .obj/memmap.o .obj/dma.o \
			.obj/dsp1.o .obj/snapshot.o .obj/data.o .obj/globals.o


TARGET	=	snes.a

all: $(TARGET)

$(TARGET): $(OBJECTS)
	rm -f $(TARGET)
	arm-linux-ar -cru $@ $(OBJECTS)
	arm-linux-ranlib $@
	
clean:
	-rm -f $(OBJECTS) $(TARGET)
	-rm -f *~ core

.obj/apu.o: apu.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/apu.o apu.cpp

.obj/spc700.o: spc700.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/spc700.o spc700.cpp

.obj/soundux.o: soundux.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/soundux.o soundux.cpp

.obj/cpuexec.o: cpuexec.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/cpuexec.o cpuexec.cpp

.obj/cpuops.o: cpuops.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/cpuops.o cpuops.cpp

.obj/cpu.o: cpu.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/cpu.o cpu.cpp

.obj/tile.o: tile.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/tile.o tile.cpp

.obj/gfx.o: gfx.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/gfx.o gfx.cpp

.obj/clip.o: clip.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/clip.o clip.cpp

.obj/ppu.o: ppu.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/ppu.o ppu.cpp

.obj/memmap.o: memmap.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/memmap.o memmap.cpp

.obj/dma.o: dma.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/dma.o dma.cpp

.obj/dsp1.o: dsp1.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/dsp1.o dsp1.cpp

.obj/snapshot.o: snapshot.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/snapshot.o snapshot.cpp

.obj/data.o: data.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/data.o data.cpp

.obj/globals.o: globals.cpp
	$(CC) -c $(CFLAGS) $(INCPATH) -o .obj/globals.o globals.cpp









