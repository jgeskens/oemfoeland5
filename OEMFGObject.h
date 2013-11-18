/*
 *  OEMFGObject.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 2/01/06.
 *  Copyright 2006 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFGOBJECT_H__
#define __OEMFGOBJECT_H__

#include <iostream>

class OEMFGObject
{
public:
	OEMFGObject(unsigned char type, unsigned char image, int posX, int posY, unsigned char flag)
	{
		m_type = type;
		m_image = image;
		m_posX = posX;
		m_posY = posY;
		m_flag = flag;
		m_ax = m_ay = m_vx = m_vy = 0.0f;
	};
	
	// getters
	unsigned char type(void) const { return m_type; };
	unsigned char image(void) const { return m_image; };
	int posX(void) const { return m_posX; };
	int posY(void) const { return m_posY; };
	unsigned char flag(void) const { return m_flag; };
	
	// setters
	void setType(unsigned char type) { m_type = type; };
	void setImage(unsigned char image) { m_image = image; };
	void setPos(int x, int y) { m_posX = x; m_posY = y; };
	void setPosX(int x) { m_posX = x; };
	void setPosY(int y) { m_posY = y; };
	void setFlag(unsigned char flag) { m_flag = flag; };
	
	// physics helpers
	float m_ax;
	float m_ay;
	float m_vx;
	float m_vy;
protected:
	unsigned char m_type;
	unsigned char m_image;
	int m_posX;
	int m_posY;
	unsigned char m_flag;
};

#endif
