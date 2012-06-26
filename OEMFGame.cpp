/*
 *  OEMFGame.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>
#include <deque>

#include "OEMFGame.h"
#include "OEMFStorage.h"
#include "OEMFString.h"

OEMFGame :: OEMFGame(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp)
	: OEMFEnvironment(screen, execPath, screenWidth, screenHeight, screenBpp)
{
	m_levelNo = lastEditedLevel;
	m_lives = 3;
	m_frameDuration = 20;
	m_delayTime = 20;
	m_level = 0x0;
	m_score = 0;
	m_done = 0;
	m_requestFlagEnd = false;
	m_requestFlagDie = false;
	
	initialize();
}

OEMFGame :: ~OEMFGame()
{
	delete m_level;
}

void OEMFGame :: initialize()
{
	float gravity = 0.5f;
	
	m_beginX = 0;
	m_beginY = 0;
	
	char levelfile[256];
	sprintf(levelfile, PREPATH "level%d.lvl", (int) m_levelNo);
	
	if (m_level != 0)
		delete m_level;
	FILE * fh = fopen(levelfile, "r");
	
	if (fh != 0)
	{
		// level found!
		fclose(fh);
		
		m_level = new OEMFLevel(levelfile);
	}
	else
	{
		m_level = new OEMFLevel(PREPATH "level1.lvl");
	}
	
	m_level->prepareForGame(); // very important!
	
	m_player = m_level->types[TYPE_START][0];
	
	m_physicsEnabled.clear();
	m_physicsEnabled.push_back(m_player); // apply physics to the player
	for (int i = 0; i < m_level->m_typeCounts[TYPE_MOVABLE]; ++i)
	{
		m_physicsEnabled.push_back(m_level->types[TYPE_MOVABLE][i]);
		m_level->types[TYPE_MOVABLE][i]->m_ay = gravity; // gravity
	}
	
	// reset the keys
	for (int i = 0; i < KEYCOUNT; ++i)
	{
		m_keyDown[i] = false;
	}
	
	// reset the physics components
	m_player->m_ay = gravity; // gravity
	
	if (fh != 0)
	{
		// initialisation done
		refreshCentered();
		refreshScoreBoard();
		updateScreen();
		
		darkenRect(160, 128, 320, 96);
		char levelbuffer[256];
		sprintf(levelbuffer, "Prepare for level %d:", m_levelNo);
		fonts[FNT_AMIGA]->blitCenterText(this, string(levelbuffer), 0xFF7700, 128 + 16, m_screenWidth);
		fonts[FNT_AMIGA]->blitCenterText(this, string("\"") + m_level->name() + "\"", 0xFFFF00, 128 + 32, m_screenWidth);
		fonts[FNT_AMIGA]->blitCenterText(this, string("Go Go Go!"), 0xFF0000, 128 + 64, m_screenWidth);
		updateScreen();
		SDL_Delay(3000);
	}
	else
	{
		// game has ended!
		m_done = 1;
		blitImage(images[IMG_ENDSCREEN], 0, 0);
		blitImage(images[IMG_OEMFOE], 32, 360);
		fonts[FNT_AMIGA]->blitText(this,
//			"----/----|----/----|----/----|----/----|"
			"Congratulations! You reached the end of\n"
			"the game! You can now do something else\n"
			"with your life, but if you are too\n"
			"addicted, you can always use the Level\n"
			"Editor and create more worlds.\n"
			"Press any key to continue...",
			0xFFFF00, 80, 320, 320, false);
		updateScreen();
		waitForKey();
		fadeOut();
	}
}

void OEMFGame :: endLevel()
{
	darkenRect(160, 128, 320, 80);
	fonts[FNT_AMIGA]->blitCenterText(this, string("Congratulations!"), 0x007FFF, 128 + 16, m_screenWidth);
	updateScreen();
	SDL_Delay(1000);
	fonts[FNT_AMIGA]->blitCenterText(this, string("You advanced to the next level!"), 0xFF7700, 128 + 32, m_screenWidth);
	updateScreen();
	SDL_Delay(1000);
	fonts[FNT_AMIGA]->blitCenterText(this, string("You get 1000 bonus points!"), 0xFFFF00, 128 + 48, m_screenWidth);
	updateScreen();
	SDL_Delay(1000);
	for (int i = 0; i < 10; ++i)
	{
		musicPlayer->playSound(sounds[SND_COIN]);
		m_score += 100;
		refreshScoreBoard();
		updateScreen();
		SDL_Delay(250);
	}
	m_levelNo += 1;
	SDL_Delay(2000);
	fadeOut();
}


void OEMFGame :: die()
{
	musicPlayer->playSound(sounds[SND_OHNO]);
	m_player->m_vy = 0.0f;
	
	m_level->moveObject(m_player, m_level->startX() * 32, m_level->startY() * 32);
	
	if (m_lives > 0)
	{
		m_lives -= 1;
		darkenRect(160, 128, 320, 48);
		char msg[256];
		sprintf(msg, "You died! Lives left:    x%2d", m_lives);
		fonts[FNT_AMIGA]->blitText(this, msg, 0xFF7700, 176, 128 + 16, 320 - 32, false);
		blitImage(images[IMG_OEMFOEMINI], 160 + 112 + 80, 128 + 16);
		updateScreen();
		SDL_Delay(2000);
		fadeOut();
	}
	else
	{
		// GAME OVER!
		m_done = 1;
		darkenRect(160, 128, 320, 48);
		fonts[FNT_AMIGA]->blitCenterText(this, string("GAME OVER!"), 0xFF0000, 128 + 16, m_screenWidth, false);
		updateScreen();
		SDL_Delay(3000);
		fadeOut();
	}
}


void OEMFGame :: refreshCentered()
{
	deque<OEMFGObject *> :: iterator it;
	int x, y, ypos;
	int width = m_level->width();
	int height = m_level->height();
	int posWidth = m_screenWidth / 32 + 2;
	int posHeight = m_screenHeight / 32 + 1;
	int centerX = (m_screenWidth - 32) / 2;
	int centerY = (m_screenHeight - 64) / 2;
	int beginX = m_player->posX() / 32 - centerX / 32 - 1;
	int beginY = m_player->posY() / 32 - centerY / 32 - 1;
	int beginPosX = -(m_player->posX() % 32);
	int beginPosY = -(m_player->posY() % 32);
	int totalBeginX = beginX * 32 - beginPosX - 16 + 32;
	int totalBeginY = beginY * 32 - beginPosY - 16 + 32;
	tile_t * ltiles = m_level->tiles();
	
	// swap all images of animations
	animateImages();
	
	// background
	blitImage(images[m_level->background()], 0, 0);
	for (y = beginY; y < height && (y-beginY) < posHeight; y++)
	{
		ypos = y * width;
		for (x = beginX; x < width && (x-beginX) < posWidth; x++)
		{
			if (x >= 0 && y >= 0)
			{
				if (ltiles[x+ypos].bg != IMG_NULL)
					blitImage(images[ltiles[x+ypos].bg], x * 32 - totalBeginX, y * 32 - totalBeginY);
			}
		}
	}
	
	// foreground
	// draw exact tiles
	for (y = beginY; y < height && (y-beginY) < posHeight; y++)
	{
		ypos = y * width;
		for (x = beginX; x < width && (x-beginX) < posWidth; x++)
		{
			if (x >= 0 && y >= 0)
			{
				for (it = m_level->raster[x+ypos]->objects.begin(); it != m_level->raster[x+ypos]->objects.end(); it++)
				{
					if ((*it)->image() != IMG_NULL && ((*it)->posX() % 32 == 0 && (*it)->posY() % 32 == 0))
						blitImage(images[(*it)->image()], (*it)->posX() - totalBeginX, (*it)->posY() - totalBeginY);
				}
			}
		}
	}
	// draw odd tiles
	for (y = beginY; y < height && (y-beginY) < posHeight; y++)
	{
		ypos = y * width;
		for (x = beginX; x < width && (x-beginX) < posWidth; x++)
		{
			if (x >= 0 && y >= 0)
			{
				for (it = m_level->raster[x+ypos]->objects.begin(); it != m_level->raster[x+ypos]->objects.end(); it++)
				{
					if ((*it)->image() != IMG_NULL && ((*it)->posX() % 32 != 0 || (*it)->posY() % 32 != 0))
						blitImage(images[(*it)->image()], (*it)->posX() - totalBeginX, (*it)->posY() - totalBeginY);
				}
			}
		}
	}
	
//	blitImage(images[IMG_SELECTION], beginPosX, beginPosY);
//	char buffer[1024];
//	sprintf(buffer, "beginX=%d beginY=%d beginPosX=%d beginPosY=%d", beginX, beginY, beginPosX, beginPosY);
//	fonts[FNT_AMIGA]->blitCenterText(this, buffer, 32 ,m_screenWidth, false);
}

void OEMFGame :: refresh()
{
	deque<OEMFGObject *> :: iterator it;
	int widthInBlocks = m_screenWidth / 32;
	int heightInBlocks = m_screenHeight / 32 - 1;
	// position calculations
	while (m_player->posX() < m_beginX * 32 - 16)
		m_beginX -= widthInBlocks;
	while (m_player->posX() > (m_beginX + widthInBlocks) * 32 - 16)
		m_beginX += widthInBlocks;
	while (m_player->posY() < m_beginY * 32 - 16)
		m_beginY -= heightInBlocks;
	while (m_player->posY() > (m_beginY + heightInBlocks) * 32 - 16)
		m_beginY += heightInBlocks;
	
	unsigned int x, y, ypos, width, height;
	width = m_level->width();
	height = m_level->height();
	
	tile_t * ltiles = m_level->tiles();
	
	// background
	// blitImageRect(images[m_level->background()], 0, 0, 0, 0, m_screenWidth, m_screenHeight - 32);
	blitImage(images[m_level->background()], 0, 0);
	for (y = m_beginY; y < height && (y-m_beginY) * 32 < m_screenHeight - 32; y++)
	{
		ypos = y * width;
		for (x = m_beginX; x < width && (x-m_beginX) * 32 < m_screenWidth; x++)
		{
			if (ltiles[x+ypos].bg != IMG_NULL)
				blitImage(images[ltiles[x+ypos].bg], (x-m_beginX) * 32, (y-m_beginY) * 32);
		}
	}
	
	// debug
//	for (y = m_beginY; y < height && (y-m_beginY) * 32 < m_screenHeight - 32; y++)
//	{
//		ypos = y * width;
//		for (x = m_beginX; x < width && (x-m_beginX) * 32 < m_screenWidth; x++)
//		{
//			int i = 0;
//			for (it = m_level->raster[x+ypos]->objects.begin(); it != m_level->raster[x+ypos]->objects.end(); it++)
//			{
//				blitImageRect(images[(*it)->image()],(x - m_beginX) * 32 + (i % 4) * 8,(y - m_beginY) * 32 + (i / 4) * 8,0,0, 8, 8);
//				i++;
//			}
//		}
//	}
	
	// foreground
	// draw exact tiles
	for (y = m_beginY; y < height && (y-m_beginY) * 32 < m_screenHeight - 32; y++)
	{
		ypos = y * width;
		for (x = m_beginX; x < width && (x-m_beginX) * 32 < m_screenWidth; x++)
		{
			for (it = m_level->raster[x+ypos]->objects.begin(); it != m_level->raster[x+ypos]->objects.end(); it++)
			{
				if ((*it)->image() != IMG_NULL && ((*it)->posX() % 32 == 0 && (*it)->posY() % 32 == 0 ))
					blitImage(images[(*it)->image()], (*it)->posX() - m_beginX * 32, (*it)->posY() - m_beginY * 32);
			}
		}
	}
	
	// draw odds
	for (y = m_beginY; y < height && (y-m_beginY) * 32 < m_screenHeight - 32; y++)
	{
		ypos = y * width;
		for (x = m_beginX; x < width && (x-m_beginX) * 32 < m_screenWidth; x++)
		{
			for (it = m_level->raster[x+ypos]->objects.begin(); it != m_level->raster[x+ypos]->objects.end(); it++)
			{
				if ((*it)->image() != IMG_NULL && ((*it)->posX() % 32 != 0 || (*it)->posY() % 32 != 0 ))
					blitImage(images[(*it)->image()], (*it)->posX() - m_beginX * 32, (*it)->posY() - m_beginY * 32);
			}

		}
	}
}

void OEMFGame :: refreshScoreBoard()
{
	// scoreboard
	blitImage(images[IMG_SCOREB], 0, m_screenHeight - 32);
	char stats[1024];

	sprintf(stats, "Score: %6d", m_score);
	fonts[FNT_AMIGA]->blitText(this, stats, 0xFFFF00, 12, m_screenHeight-24, m_screenWidth-12, false);

	sprintf(stats, "              Lives:    x%2d", m_lives);
	fonts[FNT_AMIGA]->blitText(this, stats, 0xFF7700, 12, m_screenHeight-24, m_screenWidth-12, false);

	//sprintf(stats, "                            Level: %3d                          (FD:DT %d:%d)", m_levelNo, m_frameDuration, m_delayTime);
	sprintf(stats, "                            Level: %3d                          ", m_levelNo);
	fonts[FNT_AMIGA]->blitText(this, stats, 0x3399FF, 12, m_screenHeight-24, m_screenWidth-12, false);

	blitImage(images[IMG_OEMFOEMINI], 180, m_screenHeight - 24);
}

void OEMFGame :: handleInput()
{
	if (m_done == 1)
		return;
	
	float accel = 4.0f;
	m_player->m_vx = 0.0f;
	if (m_keyDown[SDLK_LEFT]) m_player->m_vx += -accel;
	if (m_keyDown[SDLK_RIGHT]) m_player->m_vx += accel;
	if (m_keyDown[SDLK_UP])
	{
		if (!checkPosition(m_player, m_player->posX(), m_player->posY() + 1, false)
			&& checkPosition(m_player, m_player->posX(), m_player->posY() - 1, false)) // am I standing on something? is there room to jump?
		{
			m_player->m_vy = -10.0f;
			musicPlayer->playSound(sounds[SND_HIP]);
		}
	}
	if (!m_keyDown[SDLK_LEFT] && !m_keyDown[SDLK_RIGHT])
	{
#ifndef WINCE
		Mix_HaltChannel(15);
#endif
	}
}

void OEMFGame :: updatePhysics()
{
	if (m_done == 1)
		return;
	
	int x, y;
	deque<OEMFGObject *>::iterator it;
	bool needMove = false;
	for (it = m_physicsEnabled.begin(); it != m_physicsEnabled.end(); it++)
	{
		OEMFGObject * obj = *it;
		// calculate acceleration
		obj->m_vx += obj->m_ax;
		obj->m_vy += obj->m_ay;
		
		// reset the horizontal speed of all movables
		if (obj->type() == TYPE_MOVABLE)
			obj->m_vx = 0.0f;

		// calculate conveyors and forces
		deque<OEMFGObject *> objectsBelow = m_level->probePosition(obj->posX(), obj->posY() + 1);
		bool forceFound = false;
		OEMFGObject * belowObject = 0;
		for (deque<OEMFGObject *>::iterator belowIt = objectsBelow.begin(); !forceFound && belowIt != objectsBelow.end(); belowIt++)
		{
			belowObject = *belowIt;
			if (belowObject->type() == TYPE_LEFTCONV || belowObject->type() == TYPE_RIGHTCONV
				|| belowObject->type() == TYPE_LEFTFORCE || belowObject->type() == TYPE_RIGHTFORCE)
			{
				forceFound = true;
				if (belowObject->type() == TYPE_LEFTCONV || belowObject->type() == TYPE_LEFTFORCE)
				{
					obj->m_vx += -2.0f;
				}
				else if (belowObject->type() == TYPE_RIGHTCONV || belowObject->type() == TYPE_RIGHTFORCE)
				{
					obj->m_vx += 2.0f;
				}
			}
		}
		
		// calculate resulting position
		x = obj->posX();
		y = obj->posY();
		x = x + (int) obj->m_vx;
		y = y + (int) obj->m_vy;
		
		needMove = false;
		if (x == obj->posX())
			obj->m_vx = 0.0f;
		else
			needMove = true;
		
		if (y == obj->posY() && !checkPosition(obj,obj->posX(), obj->posY()+1, false))
			obj->m_vy = 0.0f;
		else
			needMove = true;
		// obj->m_vx = 0.0f;
		
		// first move vertically and then horizontally.
		// not the both directions at the same time... so it's easier to distinguish
		if (needMove)
		{
			int oldX = obj->posX();
			if (checkPosition(obj, obj->posX(), y)) 
			{
				m_level->moveObject(obj, obj->posX(), y);
			}
			// X can already be changed by the vertical movement!
			// When X is changed, don't do anything because it is already arranged!
			if (obj->posX() == oldX && checkPosition(obj, x, obj->posY()))
			{
				m_level->moveObject(obj, x, obj->posY());
			}
		}
	}
}

bool OEMFGame :: checkPosition(OEMFGObject * causedObj, int newX, int newY, bool handleCollisions)
{
	int dx, dy;
	dx = newX - causedObj->posX();
	dy = newY - causedObj->posY();
	// probe position
	deque<OEMFGObject *> objects = m_level->probePosition(newX, newY);
	OEMFGObject * obj;
	bool canPass = false;
	for (deque<OEMFGObject *>::iterator it = objects.begin(); it != objects.end(); it++)
	{
		obj = *it;
		if (obj != causedObj)
		{
			switch (obj->type())
			{
				case TYPE_START:
					if (causedObj != m_player)
					{
						if (handleCollisions) handleCollision(causedObj, obj, dx, dy, true);
						return false;
					}
					break;
				case TYPE_WALL:
					if (handleCollisions) handleCollision(causedObj, obj, dx, dy, true);
					return false;
					break;
				case TYPE_MOVABLE:
					if (dx != 0)
					{
						if (checkPosition(obj, obj->posX() + dx, obj->posY()))
						{
							m_level->moveObject(obj, obj->posX() + dx, obj->posY());
#ifndef WINCE
							if (!Mix_Playing(15))
								Mix_PlayChannel(15, sounds[SND_DRAG], 0);
#endif
							canPass = true;
						}
						else
						{
#ifndef WINCE
							Mix_HaltChannel(15);
#endif
							canPass = false;
						}
					}
					else
						canPass = false;
						
					// canPass = false;
					if (handleCollisions) handleCollision(causedObj, obj, dx, dy, !canPass);
					if (!canPass) return false;
					break;
				case TYPE_COIN100:
					if (handleCollisions) handleCollision(causedObj, obj, dx, dy, false);
					break;
				case TYPE_UNSTABLE:
					if (handleCollisions) handleCollision(causedObj, obj, dx, dy, (obj->flag() != 1));
					if (obj->flag() != 1) return false; // if flag is set, unstable object is not there and the space is open
					break;
				case TYPE_PLATFORM:
					if (handleCollisions) 
						handleCollision(causedObj, obj, dx, dy, dy > 0 && causedObj->posY() <= obj->posY() - 32);
					if (dy > 0 && causedObj->posY() <= obj->posY() - 32) return false;
					break;
				case TYPE_END:
					if (handleCollisions) handleCollision(causedObj, obj, dx, dy, true);
					return false;
					break;
				case TYPE_DEADLY:
					if (handleCollisions) handleCollision(causedObj, obj, dx, dy, true);
					return false;
					break;
				case TYPE_LEFTCONV:
				case TYPE_RIGHTCONV:
					if (handleCollisions) handleCollision(causedObj, obj, dx, dy, true);
					return false;
					break;
				case TYPE_LEFTFORCE:
				case TYPE_RIGHTFORCE:
					if (handleCollisions) handleCollision(causedObj, obj, dx, dy, false);
					break;
			}
		}
	}
	return true;
}

void OEMFGame :: handleCollision(OEMFGObject * causedObj, OEMFGObject * victimObj, int dx, int dy, bool hardSurface)
{
	if (hardSurface) // player collides with floor or ceiling
	{
		if (dy != 0)
		{	
			m_level->moveObject(causedObj, causedObj->posX(), 
									(dy > 0 ? victimObj->posY()-32 : victimObj->posY()+32)); // move te latest pixels
			causedObj->m_vy = -causedObj->m_vy / 2.0f;
			if (causedObj->m_vy < -0.5f)
			{
				if (causedObj != m_player) musicPlayer->playSound(sounds[SND_PLOF]);
				else musicPlayer->playSound(sounds[SND_TOK]);
			}
		}
		else 
		{
			m_level->moveObject(causedObj, (dx > 0 ? victimObj->posX()-32 : victimObj->posX()+32), 
									causedObj->posY()); // move te latest pixels
			if (causedObj != m_player)
				causedObj->m_vx = -causedObj->m_vx / 2.0f;
		}
	}
	if (causedObj == m_player && victimObj->type() == TYPE_COIN100)
	{
		if (victimObj->flag() == 0)
		{
			musicPlayer->playSound(sounds[SND_COIN]);
			m_score += 100;
			victimObj->setImage(IMG_NULL);
			victimObj->setFlag(1);
		}
	}
	else if (victimObj->type() == TYPE_UNSTABLE)
	{
		if (victimObj->flag() == 0 && dy != 0)
		{
			victimObj->setImage(IMG_NULL);
			victimObj->setFlag(1);
		}
	}
	else if (causedObj == m_player && victimObj->type() == TYPE_END)
	{
		if (victimObj->flag() == 0)
		{
			victimObj->setFlag(1); // void this object
			m_requestFlagEnd = true;
		}
	}
	else if (victimObj->type() == TYPE_DEADLY)
	{
		if (causedObj == m_player)
		{
			m_requestFlagDie = true;
		}
		else
		{
			// another object dies...
			// TODO: remove object
		}
	}
	else if (victimObj == m_player && causedObj->type() == TYPE_MOVABLE && dy > 0)
	{
		die(); // painful when a rock falls on you ;-)
	}
}

void OEMFGame :: run()
{
	// clearWithColor(0x000000, false);
	// refresh();
	refreshCentered();
	refreshScoreBoard();
	updateScreen();
	
	Uint32 start = SDL_GetTicks();
	Sint32 waitTime = 0;
	
	SDL_Event event;
	while (!m_done) 
	{
		if ((waitTime = SDL_GetTicks() - start) >= m_frameDuration) 
		{
			start = SDL_GetTicks();
			//refresh();
			refreshCentered();
			refreshScoreBoard();
			updateScreen();
		}
		handleInput();
		updatePhysics();
		
		// handle flags
		if (m_requestFlagDie)
		{
			m_requestFlagDie = false;
			die();
		}
		if (m_requestFlagEnd)
		{
			m_requestFlagEnd = false;
			endLevel();
			initialize();
		}
		
		Uint32 sym;
		/* Check for events */
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{	
						string options[2] = {string("No"), string("Yes")};
						if (chooseList(0, "Are you sure you want to end the game?", options, 2) == 1)
							m_done = 1;
					}
					else if (event.key.keysym.sym == SDLK_q)
					{
						m_frameDuration-=10;
					}
					else if (event.key.keysym.sym == SDLK_w)
					{
						m_frameDuration+=10;
					}
					else if (event.key.keysym.sym == SDLK_e)
					{
						m_delayTime--;
					}
					else if (event.key.keysym.sym == SDLK_r)
					{
						m_delayTime++;
					}
					else if (event.key.keysym.sym == SDLK_p)
					{
						SDL_SaveBMP(m_screen, "screenshot.bmp");
					}
					else
					{
						// key array
						sym = event.key.keysym.sym;
						if (sym < KEYCOUNT)
						{
							m_keyDown[sym] = true;
						}
					}
					break;
				case SDL_KEYUP:
					// key array
					sym = event.key.keysym.sym;
					if (sym < KEYCOUNT)
					{
						m_keyDown[sym] = false;
					}
					break;
				case SDL_QUIT:
					m_done = 1;
					break;
				default:
					break;
			}
		}
//		Uint32 start2 = SDL_GetTicks();
//		while (SDL_GetTicks() - start2 < m_delayTime);
		
		SDL_Delay(m_delayTime);
		
//		frameDuration = SDL_GetTicks() - start;
//		waitTime = 20 - (Sint32) frameDuration;
//		if (waitTime > 0)
//			SDL_Delay(waitTime);
	}
}


