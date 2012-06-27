CXX=g++
LD=g++
CFLAGS=-I. -I/usr/include/SDL/ -D__DEBUG__
LIBS=-lSDL -lSDLmain -lSDL_mixer -lSDL_image
CPP_FILES=$(wildcard *.cpp)
OBJ_FILES=$(patsubst %.cpp,%.o,$(CPP_FILES))

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

oemfoeland5: $(OBJ_FILES)
	$(LD) -o $@ $^ $(CFLAGS) $(LIBS); mkdir -p data; cp Textures/*.png data; cp Sounds/*.wav data; cp Music/*.mp3 data; cp Levels/*.lvl data

.PHONY: clean

clean:
	rm *.o; rm oemfoeland5; rm -r data

	

