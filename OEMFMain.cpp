/*
 *  OEMFMain.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <math.h>

#include "OEMFMain.h"
#include "OEMFStorage.h"
#include "OEMFString.h"
#include "OEMFLevelEdit.h"
#include "OEMFGame.h"

OEMFMain :: OEMFMain(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp)
	: OEMFEnvironment(screen, execPath, screenWidth, screenHeight, screenBpp)
{
	m_menuOptions = new string[5];
	m_menuCount = 5;
	m_menuOptions[0] = "NEW GAME";
	m_menuOptions[1] = "HIGH SCORES";
	m_menuOptions[2] = "OPTIONS";
	m_menuOptions[3] = "LEVEL EDITOR";
	m_menuOptions[4] = "EXIT";
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

void OEMFMain :: run(void)
{
	unsigned int menuIndex = 0;
	unsigned int frames = 0;
	unsigned int timepassed;
	
	if (musicPlayer->isPlayingSong())
		musicPlayer->stopSong();
	string songfile = PREPATH "boiling.mp3";
	musicPlayer->playSongAtPath(songfile.c_str());
	
	drawIntro();
	
	SDL_Event event;
	int done = 0;
	while ( !done) {
		timepassed = SDL_GetTicks();
	
		OEMF_LOCKSCREEN;
		clearWithColor(0xFF000000, false);
		OEMF_UNLOCKSCREEN;
		blitImage(images[IMG_INTRO], 0, 0);
		for (unsigned int i = 0; i < m_menuCount; i++)
			fonts[FNT_MENU]->blitText(this, m_menuOptions[i].c_str(), 160, 240 + i * 32, 480, false);
		blitImage(images[IMG_OEMFOE], 128, (unsigned int) (240 + menuIndex * 32 + sin(frames / 2.0) * 8));
		SDL_UpdateRect(m_screen, 128, 232, 512, 248);
		
		/* Check for events */
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
						if (menuIndex == 4)
						{
							fadeOut();
							done = 1;
						}
						else if (menuIndex == 0) // new game
						{
							fadeOut();
							OEMFGame * game = new OEMFGame(m_screen, m_execPath, m_screenWidth, m_screenHeight, m_screenBpp);
							game->run();
							delete game;
							drawIntro();
						}
						else if (menuIndex == 3) // level editor
						{
							fadeOut();
							OEMFLevelEdit * leveledit = new OEMFLevelEdit(m_screen, m_execPath, m_screenWidth, m_screenHeight, m_screenBpp);
							leveledit->run();
							delete leveledit;
							drawIntro();
						}
						else if (menuIndex == 2) // options
						{
							unsigned int choice = 0;
							string * options = new string[2];
							while (choice != 2) // not exit
							{
								if (musicEnabled)
									options[0] = "Turn Music Off";
								else
									options[0] = "Turn Music On";
								options[1] = "Toggle Fullscreen";
								drawIntro();
								choice = chooseList(2 /* exit */, "Options", options, 2);
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
						else
						{
							fadeOut();
							fonts[FNT_MENU]->blitCenterText(this, "NOT AVAILABLE YET", 240, m_screenWidth);
							SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight);
							SDL_Delay(2000);
							string test = fonts[FNT_MENU]->inputText(this, "default", 32, 32, 10);
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
		
		// 30 FPS
		timepassed = SDL_GetTicks() - timepassed;
		if (timepassed <= 33)
			SDL_Delay(33 - timepassed);
	}
}
