CXX=em++
LD=em++
CFLAGS=-D__DEBUG__ -DWINCE --preload-file data --preload-file resources.def -s TOTAL_MEMORY=67108864 -s ALLOW_MEMORY_GROWTH=1 -DEMSCRIPTEN
LIBS=-lSDL -lSDLmain -lSDL_mixer -lSDL_image
CPP_FILES=$(wildcard *.cpp)
OBJ_FILES=$(patsubst %.cpp,%.o,$(CPP_FILES))

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

oemfoeland5.js: $(OBJ_FILES)
	$(LD) -o $@ $^ $(CFLAGS) $(LIBS); mkdir -p data; cp Textures/*.png data; cp Sounds/*.wav data; cp Music/*.mp3 data; cp Levels/*.lvl data

.PHONY: clean

clean:
	rm *.o; rm oemfoeland5.js

	

