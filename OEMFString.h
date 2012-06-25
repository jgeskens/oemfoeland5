/*
 *  OEMFString.h
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 22/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#ifndef __OEMFSTRING_H__
#define __OEMFSTRING_H__

class OEMFString
{
public:
	OEMFString(void);
	OEMFString(const char * cstr);
	OEMFString(const OEMFString & str);
	~OEMFString(void);
	
	// selectors
	char * cStr(void) const { return m_cstr; };
	OEMFString getPathToFileName(char * executablepath) const;
protected:
	char * m_cstr;
};

#endif
