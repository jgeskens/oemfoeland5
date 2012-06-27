/*
 *  OEMFImage.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 21/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include "SDL.h"
#include "SDL_image.h"

#ifndef __OEMFIMAGE_H__
#define __OEMFIMAGE_H__

#ifdef WINCE
#define IMG_Load SDL_LoadBMP
#endif

class OEMFImage
{
public:
	OEMFImage(void); // default constructor
	OEMFImage(const OEMFImage * image); // pointer copy constructor
	OEMFImage(const char * filename, bool hasAlhpha = false); // tga load constructor
	~OEMFImage(void); // destructor
	
	// setters
	void resize(unsigned int width, unsigned int height);
	
	// getters
	unsigned int length(void) const { return m_length; }; // NOT WORKING ANYMORE
	unsigned char * data(void) const { return m_data; }; // NOT WORKING ANYMORE
	unsigned int width(void) const { return m_surface->w; };
	unsigned int height(void) const { return m_surface->h; };
	OEMFImage * createSegment(int left, int top, unsigned int width, unsigned int height);
	SDL_Surface * getSurface() const { return m_surface; };
	OEMFImage * next; // for animations
protected:
	unsigned int m_width;
	unsigned int m_height;
	unsigned char * m_data;
	unsigned int m_length;
	SDL_Surface * m_surface;
};

#endif
