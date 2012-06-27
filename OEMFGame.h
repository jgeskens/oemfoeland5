/*
 *  OEMFGame.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFGAME_H__
#define __OEMFGAME_H__

#include "OEMFEnvironment.h"
#include "OEMFLevel.h"
#include "OEMFStorage.h"
#include "OEMFGObject.h"

#define KEYCOUNT 1024

using namespace std;

class OEMFGame : public OEMFEnvironment
{
public:
	OEMFGame(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp);
	~OEMFGame();

	virtual void run();
	void setPlayerPosition(int x, int y);
protected:
	void refresh();
	void refreshCentered();
	void refreshScoreBoard();
	void handleInput();
	void initialize();
	void endLevel();
	void die();
	bool checkPosition(OEMFGObject * causedObj, int newX, int newY, bool handleCollisions = true);
	void handleCollision(OEMFGObject * causedObj, OEMFGObject * victimObj, int dx, int dy, bool hardSurface);
	unsigned int m_levelNo;
	unsigned char m_lives;
	int m_score;
	OEMFGObject * m_player;
	OEMFLevel * m_level;
	int m_beginX;
	int m_beginY;
	int m_screenScrollX;
	int m_screenScrollY;
	int m_done;
	bool m_requestFlagEnd;
	bool m_requestFlagDie;
	
	// timing
	Uint32 m_frameDuration;
	Uint32 m_delayTime;
	
	// keyboard state
	bool m_keyDown[KEYCOUNT];
	
	// physics components
	void updatePhysics();
	deque<OEMFGObject *> m_physicsEnabled;
};

#endif
