/*
 *  OEMFLevel.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 27/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFLEVEL_H__
#define __OEMFLEVEL_H__

#include <string>
#include <deque>
#include "OEMFGObject.h"
#include "OEMFGObjectList.h"

using namespace std;

typedef struct tile_s
{
	unsigned char type;
	unsigned char fg;
	unsigned char bg;
} tile_t;

class OEMFLevel
{
public:
	// constructors, destructor
	OEMFLevel(const char * name, unsigned char width, unsigned char height);
	OEMFLevel(const char * filename);
	OEMFLevel(OEMFLevel * level);
	~OEMFLevel(void);
	
	// file io
	void writeToFile(const char * filename);
	
	// getters and setters
	tile_t * tiles(void) { return m_tiles; };
	unsigned int width(void) const { return (unsigned int) m_width; };
	unsigned int height(void) const { return (unsigned int) m_height; };
	unsigned int length(void) const { return m_width * m_height; };
	string name(void) const { return m_name; };
	void setName(string newname) { m_name = newname; };
	unsigned int getTileType(unsigned int x, unsigned int y) const { return m_tiles[x + y * width()].type; };
	unsigned int getTileFG(unsigned int x, unsigned int y) const { return m_tiles[x + y * width()].fg; };
	unsigned int getTileBG(unsigned int x, unsigned int y) const { return m_tiles[x + y * width()].bg; };
	void resize(unsigned int neww, unsigned int newh);
	void setBackground(unsigned int bg) { m_bg = (unsigned char) bg; };
	unsigned int background(void) const { return (unsigned int) m_bg; };
	unsigned char startX() const { return m_startX; };
	unsigned char startY() const { return m_startY; };
	
	void prepareForGame(void);
	void freeTypesAndRaster(void);
	void moveObject(OEMFGObject * obj, int newX, int newY);
	deque<OEMFGObject *> probePosition(int newX, int newY);
	bool isWithin(int x1, int y1, int x2, int y2) { return (x1 >= x2 && x1 < x2 + 32 && y1 >= y2 && y1 < y2 + 32); };
	
	// in-game variables, performance is necessary
	OEMFGObject * ** types;
	OEMFGObjectList * * raster;
	unsigned int * m_typeCounts;
protected:
	tile_t * m_tiles;
	unsigned char m_width;
	unsigned char m_height;
	unsigned char m_bg;
	string m_name;
	unsigned char m_startX;
	unsigned char m_startY;
};

#endif
