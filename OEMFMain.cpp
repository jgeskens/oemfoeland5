/*
 *  OEMFMain.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <math.h>
#include "emscripten.h"

#include "OEMFMain.h"
#include "OEMFStorage.h"
#include "OEMFString.h"
#include "OEMFLevelEdit.h"
#include "OEMFGame.h"

#include "EventLoopStack.h"

OEMFMain :: OEMFMain(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp)
	: OEMFEnvironment(screen, execPath, screenWidth, screenHeight, screenBpp)
{
	m_menuOptions = new string[2];
	m_menuCount = 2;
	m_menuOptions[0] = "NEW GAME";
	m_menuOptions[1] = "LEVEL EDITOR";
	menuIndex = 0;
	done = 0;
	frames = 0;
	songfile = PREPATH "boiling.mp3";
}

OEMFMain :: ~OEMFMain(void)
{
}

void OEMFMain :: drawIntro(void)
{
	clearWithColor(0xFF000000, false);
	blitImage(images[IMG_INTRO], 0, 0);
	SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight);
}

void OEMFMain :: one_iter(void)
{
	clearWithColor(0xFF000000, false);
	blitImage(images[IMG_INTRO], 0, 0);
	for (unsigned int i = 0; i < m_menuCount; i++)
		fonts[FNT_MENU]->blitText(this, m_menuOptions[i].c_str(), 160, 240 + i * 32, 480, false);
	blitImage(images[IMG_OEMFOE], 128, (unsigned int) (240 + menuIndex * 32 + sin(frames / 8.0) * 8));
	SDL_UpdateRect(m_screen, 128, 232, 512, 248);
	/* Check for events */
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{	
					done = 1;
				} 
				else if (event.key.keysym.sym == SDLK_UP)
				{
					menuIndex = (menuIndex - 1 + m_menuCount) % m_menuCount;
					musicPlayer->playSound(sounds[SND_TOK]);
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					menuIndex = (menuIndex + 1) % m_menuCount;
					musicPlayer->playSound(sounds[SND_TOK]);
				}
				else if (event.key.keysym.sym == SDLK_p)
				{
					SDL_SaveBMP(m_screen, "screenshot.bmp");
				}
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					musicPlayer->playSound(sounds[SND_TIK]);
					if (menuIndex == -1)
					{
						fadeOut();
						done = 1;
					}
					else if (menuIndex == 0) // new game
					{
						fadeOut();
						OEMFGame * game = new OEMFGame(m_screen, m_execPath, m_screenWidth, m_screenHeight, m_screenBpp);
						game->run();
						//delete game;
						//drawIntro();
					}
					else if (menuIndex == 1) // level editor
					{
						fadeOut();
						OEMFLevelEdit * leveledit = new OEMFLevelEdit(m_screen, m_execPath, m_screenWidth, m_screenHeight, m_screenBpp);
						leveledit->run();
						//delete leveledit;
						//drawIntro();
					}
					else if (menuIndex == -1) // options
					{
						unsigned int choice = 0;
						static string options[2] = {"", ""};
						while (choice != 2) // not exit
						{
							if (musicEnabled)
								options[0] = "Turn Music Off";
							else
								options[0] = "Turn Music On";
							options[1] = "Toggle Fullscreen";
							drawIntro();
							choice = chooseList(2 /* exit */, "Options [broken]", options, 2, NULL);
							return;
							if (choice == 0)
							{
								musicEnabled = !musicEnabled;
								if (!musicEnabled)
								{
									if (musicPlayer->isPlayingSong())
										musicPlayer->stopSong();
								}
								else
								{
									musicPlayer->playSongAtPath(songfile.c_str());
								}
							}
							else if (choice == 1)
							{
								if (fullscreenMode)
								{
									m_screen = SDL_SetVideoMode(m_screenWidth, m_screenHeight, m_screenBpp, SDL_SWSURFACE);
									m_fb = (Uint32 *) m_screen->pixels;
									m_pitch = (Uint32) m_screen->pitch / 4;
									fullscreenMode = false;
								}
								else
								{
									m_screen = SDL_SetVideoMode(m_screenWidth, m_screenHeight, m_screenBpp, SDL_SWSURFACE | SDL_FULLSCREEN);
									m_fb = (Uint32 *) m_screen->pixels;
									m_pitch = (Uint32) m_screen->pitch / 4;
									fullscreenMode = true;
								}
								choice = 2;
							}
						}
						drawIntro();
					}
				}
				break;
			case SDL_QUIT:
				done = 1;
				break;
			default:
				break;
		}
	}
	frames++;
}

void OEMFMain :: run(void)
{
	unsigned int frames = 0;
	unsigned int timepassed;
	
	if (musicPlayer->isPlayingSong())
		musicPlayer->stopSong();
	musicPlayer->playSongAtPath(songfile.c_str());
	
	drawIntro();
	
	#ifdef EMSCRIPTEN
		oemf_env_instance.push(this);
	#else
		while (!done) {
			this->one_iter();
			SDL_Delay(33);
		}
	#endif
}
