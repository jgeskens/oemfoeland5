/*
 *  OEMFLevelEdit.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <iostream>
#include <stdio.h>
#include <deque>

#include "OEMFLevelEdit.h"
#include "OEMFStorage.h"
#include "OEMFGame.h"

OEMFLevelEdit :: OEMFLevelEdit(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp)
	: OEMFEnvironment(screen, execPath, screenWidth, screenHeight, screenBpp)
{
	unsigned int k;
	
	m_level = new OEMFLevel("Untitled Level", 20, 14);
	m_filename = PREPATH "level1.lvl";
	
	m_imageCount = (unsigned int) le_tiles.size();
	m_images = new unsigned int[m_imageCount];
	for (k = 0; k < m_imageCount; ++k)
		m_images[k] = le_tiles[k];
	
	m_currentImage = IMG_OEMFOE;
	m_background = false;
	
	m_typeListCount = TYPECOUNT;
	m_typeList = new string[m_typeListCount];
	k = 0;
	m_typeList[k] = " 0 Free, non-blocking"; k++;
	m_typeList[k] = " 1 Wall, blocking"; k++;
	m_typeList[k] = " 2 Platform, block downwards"; k++;
	m_typeList[k] = " 3 Movable block"; k++;
	m_typeList[k] = " 4 Coin 100"; k++;
	m_typeList[k] = " 5 End Level"; k++;
	m_typeList[k] = " 6 Start Position (1x)"; k++;
	m_typeList[k] = " 7 Unstable"; k++;
	m_typeList[k] = " 8 Deadly"; k++;
	m_typeList[k] = " 9 Left Force  (<--)"; k++;
	m_typeList[k] = "10 Right Force (-->)"; k++;
	m_typeList[k] = "11 Left Conveyor Belt (<--)"; k++;
	m_typeList[k] = "12 Right Conveyor Belt (-->)"; k++;
	m_typeList[k] = "13 Up force (^)"; k++;
	m_currentType = 0;
	m_beginX = 0;
	m_beginY = 0;
	m_posX = 0;
	m_posY = 0;
	m_mouseX = 0;
	m_mouseY = 0;
	m_originX = 0;
	m_originY = 0;
	m_drawRect = false;
	m_showTypes = false;
}

OEMFLevelEdit :: ~OEMFLevelEdit(void)
{
	delete [] m_images;
	delete [] m_typeList;
	delete m_level;
}

void OEMFLevelEdit :: chooseBackground()
{
	string * bglist = new string[2];
	bglist[0] = "Brown/dark clouds";
	bglist[1] = "Blue clouds";
	unsigned int bg = chooseList(0, string("Choose a background image:"), bglist, 2);
	if (bg == 0)
		m_level->setBackground(IMG_CLOUDS1);
	else if (bg == 1)
		m_level->setBackground(IMG_CLOUDS2);
}

unsigned int OEMFLevelEdit :: chooseTile(unsigned int index)
{
	unsigned int listIndex = 0;
	unsigned int imageIndex = 0;
	unsigned int beginIndex = 0;
	// select right index
	for (unsigned int i = 0; i < m_imageCount; i++)
		if (m_images[i] == index)
			listIndex = i;
	
	int done = 0;
	SDL_Event event;
	while (!done)
	{
		// calculate beginindex (for scrolling)
		while ((int) listIndex - (int) beginIndex < 0 && beginIndex >= 0)
		{
			beginIndex--;
		}
		while (((int) listIndex - (int) beginIndex + 1) * 32 > (int) m_screenWidth && beginIndex < m_screenWidth / 32)
		{
			beginIndex++;
		}
		
		clearRectWithColor(0, m_screenHeight - 32, m_screenWidth, 32, 0x000000, false);
		unsigned int p = 0;
		for (unsigned int i = beginIndex; i < m_imageCount && (p + 1) * 32 <= m_screenWidth; i++)
		{
			blitImage(images[m_images[i]], p * 32, m_screenHeight - 32);
			p++;
		}
		p = listIndex - beginIndex;
		if (listIndex >= beginIndex && (p + 1) * 32 <= m_screenWidth)
			blitImage(images[IMG_SELECTION], p * 32, m_screenHeight - 32);
		
		SDL_UpdateRect(m_screen, 0, m_screenHeight - 32, m_screenWidth, 32);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{	
						imageIndex = index;
						done = 1;
					}
					else if (event.key.keysym.sym == SDLK_LEFT)
					{
						listIndex = (listIndex - 1 + m_imageCount) % m_imageCount;
					}
					else if (event.key.keysym.sym == SDLK_RIGHT)
					{
						listIndex = (listIndex + 1) % m_imageCount;
					}
					else if (event.key.keysym.sym == SDLK_RETURN)
					{
						imageIndex = m_images[listIndex];
						done = 1;
					}
				break;
			}
		}
	}
	return imageIndex;
}

void OEMFLevelEdit :: drawInterface(unsigned int left, unsigned int top, unsigned int width, unsigned int height)
{
	OEMFFontFactory * font = fonts[FNT_AMIGA];
	unsigned int levelwidth = m_level->width();
	unsigned int levelheight = m_level->height();
	
	// first, calculate beginX and beginY according to the cursor position
	while ((int) m_posX - (int) m_beginX < 0)
	{
		m_beginX--;
	}
	while (((int) m_posX - (int) m_beginX + 1) * 32 > (int) m_screenWidth)
	{
		m_beginX++;
	}
	while ((int) m_posY - (int) m_beginY < 0)
	{
		m_beginY--;
	}
	while (((int) m_posY - (int) m_beginY + 1) * 32 > (int) m_screenHeight - 64)
	{
		m_beginY++;
	}
	
	// clear screen
	clearWithColor(0x000000, false);
	
	// top bar
	font->blitText(this, ("Level Edit '" + m_level->name() + "' Change: [N]ame [I]ndex [R]esize"), 0xFFFF00, 0, 0, 640, false);
	font->blitText(this, "[B]ackground [S]elect icon [T]ype [E]yedrop [F]ill [L]oad [W]rite [ESC] exit", 0x007FFF, 0, 16, 640, false);
	
	// background
	blitImageRect(images[m_level->background()], 0, 32, 0, 0, m_screenWidth, m_screenHeight-64);	
	
	// level
	unsigned int x, y, ypos;
	char typestring[4];
	for (y = m_beginY; y - m_beginY < 13 && y < m_level->height(); y++)
	{
		ypos = y * m_level->width();
		for (x = m_beginX; x - m_beginX < 20 && x < m_level->width(); x++)
		{
			tile_t curtile = m_level->tiles()[x + ypos];
			
			if (curtile.bg < IMAGECOUNT)
			{
				blitImage(images[curtile.bg], (x - m_beginX) * 32, (y - m_beginY) * 32 + 32);
				
			}
			if (curtile.fg < IMAGECOUNT)
			{
				if (m_background)
					SDL_SetAlpha(images[curtile.fg]->getSurface(), SDL_SRCALPHA | SDL_RLEACCEL, 128);
				blitImage(images[curtile.fg], (x - m_beginX) * 32, (y - m_beginY) * 32 + 32);
				if (m_background)
				{
					SDL_SetAlpha(images[curtile.fg]->getSurface(), SDL_RLEACCEL, 255);
					SDL_SetColorKey(images[curtile.fg]->getSurface(), SDL_RLEACCEL | SDL_SRCCOLORKEY, 0x0);
				}
				if (m_showTypes && curtile.type > 0)
				{
					sprintf(typestring, "%d", curtile.type);
					font->blitText(this, typestring, 0xFFFFFF, (x - m_beginX) * 32, (y - m_beginY) * 32 + 32, 32, false);
				}
			}
		}
	}
	
	// selection
	blitImage(images[IMG_SELECTION], (m_posX - m_beginX) * 32, (m_posY - m_beginY) * 32 + 32);

	// fill rectangle
	if (m_drawRect)
	{
		for (unsigned int rX = (m_originX - m_beginX); rX <= (m_posX - m_beginX); ++rX)
			for (unsigned int rY = (m_originY - m_beginY); rY <= (m_posY - m_beginY); ++rY)
				blitImage(images[IMG_SELECTION], rX * 32, rY * 32 + 32);
	}

	// current icon
	blitImage(images[m_currentImage], 0, m_screenHeight-32);
	font->blitText(this, "=current icon", 0xFF0000, 32, m_screenHeight-32, m_screenWidth-32, false);
	
	// position
	char positionX[32];
	char positionY[32];
	sprintf(positionX,"X%3d/%3d", m_posX, levelwidth-1);
	sprintf(positionY,"Y%3d/%3d", m_posY, levelheight-1);
	font->blitText(this, positionX, 0x009900, m_screenWidth-64, m_screenHeight-32, 64, false);
	font->blitText(this, positionY, 0x00CC00, m_screenWidth-64, m_screenHeight-16, 64, false);
	
	// background or foreground mode
	if (m_background)
		font->blitText(this, "Background Mode     [Y] change Background image", 0x007FFF, 32, m_screenHeight-16,m_screenWidth-32, false);
	else
	{
		font->blitText(this, "Foreground Mode", 0x007FFF, 32, m_screenHeight-16,m_screenWidth-32, false);
		// tile type
		font->blitText(this, m_typeList[m_currentType], 0xFFFF00, 32+16*8,m_screenHeight-32,m_screenWidth-(32+16*8), false);
		font->blitText(this, m_typeList[m_level->getTileType(m_posX, m_posY)],0xFF0000,32+16*8,m_screenHeight-16,m_screenWidth-(32+16*8), false);
		
		// show types
		if (m_showTypes)
			font->blitText(this, "[U] hide types", 0xFFFF00, m_screenWidth-192, m_screenHeight-16, 128, false);
		else
			font->blitText(this, "[U] show types", 0xFFFF00, m_screenWidth-192, m_screenHeight-16, 128, false);
	}
	
	// play level
	font->blitText(this, "[P]lay level", 0x00CC00, m_screenWidth-192, m_screenHeight-32, 128, false);
	
	blitImage(images[IMG_CURSOR], m_mouseX, m_mouseY);
	
	updateScreen();
	
	// animate all the animations
	animateImages();
}

void OEMFLevelEdit :: resizeDialog(void)
{
	OEMFFontFactory * font = fonts[FNT_AMIGA];
	unsigned int newwidth = m_level->width();
	unsigned int newheight = m_level->height();
	char originalpos[32];
	sprintf(originalpos, "%d\n%d", newwidth, newheight);

	darkenRect(160, 128, 320, 96, true);
	font->blitCenterText(this, "Resize Level", 0x007FFF, 128+16, m_screenWidth, false);
	font->blitText(this, "New width:", 0xFFFF00, 160+16, 128+16+32, 12*font->charWidth(), false);
	font->blitText(this, "New height:", 0xFFFF00, 160+16, 128+16+48, 12*font->charWidth(), false);
	font->blitText(this, originalpos, 0xFFFFFF, 160+16+12*font->charWidth(), 128+16+32, 12*font->charWidth(), false);

	SDL_UpdateRect(m_screen, 160, 128, 320, 96);
	newwidth = (unsigned int) font->inputNumber(this, newwidth, 160+16+12*font->charWidth(), 128+16+32, 3);
	sprintf(originalpos, "%d\n%d", newwidth, newheight);
	font->blitText(this, originalpos, 0xFFFFFF, 160+16+12*font->charWidth(), 128+16+32, 12*font->charWidth(), true);
	newheight = (unsigned int) font->inputNumber(this, newheight, 160+16+12*font->charWidth(), 128+16+48, 3);
	// actual resize
	m_level->resize(newwidth, newheight);
}

void OEMFLevelEdit :: importLevel()
{
	char buffer[1024];
	int levelNum = 0;
	FILE * fh = NULL;
	do
	{
		levelNum++;
		sprintf(buffer, PREPATH "level%d.lvl", levelNum);
		if (fh != NULL) 
			fclose(fh);
	} while (fh = fopen(buffer, "r"));
	if (fh != NULL) 
		fclose(fh);
	levelNum--;
	string * fileList = new string[levelNum];
	for (int i = 0; i < levelNum; ++i)
	{
		sprintf(buffer, "Level %03d", i+1);
		fileList[i] = buffer;
	}
	
	int levelno;
	sscanf(m_filename.c_str(), PREPATH "level%d.lvl", &levelno);
	int result = chooseList(levelno-1, "Load Level", fileList, levelNum);
	if (result != levelNum)
	{
		sprintf(buffer, PREPATH "level%d.lvl", result+1);
		m_filename = buffer;
		delete m_level;
		m_level = new OEMFLevel((char *) m_filename.c_str());
	}
	delete [] fileList;
}

void OEMFLevelEdit :: run(void)
{
	unsigned int timepassed;
	OEMFFontFactory * font = fonts[FNT_AMIGA];
	SDL_Event event;
	int done = 0;
	
	SDL_WarpMouse(16, 48);
	
	while ( !done) 
	{
		timepassed = SDL_GetTicks();
		
		drawInterface(0, 0, m_screenWidth, m_screenHeight);
		/* Check for events */
		while ( SDL_PollEvent(&event) ) 
		{
			switch (event.type) 
			{
				case SDL_MOUSEMOTION:
					m_mouseX = event.motion.x;
					m_mouseY = event.motion.y;
					if (m_mouseY >= 32 && m_mouseY < m_screenHeight-32)
					{
						m_posX = (m_mouseX / 32) + m_beginX;
						m_posY = ((m_mouseY - 32) / 32) + m_beginY;
						m_posX = (m_posX + (unsigned int) m_level->width()) % (unsigned int) m_level->width();
						m_posY = (m_posY + (unsigned int) m_level->height()) % (unsigned int) m_level->height();
						if (event.motion.state & SDL_BUTTON(1))
							goto LABEL_RETURNEVENT;
						else if (event.motion.state & SDL_BUTTON(3))
							goto LABEL_EYEDROPEVENT;
					}

					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT)
						goto LABEL_RETURNEVENT;
					else if (event.button.button == SDL_BUTTON_RIGHT)
						goto LABEL_EYEDROPEVENT;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{	
						done = 1;
					}
					if (event.key.keysym.sym == SDLK_LEFT)
					{	
						m_posX = (m_posX - 1 + (unsigned int) m_level->width()) % (unsigned int) m_level->width();
					}
					if (event.key.keysym.sym == SDLK_RIGHT)
					{	
						m_posX = (m_posX + 1) % (unsigned int) m_level->width();
					}
					if (event.key.keysym.sym == SDLK_UP)
					{	
						m_posY = (m_posY - 1 + (unsigned int) m_level->height()) % (unsigned int) m_level->height();
					}
					if (event.key.keysym.sym == SDLK_DOWN)
					{	
						m_posY = (m_posY + 1) % (unsigned int) m_level->height();
					}
					else if (event.key.keysym.sym == SDLK_RETURN)
					{
						; LABEL_RETURNEVENT: ;
						if (!m_drawRect)
						{
							if (m_background)
								m_level->tiles()[m_posX + m_posY * m_level->width()].bg = m_currentImage;
							else
							{
								m_level->tiles()[m_posX + m_posY * m_level->width()].fg = m_currentImage;
								m_level->tiles()[m_posX + m_posY * m_level->width()].type = m_currentType;
							}
						}
						else
						{
							for (unsigned int rX = m_originX; rX <= m_posX; ++rX)
								for (unsigned int rY = m_originY; rY <= m_posY; ++rY)
								{
									if (m_background)
										m_level->tiles()[rX + rY * m_level->width()].bg = m_currentImage;
									else
									{
										m_level->tiles()[rX + rY * m_level->width()].fg = m_currentImage;
										m_level->tiles()[rX + rY * m_level->width()].type = m_currentType;
									}
								}
							m_drawRect = false;
						}
					}
					else if (event.key.keysym.sym == SDLK_f)
					{
						if (event.key.keysym.mod == KMOD_LALT
							|| event.key.keysym.mod == KMOD_RALT)
						{
							// fill whole level
							for (unsigned int rX = 0; rX < m_level->width(); ++rX)
								for (unsigned int rY = 0; rY < m_level->height(); ++rY)
								{
									if (m_background)
										m_level->tiles()[rX + rY * m_level->width()].bg = m_currentImage;
									else
									{
										m_level->tiles()[rX + rY * m_level->width()].fg = m_currentImage;
										m_level->tiles()[rX + rY * m_level->width()].type = m_currentType;
									}
								}
						}
						else if (!m_drawRect)
						{
							m_originX = m_posX;
							m_originY = m_posY;
							m_drawRect = true;
						}
						else
						{
							m_drawRect = false;
						}
					}
					else if (event.key.keysym.sym == SDLK_w) // write
					{
						m_level->writeToFile(m_filename.c_str());
						chooseList(0,"Written to '" + m_filename + "'.", NULL, 0);
						int levelno;
						sscanf(m_filename.c_str(), PREPATH "level%d.lvl", &levelno);
						lastEditedLevel = levelno;
					}
					else if (event.key.keysym.sym == SDLK_l) // import
					{
						importLevel();
					}
					else if (event.key.keysym.sym == SDLK_s) // select icon
					{
						m_currentImage = chooseTile(m_currentImage);
					}
					else if (event.key.keysym.sym == SDLK_b) // background
					{
						m_background = !m_background;
					}
					else if (event.key.keysym.sym == SDLK_u) // show types
					{
						m_showTypes = !m_showTypes;
					}
					else if (event.key.keysym.sym == SDLK_t) // select type
					{
						// TODO: better message system
						if (m_background)
							chooseList(0,"You must be in Foreground Mode!", NULL, 0);
						else
							m_currentType = chooseList(m_currentType, "Choose a tile Type:", m_typeList, m_typeListCount);
					}
					else if (event.key.keysym.sym == SDLK_e) // eyedrop
					{
						; LABEL_EYEDROPEVENT: ;
						if (m_background)
							m_currentImage = m_level->getTileBG(m_posX, m_posY);
						else
						{
							m_currentImage = m_level->getTileFG(m_posX, m_posY);
							m_currentType = m_level->getTileType(m_posX, m_posY);
						}
					}
					else if (event.key.keysym.sym == SDLK_r) // resize
					{
						resizeDialog();
					}
					else if (event.key.keysym.sym == SDLK_n) // rename
					{
						m_level->setName(font->inputText(this, m_level->name(), 12*font->charWidth(), 0, 32));
					}
					else if (event.key.keysym.sym == SDLK_i) // change level number
					{
						char newfilename[256];
						int levelno;
						sscanf(m_filename.c_str(), PREPATH "level%d.lvl", &levelno);
						sprintf(newfilename, PREPATH "level%d.lvl", font->inputNumber(this, levelno, 0, 0, 3));
						m_filename = newfilename;
					}
					else if (event.key.keysym.sym == SDLK_y)
					{
						chooseBackground();
					}
					else if (event.key.keysym.sym == SDLK_p)
					{
						// Save level first
						m_level->writeToFile(m_filename.c_str());
						int levelno;
						sscanf(m_filename.c_str(), PREPATH "level%d.lvl", &levelno);
						lastEditedLevel = levelno;
						
						// Start a test game
						OEMFGame * game = new OEMFGame(m_screen, m_execPath, m_screenWidth, m_screenHeight, m_screenBpp);
						game->setPlayerPosition(m_posX * 32, m_posY * 32);
						game->run();
						delete game;
					}
					else if (event.key.keysym.sym == SDLK_PAGEUP)
					{
						if ((int) m_posY - ((int) m_screenHeight-64) / 32 >= 0)
						{
							m_posY = m_posY - (m_screenHeight-64) / 32;
							m_beginY = m_beginY - (m_screenHeight-64) / 32;
						}
					}
					else if (event.key.keysym.sym == SDLK_PAGEDOWN)
					{
						if (m_posY + (m_screenHeight-64) / 32 < m_level->height())
						{
							m_posY = m_posY + (m_screenHeight-64) / 32;
							m_beginY = m_beginY + (m_screenHeight-64) / 32;
						}
					}
					else if (event.key.keysym.sym == SDLK_HOME)
					{
						if ((int) m_posX - (int) m_screenWidth / 32 >= 0)
						{
							m_posX = m_posX - m_screenWidth / 32;
							m_beginX = m_beginX - m_screenWidth / 32;
						}
					}
					else if (event.key.keysym.sym == SDLK_END)
					{
						if (m_posX + m_screenWidth / 32 < m_level->width())
						{
							m_posX = m_posX + m_screenWidth / 32;
							m_beginX = m_beginX + m_screenWidth / 32;
						}
					}
					break;
				case SDL_QUIT:
					exit(0); // TODO
					done = 1;
					break;
				default:
					break;
			}
		}
				
		// 30 FPS
		timepassed = SDL_GetTicks() - timepassed;
		if (timepassed <= 50)
			SDL_Delay(50 - timepassed);
	}
}

