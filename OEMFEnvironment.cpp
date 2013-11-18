/*
 *  OEMFEnvironment.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 24/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */


#include "SDL.h"

#include <iostream>

#include "OEMFStorage.h"
#include "OEMFFontFactory.h"
#include "MusicPlayer.h"
#include "OEMFString.h"
#include "OEMFEnvironment.h"


OEMFEnvironment * oemf_env_instance = NULL;


OEMFEnvironment :: OEMFEnvironment(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp)
{
	m_screen = screen;
	m_execPath = execPath;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_screenBpp = screenBpp;
	
	m_fb = (unsigned int *) screen->pixels;
	m_pitch = (unsigned int) screen->pitch / 4; // TODO: check this with 8bit and 16bit displays
}


OEMFEnvironment :: ~OEMFEnvironment(void)
{
}


void OEMFEnvironment :: blitImage(OEMFImage * image, unsigned int left, unsigned int top)
{
	SDL_Rect rect;
	rect.x = left;
	rect.y = top;
	SDL_BlitSurface(image->getSurface(), NULL, m_screen, &rect);
}

void OEMFEnvironment :: blitImage(OEMFImage * image, unsigned int color, unsigned int left, unsigned int top)
{
	// This is TOO SLOW for Browsers!!!
	this->blitImage(image, left, top);
	return;

	OEMF_LOCKSCREEN;
	if (SDL_MUSTLOCK(image->getSurface())) SDL_LockSurface(image->getSurface());
	Uint32 * pixels = (Uint32 *) image->getSurface()->pixels;
	Uint32 pitch = (Uint32) image->getSurface()->pitch / 4;
	for (Uint32 x = 0; x < image->width(); ++x)
	{
		for (Uint32 y = 0; y < image->height(); ++y)
		{
			Uint32 fromPixel = pixels[x + pitch * y];
			if (left + x < m_screenWidth && top + y < m_screenHeight)
			{
				if (fromPixel == 0xFFFFFF)
				{
					m_fb[left + x + (top + y) * m_pitch] = color;
				}
				else if (fromPixel != 0x000000)
				{
					m_fb[left + x + (top + y) * m_pitch] = fromPixel;
				}
			}
		}
	}
	if (SDL_MUSTLOCK(image->getSurface())) SDL_UnlockSurface(image->getSurface());
	OEMF_UNLOCKSCREEN;
}

void OEMFEnvironment :: blitImageRect	(OEMFImage * image, unsigned int left, unsigned int top, unsigned int sleft, 
										unsigned int stop, unsigned int swidth, unsigned int sheight)
{
	SDL_Rect srcRect, dstRect;
	srcRect.x = sleft;
	srcRect.y = stop;
	srcRect.w = swidth;
	srcRect.h = sheight;
	dstRect.x = left;
	dstRect.y = top;
	SDL_BlitSurface(image->getSurface(), &srcRect, m_screen, &dstRect);
}

void OEMFEnvironment :: darkenRect(unsigned int left, unsigned int top, unsigned int width, unsigned int height, bool lock)
{
	if (lock)
	{
		OEMF_LOCKSCREEN;
	}
	unsigned char r, g, b;
	unsigned int x, y, ypos;
	for (y = top; y < top + height && y < m_screenHeight; y++)
	{
		ypos = m_pitch * y;
		for (x = left; x < left + width && x < m_screenWidth; x++)
		{
			r = (unsigned char) ((float) ((m_fb[ypos + x] & 0x00FF0000) >> 16) * 0.5f);
			g = (unsigned char) ((float) ((m_fb[ypos + x] & 0x0000FF00) >> 8) * 0.5f);
			b = (unsigned char) ((float) ((m_fb[ypos + x] & 0x000000FF) >> 0) * 0.5f);
			m_fb[ypos + x] = r << 16 | g << 8 | b;
		}
	} 
	
	if (lock)
	{
		OEMF_UNLOCKSCREEN;
		SDL_UpdateRect(m_screen, left, top, width, height);
	}
}

void OEMFEnvironment :: clearRectWithColor(int left, int top, unsigned int width, unsigned int height, unsigned int color, bool lock)
{
	if (lock)
	{
		OEMF_LOCKSCREEN;
	}
	SDL_Rect rect;
	rect.x = left;
	rect.y = top;
	rect.w = width;
	rect.h = height;
	SDL_FillRect(m_screen, &rect, color);
	if (lock)
	{
		OEMF_UNLOCKSCREEN;
		SDL_UpdateRect(m_screen, left, top, width, height);
	}
}

void OEMFEnvironment :: clearWithColor(unsigned int color, bool lock)
{
	if (lock)
	{
		OEMF_LOCKSCREEN;
	}
	SDL_FillRect(m_screen, NULL, color);
	if (lock)
	{
		OEMF_UNLOCKSCREEN;
	}
}

void OEMFEnvironment :: fadeOut(void)
{
#ifdef __DEBUG__
	// in debug mode, please don't fade, a waste of time ;-)
	clearWithColor(0x000000, false);
	SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight);
	return;
#endif
	float alpha; 
	unsigned int x, y, ypos;
	unsigned char r, g, b;
	for (int i = 16; i >= 0; i--)
	{
		alpha = (float) i / 16.0f;
		OEMF_LOCKSCREEN;
		
		for (y = 0; y < m_screenHeight; y++)
		{
			ypos = y * m_pitch;
			for (x = 0; x < m_screenWidth; x++)
			{
				r = (unsigned char) ((float) ((m_fb[ypos + x] & 0x00FF0000) >> 16) * alpha);
				g = (unsigned char) ((float) ((m_fb[ypos + x] & 0x0000FF00) >> 8) * alpha);
				b = (unsigned char) ((float) ((m_fb[ypos + x] & 0x000000FF) >> 0) * alpha);
				m_fb[ypos + x] = r << 16 | g << 8 | b;
			}
		}
		
		OEMF_UNLOCKSCREEN;
		SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight);
	}
}

void OEMFEnvironment :: ripEffect(void)
{
	ripEffect(0, m_screenHeight);
}

void OEMFEnvironment :: ripEffect(unsigned int top, unsigned int height)
{
	unsigned int x, y, ypos, j;
	for (unsigned int i = 0; i < m_screenWidth / 4; i++)
	{
		OEMF_LOCKSCREEN;
		
		for (j = 0; j < 4; j++)
		{
			for (y = top; y < top + height; y++)
			{
				ypos = y * m_pitch;
				if (y % 2)
				{
					for (x = 1; x < m_screenWidth; x++)
					{
						m_fb[ypos + x - 1] = m_fb[ypos + x];
						m_fb[ypos + x] = 0;
					}
				}
				else
				{
					for (x = m_screenWidth-1; x > 0; x--)
					{
						m_fb[ypos + x] = m_fb[ypos + x - 1];
						m_fb[ypos + x - 1] = 0;
					}
				}
			}
		}
		
		OEMF_UNLOCKSCREEN;
		updateScreen();
	}
}

unsigned int OEMFEnvironment :: chooseList(unsigned int index, string title, string * items, unsigned int itemCount)
{
	SDL_Surface * backgroundSrf = SDL_CreateRGBSurface(SDL_SWSURFACE, m_screen->w, m_screen->h, 
		m_screen->format->BitsPerPixel,
		m_screen->format->Rmask,
		m_screen->format->Gmask,
		m_screen->format->Bmask,
		m_screen->format->Amask);
	SDL_BlitSurface(m_screen, NULL, backgroundSrf, NULL);
	
	OEMFFontFactory * font = fonts[FNT_AMIGA];
	unsigned int newIndex = index;
	if (newIndex >= itemCount)
		newIndex = 0;
	
	unsigned int offset = 0;
	
	SDL_Event event;
	int done = 0;
	while (!done)
	{
		SDL_Delay(50);
		SDL_BlitSurface(backgroundSrf, NULL, m_screen, NULL);
		darkenRect(160, 128, 320, 64 + 16 * (itemCount <= 10 ? itemCount + 1 : 11), false);
		font->blitCenterText(this, title.c_str(), 0x007FFF, 128 + 16, m_screenWidth, false);
		
		while (newIndex >= offset + 10)
			offset += 10;
		while (newIndex < offset)
			offset -= 10;
		for (unsigned int i = offset; i < itemCount && i - offset < 10; i++)
		{
			if (offset + 10 < itemCount)
				font->blitText(this, string("vvv Next Page vvv"), 0xFF7700, 184, 128 + 48 + 16 * 10, m_screenWidth, false);
			if (offset > 0)
				font->blitText(this, string("^^^ Previous Page ^^^"), 0xFF7700, 184, 128 + 32, m_screenWidth, false);
			if (i == newIndex)
			{	
				font->blitText(this, (string("> ") + items[i]).c_str(), 0xFFFF00, 168, 128 + 48 + 16 * (i%10), m_screenWidth, false);
			}
			else
				font->blitText(this, items[i].c_str(), 0xFFFFFF, 184, 128 + 48 + 16 * (i%10), m_screenWidth, false);
		}
		
		SDL_UpdateRect(m_screen, 160, 128, 320, 64 + 16 * itemCount);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_UP)
				{
					newIndex = (newIndex - 1 + itemCount) % itemCount;
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					newIndex = (newIndex + 1) % itemCount;
				}
				else if (event.key.keysym.sym == SDLK_PAGEUP)
				{
					if (newIndex >= 10) 
						newIndex -= 10;
				}
				else if (event.key.keysym.sym == SDLK_PAGEDOWN)
				{
					if (newIndex + 10 < itemCount) 
						newIndex += 10;
				}
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					done = 1;
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					newIndex = index;
					done = 1;
				}
			}
		}
	}
	
	SDL_FreeSurface(backgroundSrf);
	return newIndex;
}

void OEMFEnvironment :: waitForKey()
{
	SDL_Event event;
	int done = 0;
	while (!done) 
	{
		SDL_Delay(50);
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
				case SDL_KEYDOWN:
					done = 1;
					break;
				case SDL_QUIT:
					exit(0);
					break;
			}
		}
	}
}

void OEMFEnvironment :: run(void)
{
	if (musicPlayer->isPlayingSong())
		musicPlayer->stopSong();
	OEMF_LOCKSCREEN;
	clearWithColor(0x000000, false);
	OEMF_UNLOCKSCREEN;
	SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight);
	SDL_Event event;
	int done = 0;
	while ( !done) {
		/* Check for events */
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{	
						done = 1;
					}
					break;
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
	}
}

void OEMFEnvironment :: one_iter(void)
{
}

