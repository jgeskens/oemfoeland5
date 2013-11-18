/*
 *  OEMFIntro.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 25/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFINTRO_H__
#define __OEMFINTRO_H__

#include "OEMFEnvironment.h"

class OEMFIntro : public OEMFEnvironment
{
public:
	OEMFIntro(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp)
		: OEMFEnvironment(screen, execPath, screenWidth, screenHeight, screenBpp) {};
	~OEMFIntro(void);
	
	
	virtual void run(void);
};

#endif
