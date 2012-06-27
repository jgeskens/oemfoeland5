/*
 *  OEMFStorage.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 26/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include "OEMFStorage.h"
#include "OEMFImage.h"
#include "OEMFString.h"
#include "OEMFFontFactory.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

vector<OEMFImage *> images;
vector<unsigned int> le_tiles;
vector<OEMFFontFactory *> fonts;
vector<Mix_Chunk *> sounds;
MusicPlayer * musicPlayer;

int lastEditedLevel = 1;
int animationWaitCounter = 0;
int animationSkip = 4;

void (*loadProgressFunc)(int current, int total, void * object) = NULL;
void * loadProgressObject = NULL;

#if defined(__DEBUG__) || defined(_DEBUG)
bool musicEnabled = false;
bool fullscreenMode = false;
#else
bool musicEnabled = true;
bool fullscreenMode = true;
#endif

void loadFilesIntoDataMemory(const char * defFile)
{
	FILE * def = fopen(defFile, "rb");
	if (def == 0)
	{
		fprintf(stderr, "FATAL ERROR: Resource definitions file '%s' not found.\n", defFile);
		exit(1);
	}
	
	// count lines
	int lineCount = 0;
	char buffer[1024];
	while (fgets(buffer, 1024, def) != 0)
		lineCount += 1;
	fclose(def);
	def = fopen(defFile, "rb");
	
	// read the definitions file
	char resType[256];
	char fileName[1024];
	
	map<int, vector<OEMFImage *> > animations;
	map<int, vector<OEMFImage *> >::iterator animIt;
	
	int currentLine = 0;
	while (fgets(buffer, 1024, def) != 0)
	{
		if (strncmp(buffer, "image ", 6) == 0)
		{
			int flag, animID;
			sscanf(buffer, "%s %s %d %d\n", resType, fileName, &flag, &animID);
			OEMFImage * loadedImg = new OEMFImage((char *) (string(PREPATH) + fileName).c_str());
			images.push_back(loadedImg);
			if (flag == 1)
				le_tiles.push_back((unsigned int) images.size()-1);
			
			// arrange animations
			if (animID > 0)
			{
				animIt = animations.find(animID);
				if (animIt == animations.end())
				{
					animations[animID].push_back(loadedImg);
					animations[animID].push_back(loadedImg);
					DPRINTF("new animation number %d: %p\n", animID, loadedImg);
				}
				else
				{
					animations[animID][1]->next = loadedImg;
					DPRINTF("add to animation number %d: %p->%p\n", animID, animations[animID][1], loadedImg);
					animations[animID][1] = loadedImg;
				}
			}
			
			DPRINTF("images[%d]=%s (flag %d)\n", (int) images.size()-1, fileName, flag);
		}
		else if (strncmp(buffer, "font ", 5) == 0)
		{
			int fntShadow, fntStart, fntEnd, fntWidth, fntHeight, rowCount;
			sscanf(buffer, "%s %s %d %d %d %d %d %d\n", resType, fileName, 
				&fntShadow, &fntStart, &fntEnd, &fntWidth, &fntHeight, &rowCount);
			OEMFImage * loadedImg = new OEMFImage((char *) (string(PREPATH) + fileName).c_str());
			OEMFFontFactory * font = new OEMFFontFactory(loadedImg, fntStart, fntEnd, fntWidth, fntHeight, rowCount);
			if (fntShadow > 0)
				font->setShadow(fntShadow);
			fonts.push_back(font);
			delete loadedImg;
			DPRINTF("fonts[%d]=%s (%d, %d, %d, %d, %d, %d)\n", (int) fonts.size()-1, fileName, 
				fntShadow, fntStart, fntEnd, fntWidth, fntHeight, rowCount);
		}
		else if (strncmp(buffer, "sound ", 6) == 0)
		{
			sscanf(buffer, "%s %s\n", resType, fileName);
			Mix_Chunk * sound = Mix_LoadWAV((char *) (string(PREPATH) + fileName).c_str());
			sounds.push_back(sound);
			DPRINTF("sounds[%d]=%s\n", (int) sounds.size()-1, fileName);
		}
		currentLine += 1;
		if (loadProgressFunc != NULL)
			loadProgressFunc(currentLine, lineCount, loadProgressObject);
	}
	
	// close the definitions file
	fclose(def);
	
	// attach the begin and end of the animation cycles
	for (animIt = animations.begin(); animIt != animations.end(); animIt++)
	{
		animIt->second[1]->next = animIt->second[0];
		DPRINTF("%d: %p->%p\n", animIt->first, animIt->second[1], animIt->second[0]);
	}
}

void animateImages()
{
	animationWaitCounter = (animationWaitCounter + 1) % animationSkip;
	if (animationWaitCounter != 0)
		return;
	
	OEMFImage * img;
	int len = (int) images.size();
	for (int i = 0; i < len; ++i)
	{
		img = images[i];
		if (img->next != NULL)
			images[i] = img->next;
	}
}

void setLoadProgressCB(void (*func)(int current, int total, void * object), void * object)
{
	loadProgressFunc = func;
	loadProgressObject = object;
}

void releaseDataMemory()
{
	// remove all the stuff from memory
	
	for (int i = 0; i < IMAGECOUNT; i++)
		delete images[i];
	
	for (int i = 0; i < FONTCOUNT; i++)
		delete fonts[i];

#ifndef WINCE
	for (int i = 0; i < SOUNDCOUNT; i++)
		Mix_FreeChunk(sounds[i]);
#endif

}
