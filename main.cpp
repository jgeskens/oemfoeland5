#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>

#include "SDL.h"

#include "MusicPlayer.h"
#include "OEMFEnvironment.h"
#include "OEMFIntro.h"
#include "OEMFMain.h"
#include "OEMFImage.h"
#include "OEMFString.h"
#include "OEMFStorage.h"

using namespace std;

int main(int argc, char *argv[])
{
	Uint32 initflags = SDL_INIT_VIDEO | SDL_INIT_AUDIO; 
	
	SDL_Surface * screen;
	Uint8  video_bpp = 32;
	Uint32 videoflags = SDL_SWSURFACE;

	if (fullscreenMode)
		videoflags |= SDL_FULLSCREEN;

	/* Initialize the SDL library */
	if ( SDL_Init(initflags) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
		exit(1);
	}
	
	// SDL_EnableKeyRepeat(0, 0); // disable key repeat

	// Quit SDL at exit (for safety)
	atexit(SDL_Quit);
	
	/* Set 640x480 video mode */
	screen = SDL_SetVideoMode(640, 480, video_bpp, videoflags);
	if (screen == NULL)
	{
		fprintf(stderr, "Couldn't set 640x480x%d video mode: %s\n",
                        video_bpp, SDL_GetError());
		SDL_Quit();
		exit(2);
	}
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Oemfoeland 5", NULL);
	
	// Do all the magic
	
	musicPlayer = new MusicPlayer();
	
	OEMFEnvironment * intro = new OEMFIntro(screen, argv[0], 640, 480, 32);
	intro->run();
	delete intro;
	
	OEMFEnvironment * mainmenu = new OEMFMain(screen, argv[0], 640, 480, 32);
	mainmenu->run();
	delete mainmenu;
	
	releaseDataMemory(); // loaded by OEMFIntro !!!
	delete musicPlayer;
	
	/* Clean up the SDL library */
	SDL_Quit();
	return(0);
}
