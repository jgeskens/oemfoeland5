/*
 *  OEMFLevel.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <iostream>
#include <fstream>
#include <string>

#include "OEMFLevel.h"
#include "OEMFStorage.h"

#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0

OEMFLevel :: OEMFLevel(char * name, unsigned char width, unsigned char height)
{
	m_width = width;
	m_height = height;
	m_tiles = new tile_t[length()];
	m_name = name;
	m_bg = IMG_CLOUDS2;
	
	for (unsigned int i = 0; i < length(); i++)
	{
		m_tiles[i].bg = IMG_NULL;
		m_tiles[i].fg = IMG_NULL;
		m_tiles[i].type = 0;
	}
	
	types = NULL;
	raster = NULL;
}

OEMFLevel :: OEMFLevel(char * filename)
{	
	if (!filename)
	{
		m_tiles = NULL;
		m_width = 0;
		m_height = 0;
		m_bg = IMG_NULL;
		m_name = "Untitled level";
		return;
	}
	
	ifstream inputstr(filename, ios::binary);
	unsigned char namelen = 0;
	unsigned char width = 0;
	unsigned char height = 0;
	unsigned char bg = IMG_CLOUDS1;
	tile_t * tiles = NULL;
	
	if (!inputstr.eof()) inputstr.read((char *) &namelen, sizeof(unsigned char));
	char * name = new char[namelen+1];
	if (!inputstr.eof()) inputstr.read((char *) name, namelen);
	name[namelen] = 0; // terminate with null char
	if (!inputstr.eof()) inputstr.read((char *) &width, sizeof(unsigned char));
	if (!inputstr.eof()) inputstr.read((char *) &height, sizeof(unsigned char));
 	if (!inputstr.eof()) inputstr.read((char *) &bg, sizeof(unsigned char));
	tiles = new tile_t[(unsigned int) width * (unsigned int) height];
	if (!inputstr.eof()) inputstr.read((char *) tiles, sizeof(tile_t) * (unsigned int) width * (unsigned int) height);
	inputstr.close();
	
	m_tiles = tiles;
	m_width = width;
	m_height = height;
	m_bg = bg;
	m_name = name;
	
	types = NULL;
	raster = NULL;
}

OEMFLevel :: OEMFLevel(OEMFLevel * level)
{
	m_tiles = new tile_t[length()];
	for (unsigned int i = 0; i < level->length(); i++)
	{
		m_tiles[i] = level->tiles()[i];
	}
	
	types = NULL;
	raster = NULL;
}

OEMFLevel :: ~OEMFLevel(void)
{
	delete [] m_tiles;
	
	freeTypesAndRaster();
}

void OEMFLevel :: writeToFile(const char * filename)
{
	if (!filename)
		return;
	
	char namelen = m_name.length();
	ofstream outputstr(filename, ios::binary | ios::trunc);
	
	outputstr.write(&namelen, sizeof(char));
	outputstr.write(m_name.c_str(), namelen);
	outputstr.write((char *) &m_width, sizeof(unsigned char));
	outputstr.write((char *) &m_height, sizeof(unsigned char));
	outputstr.write((char *) &m_bg, sizeof(unsigned char));
	outputstr.write((char *) m_tiles, sizeof(tile_t) * length());
	outputstr.close();
}

void OEMFLevel :: resize(unsigned int neww, unsigned int newh)
{
	if (!(neww > 0 && neww < 256 && newh > 0 && newh < 256))
		return; // nothing to do when user inputs invalid number
	
	unsigned int oldw = width();
	unsigned int oldh = height();
	
	tile_t * newtiles = new tile_t[neww * newh];
	
	for (unsigned int y = 0; y < newh; y++)
		for (unsigned int x = 0; x < neww; x++)
			if (x < oldw && y < oldh) // in old sized level, so copy data
				newtiles[x + y * neww] = m_tiles[x + y * oldw];
			else
			{
				newtiles[x + y * neww].bg = IMG_NULL;
				newtiles[x + y * neww].fg = IMG_NULL;
				newtiles[x + y * neww].type = 0;
			}
	
	m_width = (unsigned char) neww;
	m_height = (unsigned char) newh;
	
	delete [] m_tiles;
	m_tiles = newtiles;
}

void OEMFLevel :: prepareForGame(void)
{
	freeTypesAndRaster();
	
	m_typeCounts = new unsigned int[TYPECOUNT];
	for (unsigned int i = 0; i < TYPECOUNT; i++)
		m_typeCounts[i] = 0;
	
	// prepare raster and count individual types
	raster = new OEMFGObjectList*[width() * height()];
	
	unsigned int x, y, ypos;
	for (y = 0; y < height(); y++)
	{
		ypos = y * width();
		for (x = 0; x < width(); x++)
		{
			raster[x+ypos] = new OEMFGObjectList();
			raster[x+ypos]->objects.push_back(new OEMFGObject(m_tiles[x+ypos].type, m_tiles[x+ypos].fg, x * 32, y * 32, 0));
			m_typeCounts[m_tiles[x+ypos].type] ++;
			if (m_tiles[x+ypos].type == TYPE_START)
			{
				// remember start position
				m_startX = x;
				m_startY = y;
			}
		}
	}
	
	// prepare types
	// make the first level of the types array
	types = new OEMFGObject**[TYPECOUNT];
	for (unsigned int i = 0; i < TYPECOUNT; i++)
		types[i] = new OEMFGObject*[m_typeCounts[i]];
	
	// prepare the indices array
	unsigned int * typeIndices = new unsigned int[TYPECOUNT];
	for (unsigned int i = 0; i < TYPECOUNT; i++)
		typeIndices[i] = 0;
	
	// add the objects to their right array
	for (y = 0; y < height(); y++)
	{
		ypos = y * width();
		for (x = 0; x < width(); x++)
		{
			unsigned int curType = raster[x+ypos]->objects[0]->type();
			types[curType][typeIndices[curType]] = raster[x+ypos]->objects[0];
			typeIndices[curType] ++;
		}
	}
}

void OEMFLevel :: freeTypesAndRaster(void)
{
	// TODO
}

void OEMFLevel :: moveObject(OEMFGObject * obj, int newX, int newY)
{
	int oldIX, oldIY, newIX, newIY;
	deque<OEMFGObject *> :: iterator it;
	
	oldIX = (obj->posX() + 16) / 32;
	oldIY = (obj->posY() + 16) / 32;
	
	newIX = (newX + 16) / 32;
	newIY = (newY + 16) / 32;
	
	if (newIX >= 0 && newIX < (int) width() &&
		newIY >= 0 && newIY < (int) height())
	{
		if (newIX != oldIX || newIY != oldIY)
		{
			bool done = false;
			if (raster[oldIX + width() * oldIY]->objects.size() != 0)
			{
				for (it = raster[oldIX + width() * oldIY]->objects.begin(); !done && it != raster[oldIX + width() * oldIY]->objects.end(); it++)
				{
					// cout << "Number of elements:" << raster[oldIX + width() * oldIY].size() << endl;
					if (*it == obj)
					{
						//int size1 = raster[oldIX + width() * oldIY]->objects.size();
						raster[oldIX + width() * oldIY]->objects.erase(it);
						//int size2 = raster[oldIX + width() * oldIY]->objects.size();
						//cout << "Size1: " << size1 << " Size2: " << size2 << endl;
						done = true;
					}
				}
			}
			raster[newIX + width() * newIY]->objects.push_back(obj);
		}
		obj->setPos(newX, newY);
	}
//	else
//		cout << "Bad things are happening..." << endl;
}

// this function returns the objects below the new position
deque<OEMFGObject *> OEMFLevel :: probePosition(int newX, int newY)
{
	deque<OEMFGObject *> objects;
	deque<OEMFGObject *>::iterator it;
	int x1, x2, y1, y2, ix, iy, x, y, ypos;
	
	ix = (newX + 16) / 32;
	iy = (newY + 16) / 32;
	x1 = (ix > 0 ? ix - 1 : ix);
	x2 = (ix < (int) width()-1 ? ix + 1 : ix);
	y1 = (iy > 0 ? iy - 1 : iy);
	y2 = (iy < (int) height()-1 ? iy + 1 : iy);
	
	for (y = y1; y <= y2; ++y)
	{
		ypos = y * width();
		for (x = x1; x <= x2; ++x)
		{
			for (it = raster[x + ypos]->objects.begin(); it != raster[x + ypos]->objects.end(); it++)
			{
				OEMFGObject * obj2 = (*it);
				if (obj2->type() != TYPE_FREE /* && obj2->type() != TYPE_START */ &&
					(	isWithin(newX, newY, obj2->posX(), obj2->posY())
					||	isWithin(newX + 31, newY, obj2->posX(), obj2->posY())
					||	isWithin(newX, newY + 31, obj2->posX(), obj2->posY())
					||	isWithin(newX + 31, newY + 31, obj2->posX(), obj2->posY())
					))
				{
					objects.push_back(*it);
				}
			}
		}
	}
	return objects;
}

















