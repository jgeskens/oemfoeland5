/*
 *  OEMFFontFactory.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 24/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>


#include "SDL.h"


#include "OEMFFontFactory.h"
#include "OEMFImage.h"
#include "OEMFEnvironment.h"

OEMFFontFactory :: OEMFFontFactory(OEMFImage * fontImage, unsigned int rangeFrom, unsigned int rangeTo, unsigned int charWidth, unsigned int charHeight, unsigned int charsPerRow)
{
	m_rangeFrom = rangeFrom;
	m_rangeTo = rangeTo;
	m_charWidth = charWidth;
	m_charHeight = charHeight;
	m_charsPerRow = charsPerRow;
	m_shadow = 0;
	m_chars = new OEMFImage*[rangeTo - rangeFrom + 1];
	unsigned int x = 0;
	unsigned int y = 0;
	for (unsigned int i = 0; i <= rangeTo - rangeFrom; i++)
	{
		m_chars[i] = fontImage->createSegment(x, y, charWidth, charHeight);
		x += charWidth;
		if (x == charsPerRow * charWidth)
		{
			y += charHeight;
			x = 0;
		}
	}
}

OEMFFontFactory :: ~OEMFFontFactory(void)
{
	for (unsigned int i = 0; i <= m_rangeTo - m_rangeFrom; i++)
		delete m_chars[i];
	delete [] m_chars;
}

void OEMFFontFactory :: blitText(OEMFEnvironment * env, string text, unsigned int left, unsigned int top, unsigned int width, bool lock)
{
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int totalHeight = m_charHeight;
	m_screen = env->Screen();
	
	for (unsigned int i = 0; i < text.size(); i++)
	{
		unsigned int c = text[i];
		if (c >= m_rangeFrom && c <= m_rangeTo && c != '\n')
		{
			env->blitImage(m_chars[c - m_rangeFrom], left + x, top + y);
		}
		else if (c == '\n')
		{
			x = width;
		}
		x += m_charWidth;
		if (x >= width)
		{
			y += m_charHeight;
			totalHeight = y + m_charHeight;
			x = 0;
		}
		
	}
	// SDL_UpdateRect(env->Screen(), left, top, width, m_charHeight);
}

void OEMFFontFactory :: blitText(OEMFEnvironment * env, string text, unsigned int color, unsigned int left, unsigned int top, unsigned int width, bool lock)
{
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int totalHeight = m_charHeight;
	m_screen = env->Screen();
	
	
	for (unsigned int i = 0; i < text.size(); i++)
	{
		unsigned int c = text[i];
		if (c >= m_rangeFrom && c <= m_rangeTo && c != '\n')
		{
			// shadow
			if (m_shadow > 0)
			{
				env->blitImage(m_chars[c - m_rangeFrom], 0xFF111111, 
					left + x + (unsigned int) m_shadow, 
					top + y + (unsigned int) m_shadow);
			}
			// real text
			env->blitImage(m_chars[c - m_rangeFrom], color, left + x, top + y);
		}
		else if (c == '\n')
		{
			x = width;
		}
		x += m_charWidth;
		if (x >= width)
		{
			y += m_charHeight;
			totalHeight = y + m_charHeight;
			x = 0;
		}
	}
	// SDL_UpdateRect(env->Screen(), left, top, width, m_charHeight);
}

void OEMFFontFactory :: blitCenterText(OEMFEnvironment * env, string text, unsigned int top, unsigned int screenWidth, bool lock)
{
	unsigned int textLeft = (screenWidth - text.size() * m_charWidth) / 2; 
	blitText(env, text, textLeft, top, screenWidth - textLeft, lock);
}

void OEMFFontFactory :: blitCenterText(OEMFEnvironment * env, string text, unsigned int color, unsigned int top, unsigned int screenWidth, bool lock)
{
	unsigned int textLeft = (screenWidth - text.size() * m_charWidth) / 2; 
	blitText(env, text, color, textLeft, top, screenWidth - textLeft, lock);
}

int OEMFFontFactory :: inputNumber(OEMFEnvironment * env, int def, unsigned int left, unsigned int top, unsigned int maxlen)
{
	char * defcstr = (char *) malloc(maxlen+32);
	
	sprintf(defcstr, "%d", def);
	string defstr = defcstr;
	string newstr = inputText(env, defstr, left, top, maxlen);
	int newint;
	sscanf(newstr.c_str(), "%d", &newint);
	return newint;
}

string OEMFFontFactory :: inputText(OEMFEnvironment * env, string def, unsigned int left, unsigned int top, unsigned int maxlen)
{
	bool done = false;
	unsigned int cursor = def.length();
	string newstring = def;
	m_screen = env->Screen();
	SDL_Event event;
	SDL_EnableUNICODE(1);
	while (!done)
	{
		// render text input field
		env->clearRectWithColor(left, top, charWidth() * (maxlen+1), charHeight() + 2, 0xFF000000, false);
		blitText(env, newstring.c_str(), left, top, (maxlen+1) * charWidth(), false);
		env->clearRectWithColor(left + charWidth() * cursor, top + charHeight(), charWidth(), 2, 0xFFFFFFFF, false);
		SDL_UpdateRect(m_screen, left, top, charWidth() * (maxlen+1), charHeight() + 2);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					newstring = def;
					done = true;
				}
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					done = true;
				}
				else if (event.key.keysym.sym == SDLK_LEFT)
				{
					if (cursor > 0)
					{
						cursor--;
					}
				}
				else if (event.key.keysym.sym == SDLK_RIGHT)
				{
					if (cursor < newstring.length())
					{
						cursor++;
					}
				}
				else if (event.key.keysym.sym == SDLK_BACKSPACE)
				{
					if (cursor > 0)
					{
						if (cursor < newstring.length())
						{
							 string leftpart = newstring.substr(0, cursor-1);
							 string rightpart = newstring.substr(cursor);
							 newstring = leftpart + rightpart;
							 cursor--;
						}
						else
						{
							newstring.resize(newstring.length()-1);
							cursor--;
						}
					}
				}
				else if (event.key.keysym.sym == SDLK_DELETE)
				{
					if (cursor < newstring.length())
					{
						 string leftpart = newstring.substr(0, cursor);
						 string rightpart = newstring.substr(cursor+1);
						 newstring = leftpart + rightpart;
					}
				}
				else
				{
					unsigned int code = event.key.keysym.unicode;
					if (code > 0 && code < 256 && newstring.length() < maxlen)
					{
						if (cursor < newstring.length())
						{
							 string leftpart = newstring.substr(0, cursor);
							 string rightpart = newstring.substr(cursor);
							 newstring = leftpart + (char) code + rightpart;
							 cursor++;
						} 
						else
						{
							newstring = newstring + (char) code;
							cursor++;
						}
					}
				}
			}
		}
	}
	SDL_EnableUNICODE(0);
	return newstring;
}

