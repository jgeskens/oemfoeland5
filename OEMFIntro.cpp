/*
 *  OEMFIntro.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 25/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include "OEMFEnvironment.h"
#include "OEMFIntro.h"
#include "OEMFImage.h"
#include "OEMFFontFactory.h"
#include "OEMFString.h"
#include "OEMFStorage.h"

#if !defined(__DEBUG__) && defined(_DEBUG)
	#define __DEBUG__
#endif

OEMFIntro :: ~OEMFIntro(void)
{
}

void progress(int current, int total, void * object)
{
	OEMFIntro * intro = (OEMFIntro *) object;
	// border
	intro->clearRectWithColor(160, 232, 320, 16, 0xFFFFFFFF, false);
	intro->clearRectWithColor(161, 233, 318, 14, 0xFF000000, false);
	// bar
	intro->clearRectWithColor(161, 233, (318 * current) / total, 14, 0xFF6633FF, false);
	intro->clearRectWithColor(161, 233, (318 * current) / total - 1, 1, 0xFF9966FF, false);
	intro->clearRectWithColor(161, 233, 1, 13, 0xFF9966FF, false);
	intro->clearRectWithColor(162, 233 + 13, (318 * current) / total - 1, 1, 0xFF330099, false);
	intro->clearRectWithColor(161 + (318 * current) / total - 1, 234, 1, 13, 0xFF330099, false);
	intro->updateScreen();
}

void OEMFIntro :: run(void)
{
	// small hack to display "loading..." without the amiga rom being loaded
	
	OEMFImage * amigarom = new OEMFImage(PREPATH "font.png");
	OEMFFontFactory * tempfont = new OEMFFontFactory(amigarom, 32, 127, 8, 16, 96);
	delete amigarom;
	tempfont->blitCenterText(this, "Loading...", 0xFFFFFF00, 216, m_screenWidth);
	updateScreen();
	delete tempfont;
	
	// loading...
	setLoadProgressCB(progress, this);
	loadFilesIntoDataMemory("resources.def");
	clearRectWithColor(0, 216, 640, 16, 0xFF000000, false);
	updateScreen();
	
#ifndef __DEBUG__
	OEMFFontFactory * ff = fonts[FNT_AMIGA];
	OEMFFontFactory * menufont = fonts[FNT_MENU];
	OEMFImage * oemfoelogo = images[IMG_INTRO];
	
	// loading done
	//ripEffect(224, 16);
	SDL_Delay(1000);
	fadeOut();
	
	//clearWithColor(0xFFFFFF);
	//blitImage(images[IMG_SDL], (m_screenWidth - images[IMG_SDL]->width()) / 2, 120);
//	ff->blitCenterText(this, "http://www.libsdl.org", 0x330077, 400, m_screenWidth, false);
//	updateScreen();
//	SDL_Delay(3000);
//	fadeOut();

	ff->blitCenterText(this, "Royal Belgian Beer Squadron", 0xFFFFFFFF, 224, m_screenWidth);
	SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight);
	SDL_Delay(1500);
	//ripEffect(224, 16);
	fadeOut();
	
	ff->blitCenterText(this, "presents...", 0xFFFFFFFF, 224, m_screenWidth);
	SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight);
	SDL_Delay(1500);
	//ripEffect(224, 16);
	fadeOut();
	
	// draw welcome screen
	blitImage(oemfoelogo, 0, 0);
	ff->blitCenterText(this, "Idea:                      Oemfoe", 0xFFFFFFFF, 192+192, m_screenWidth, true);
	ff->blitCenterText(this, "Coding:                    Oemfoe", 0xFFFFFFFF, 208+192, m_screenWidth, true);
	ff->blitCenterText(this, "GFX:                       Kuroto", 0xFFFFFFFF, 224+192, m_screenWidth, true);
	ff->blitCenterText(this, "Music:            Belief Systems*", 0xFFFFFFFF, 240+192, m_screenWidth, true);
	SDL_UpdateRect(m_screen, 0, 0, m_screenWidth, m_screenHeight);
	SDL_Delay(3000);
#endif
	
	if (musicPlayer->isPlayingSong())
		musicPlayer->stopSong();
}
