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

#include "EventLoopStack.h"

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

void (* inputNumberIntCB)(int);
void inputNumberCB(string newstr){
	int newint;
	sscanf(newstr.c_str(), "%d", &newint);
	inputNumberIntCB(newint);
}

void OEMFFontFactory :: inputNumber(OEMFEnvironment * env, int def, unsigned int left, unsigned int top, unsigned int maxlen, void (* callback)(int))
{
	char * defcstr = (char *) malloc(maxlen+32);
	sprintf(defcstr, "%d", def);
	string defstr = defcstr;
	inputNumberIntCB = callback;
	inputText(env, defstr, left, top, maxlen, inputNumberCB);
}


struct inputTextHandle * inputTextHandleInstance = NULL;
struct inputTextHandle
{
	OEMFFontFactory * font;
	OEMFEnvironment * env;
	string def;
	unsigned int left;
	unsigned int top;
	unsigned int maxlen;
	void (* callback)(string);

	unsigned int cursor;
	string newstring;

	inputTextHandle(OEMFFontFactory * font, OEMFEnvironment * env, string def, unsigned int left, unsigned int top, unsigned int maxlen, void (* callback)(string))
	{
		this->font = font;
		this->env = env;
		this->def = def;
		this->left = left;
		this->top = top;
		this->maxlen = maxlen;
		this->callback = callback;

		cursor = def.length();
		newstring = def;
		font->m_screen = env->Screen();
		SDL_EnableUNICODE(1);

		inputTextHandleInstance = this;
		EVENT_LOOP_STACK.push(loop);
	};

	void end(void)
	{
		SDL_EnableUNICODE(0);
		EVENT_LOOP_STACK.pop();
		inputTextHandleInstance = NULL;
		delete this;
	};

	void one_iter(void)
	{
		SDL_Event event;
		// render text input field
		env->clearRectWithColor(left, top, font->charWidth() * (maxlen+1), font->charHeight() + 2, 0xFF000000, false);
		font->blitText(env, newstring.c_str(), left, top, (maxlen+1) * font->charWidth(), false);
		env->clearRectWithColor(left + font->charWidth() * cursor, top + font->charHeight(), font->charWidth(), 2, 0xFFFFFFFF, false);
		SDL_UpdateRect(font->m_screen, left, top, font->charWidth() * (maxlen+1), font->charHeight() + 2);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					end();
				}
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					void (* cb)(string) = callback;
					if (cb)
						cb(newstring);
					end();
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
	};

	static void loop(void){
		if (inputTextHandleInstance != NULL)
			inputTextHandleInstance->one_iter();
	};
};


void OEMFFontFactory :: inputText(OEMFEnvironment * env, string def, unsigned int left, unsigned int top, unsigned int maxlen, void (* callback)(string))
{
	new struct inputTextHandle(this, env, def, left, top, maxlen, callback);
}

