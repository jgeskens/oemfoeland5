/*
 *  Music.h
 *  MyWorld++
 *
 *  Created by Jef Geskens on 20/10/05.
 *  Copyright 2005-2006 Jef Geskens. All rights reserved.
 *
 */

#ifndef __MUSICPLAYER_H__
#define __MUSICPLAYER_H__

#ifndef WINCE
#include "SDL_mixer.h"
#else
#define Mix_Music int
#define Mix_Chunk int
#endif

class MusicPlayer
{
public:
	MusicPlayer(void);
	~MusicPlayer(void);
	void playSound(Mix_Chunk * sample);
	int playSongAtPath(const char * filename);
	void updateSong(void);
	int isPlayingSong(void);
	void stopSong(void);
private:
	Mix_Music * m_module;
};

#endif



