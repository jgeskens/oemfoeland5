/*
 *  OEMFFontFactory.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 24/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFFONTFACTORY_H__
#define __OEMFFONTFACTORY_H__

#include "SDL.h"


#include "OEMFImage.h"
#include "OEMFEnvironment.h"

using namespace std;

class OEMFFontFactory
{
public:
	OEMFFontFactory(OEMFImage * fontImage, unsigned int rangeFrom, unsigned int rangeTo, unsigned int charWidth, unsigned int charHeight, unsigned int charsPerRow);
	~OEMFFontFactory(void);
	
	void blitText(OEMFEnvironment * env, string text, unsigned int left, unsigned int top, unsigned int width, bool lock = true);
	void blitText(OEMFEnvironment * env, string text, unsigned int color, unsigned int left, unsigned int top, unsigned int width, bool lock = true);
	void blitCenterText(OEMFEnvironment * env, string text, unsigned int top, unsigned int screenWidth, bool lock = true);
	void blitCenterText(OEMFEnvironment * env, string text, unsigned int color, unsigned int top, unsigned int screenWidth, bool lock = true);
	
	void inputNumber(OEMFEnvironment * env, int def, unsigned int left, unsigned int top, unsigned int maxlen, void (* callback)(int));
	void inputText(OEMFEnvironment * env, string def, unsigned int left, unsigned int top, unsigned int maxlen, void (* callback)(string));

	unsigned int charWidth(void) const { return m_charWidth; };
	unsigned int charHeight(void) const { return m_charHeight; };
	
	void setShadow(char offset) { m_shadow = offset; };

	OEMFImage ** m_chars;
	SDL_Surface * m_screen;
	unsigned int m_rangeFrom;
	unsigned int m_rangeTo;
	unsigned int m_charWidth;
	unsigned int m_charHeight;
	unsigned int m_charsPerRow;
	char m_shadow;
};

#endif
