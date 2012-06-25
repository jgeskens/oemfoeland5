/*
 *  OEMFEnvironment.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 24/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFENVIRONMENT_H__
#define __OEMFENVIRONMENT_H__


#include "SDL.h"

#include <string>

#include "MusicPlayer.h"
#include "OEMFImage.h"

using namespace std;

#define OEMF_LOCKSCREEN if (SDL_MUSTLOCK(m_screen)) if (SDL_LockSurface(m_screen) < 0) return
#define OEMF_UNLOCKSCREEN if (SDL_MUSTLOCK(m_screen)) SDL_UnlockSurface(m_screen)

class OEMFEnvironment
{
public:
	OEMFEnvironment(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp); 
	virtual ~OEMFEnvironment(void);
	
	SDL_Surface * Screen(void) const { return m_screen; };
	
	void blitImage(OEMFImage * image, unsigned int left, unsigned int top);
	void blitImage(OEMFImage * image, unsigned int color, unsigned int left, unsigned int top);
	void blitImageRect(OEMFImage * image, unsigned int left, unsigned int top, unsigned int sleft, unsigned int stop, unsigned int swidth, unsigned int sheight);

	void darkenRect(unsigned int left, unsigned int top, unsigned int width, unsigned int height, bool lock = true);
	void clearRectWithColor(int left, int top, unsigned int width, unsigned int height, unsigned int color, bool lock = true);
	void clearWithColor(unsigned int color, bool lock = true);
	void fadeOut(void);
	void ripEffect(void);
	void ripEffect(unsigned int top, unsigned int height);
	
	void updateScreen() { SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight); };
	void waitForKey();
	unsigned int chooseList(unsigned int index, string title, string * items, unsigned int itemCount);
	virtual void run(void);
protected:
	unsigned int m_screenWidth;
	unsigned int m_screenHeight;
	unsigned char m_screenBpp;
	SDL_Surface * m_screen;
	Uint32 * m_fb;
	Uint32 m_pitch;
	char * m_execPath;
};

#endif
