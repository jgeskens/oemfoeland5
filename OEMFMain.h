/*
 *  OEMFMain.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFMAIN_H__
#define __OEMFMAIN_H__

#include <string>
#include "OEMFEnvironment.h"

using namespace std;

class OEMFMain : public OEMFEnvironment
{
public:
	OEMFMain(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp);
	~OEMFMain(void);
	
	void drawIntro(void);
	virtual void run(void);
	virtual void one_iter(void);
	int done;
	unsigned int menuIndex;
	unsigned int frames;
	string songfile;
protected:
	string * m_menuOptions;
	unsigned int m_menuCount;
};

#endif

