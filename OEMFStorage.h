/*
 *  OEMFStorage.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 26/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFSTORAGE_H__
#define __OEMFSTORAGE_H__

#include <vector>

using namespace std;

#include "OEMFImage.h"
#include "OEMFFontFactory.h"
#include "OEMFEnvironment.h"

#if defined (__APPLE__) || defined (MACOSX)
#define PREPATH ""
#else
#define PREPATH "data/"
#endif

#if defined(__DEBUG__) || defined(_DEBUG)
#define DPRINTF printf
#else
#define DPRINTF if (0) printf
#endif

#define IMAGECOUNT ((int) images.size())

#define IMG_INTRO 0
#define IMG_OEMFOE 1
#define IMG_SCOREB 2
#define IMG_WALL 3
#define IMG_COIN 4
#define IMG_BG 5
#define IMG_BGWINDOW 6
#define IMG_SELECTION 7
#define IMG_NULL 8
#define IMG_CLOUDS1 9
#define IMG_CLOUDS2 10
#define IMG_SNOWFULL 11
#define IMG_SNOWGROUND 12
#define IMG_ARROWLEFT 13
#define IMG_ARROWRIGHT 14
#define IMG_PLATFORM1 15
#define IMG_PLATFORM2 16
#define IMG_CURSOR 17
#define IMG_SNOWMAN 18
#define IMG_MOVABLE 19
#define IMG_OEMFOEMINI 20
#define IMG_SDL 21
#define IMG_BOMB 22
#define IMG_GREYBRICKS 23
#define IMG_MARBLEROCK 24
#define IMG_ENDSCREEN 25

#define FONTCOUNT ((int) fonts.size())

#define FNT_AMIGA 0
#define FNT_MENU 1

#define SOUNDCOUNT ((int) sounds.size())

#define SND_COIN 0
#define SND_TIK 1
#define SND_TOK 2
#define SND_HMF 3
#define SND_PLOF 4
#define SND_HIP 5
#define SND_DRAG 6
#define SND_OHNO 7

#define TYPECOUNT 13

#define TYPE_FREE 0
#define TYPE_WALL 1
#define TYPE_PLATFORM 2
#define TYPE_MOVABLE 3
#define TYPE_COIN100 4
#define TYPE_END 5
#define TYPE_START 6
#define TYPE_UNSTABLE 7
#define TYPE_DEADLY 8
#define TYPE_LEFTFORCE 9
#define TYPE_RIGHTFORCE 10
#define TYPE_LEFTCONV 11
#define TYPE_RIGHTCONV 12

void loadFilesIntoDataMemory(const char * defFile);
void releaseDataMemory();
void animateImages();
void setLoadProgressCB(void (*func)(int current, int total, void * object), void * object);

extern vector<OEMFImage *> images;
extern vector<unsigned int> le_tiles;
extern vector<OEMFFontFactory *> fonts;
extern vector<Mix_Chunk *> sounds;

extern MusicPlayer * musicPlayer;

extern bool musicEnabled;
extern bool fullscreenMode;
extern int lastEditedLevel;
#endif
