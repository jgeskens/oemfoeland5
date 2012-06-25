/*
 *  OEMFGObjectList.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 15/12/06.
 *  Copyright 2006 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFGOBJECTLIST_H__
#define __OEMFGOBJECTLIST_H__

#include <deque>
#include "OEMFGObject.h"

using namespace std;

class OEMFGObjectList
{
	public:
		deque<OEMFGObject *> objects;
};

#endif
