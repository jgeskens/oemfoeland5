/*
 *  OEMFLevelEdit.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFLEVELEDIT_H__
#define __OEMFLEVELEDIT_H__

#include <string>

#include "OEMFEnvironment.h"
#include "OEMFLevel.h"

using namespace std;

class OEMFLevelEdit : public OEMFEnvironment
{
public:
	OEMFLevelEdit(SDL_Surface * screen, char * execPath, unsigned int screenWidth, unsigned int screenHeight, unsigned int screenBpp);
	~OEMFLevelEdit(void);
	
	virtual void run(void);
protected:
	void drawInterface(unsigned int left, unsigned int top, unsigned int width, unsigned int height);
	void chooseBackground();
	unsigned int chooseTile(unsigned int index);
	void resizeDialog(void);
	void importLevel();
	
	string m_filename;
	OEMFLevel * m_level;
	unsigned int * m_images;
	unsigned int m_imageCount;
	unsigned int m_currentImage;
	bool m_background;
	unsigned int m_typeListCount;
	string * m_typeList;
	unsigned int m_currentType;
	unsigned int m_beginX;
	unsigned int m_beginY;
	unsigned int m_posX;
	unsigned int m_posY;
	unsigned int m_mouseX;
	unsigned int m_mouseY;
	unsigned int m_originX;
	unsigned int m_originY;
	bool m_drawRect;
};

#endif
