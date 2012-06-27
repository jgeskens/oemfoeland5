/*
 *  Music.cpp
 *  MyWorld++
 *
 *  Created by Jef Geskens on 20/10/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdarg.h>

#include "MusicPlayer.h"
#include "OEMFStorage.h"

MusicPlayer :: MusicPlayer(void)
{
#ifndef WINCE
	if (-1 == Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024))
	{
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}
	Mix_AllocateChannels(16);
#endif
}

MusicPlayer :: ~MusicPlayer(void)
{
#ifndef WINCE
	if (isPlayingSong())
		stopSong();
	Mix_CloseAudio();
#endif
}

void MusicPlayer :: playSound(Mix_Chunk * sample)
{
#ifndef __DEBUG__
#ifndef WINCE
	// play sample on first free unreserved channel
	// play it exactly once through
	// Mix_Chunk *sample; //previously loaded
	Mix_PlayChannel(-1, sample, 0);
#endif
#endif
}

int MusicPlayer :: playSongAtPath(const char * filename)
{
#ifndef WINCE
	if (!musicEnabled)
		return 1;
	// load module
    m_module = Mix_LoadMUS((char *)filename);
	// play module
	Mix_PlayMusic(m_module, -1);
	return 1;
#endif
	return 1;
}

void MusicPlayer :: updateSong(void)
{
	
}

int MusicPlayer :: isPlayingSong(void)
{
#ifndef WINCE
	return Mix_PlayingMusic();
#else
	return 0;
#endif
}

void MusicPlayer :: stopSong(void)
{
#ifndef WINCE
	// Mix_HaltMusic(m_module)
	Mix_FadeOutMusic(1000);
	while (Mix_FadingMusic() == MIX_FADING_OUT)
		SDL_Delay(100);
	Mix_FreeMusic(m_module);
#endif
}


