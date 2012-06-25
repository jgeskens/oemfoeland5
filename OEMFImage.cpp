/*
 *  OEMFImage.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 21/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <string.h>
#include <iostream>
#include <fstream>

#include "OEMFImage.h"

using namespace std;

OEMFImage :: OEMFImage(void)
{
	m_width = 0;
	m_height = 0;
	m_length = 0;
	m_data = NULL;
	m_surface = NULL; 
	next = NULL;
}

OEMFImage :: OEMFImage(const OEMFImage * image)
{
	m_width = 0;
	m_height = 0;
	m_length = 0;
	m_data = NULL;
	m_surface = NULL; 
	next = NULL;
	
	if (m_surface != NULL)
		SDL_FreeSurface(m_surface);
	if (image == NULL || image->m_surface == NULL)
		return;
	
	m_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, image->m_surface->w, image->m_surface->h, 32, 
										0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	SDL_BlitSurface(image->m_surface, NULL, m_surface, NULL);
}


OEMFImage :: OEMFImage(char * filename, bool hasAlpha)
{
	// load image
	m_width = 0;
	m_height = 0;
	m_length = 0;
	m_data = NULL;
	next = NULL;
	if (!filename)
	{
		m_surface = NULL;
		return;
	}
	
	SDL_Surface * image = IMG_Load(filename);
	if (image == NULL) 
	{
		std::cout << "File not found: " << filename << std::endl;
		SDL_Quit();
		return;
	}
	
	if (hasAlpha)
	{	
		m_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 
											32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000); // alpha
	}
	else
	{	
		m_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 
											32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000); // geen alpha (0)
		SDL_SetColorKey(m_surface, SDL_RLEACCEL | SDL_SRCCOLORKEY, 0x0);
	}
	
	SDL_BlitSurface(image, NULL, m_surface, NULL);
	SDL_FreeSurface(image);
}

OEMFImage :: ~OEMFImage(void)
{
	if (m_surface != NULL)
	{
		SDL_FreeSurface(m_surface);
	}
}

void OEMFImage :: resize(unsigned int width, unsigned int height) // TODO: fixen
{
	if (m_surface == NULL) 
		return;
	SDL_Surface * resized = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, m_surface->format->Amask);
	Uint32 * pixels = (Uint32 *) m_surface->pixels;
	Uint32 pitch = (Uint32) m_surface->pitch / (32 / 8);
	Uint32 * pixelsr = (Uint32 *) resized->pixels;
	Uint32 pitchr = (Uint32) resized->pitch / (32 / 8);
	for (int x = 0; x < resized->w; ++x)
	{
		for (int y = 0; y < resized->h; ++y)
		{
			int origX = (int) ((float) x / (float) resized->w * (float) m_surface->w);
			int origY = (int) ((float) y / (float) resized->h * (float) m_surface->h);
			// testen voor afrondingsfouten
			if (origX >= 0 && origX < m_surface->w &&
				origY >= 0 && origY < m_surface->h)
			{
				pixelsr[x + pitchr * y] = pixels[origX + pitch * origY];
			}
		}
	}
	if (m_surface->format->Amask == 0)
		SDL_SetColorKey(resized, SDL_RLEACCEL | SDL_SRCCOLORKEY, 0);
	SDL_FreeSurface(m_surface);
	m_surface = resized;
}

OEMFImage * OEMFImage :: createSegment(int left, int top, unsigned int width, unsigned int height)
{
	if (m_surface == NULL) 
		return NULL;
	
	OEMFImage * segmentImg = new OEMFImage();
	SDL_Surface * segment = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 
													0x00FF0000, 0x0000FF00, 0x000000FF, m_surface->format->Amask);
	SDL_Rect srcRect;
	srcRect.x = left;
	srcRect.y = top;
	srcRect.w = width;
	srcRect.h = height;
	SDL_BlitSurface(m_surface, &srcRect, segment, NULL);
	if (m_surface->format->Amask == 0)
		SDL_SetColorKey(segment, SDL_RLEACCEL | SDL_SRCCOLORKEY, 0);
	segmentImg->m_surface = segment;
	return segmentImg;
}