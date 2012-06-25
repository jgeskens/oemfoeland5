/*
 *  OEMFString.cpp
 *  Oemfoeland 5
 *
 *  Created by Jef Geskens on 22/12/05.
 *  Copyright 2005 Jef Geskens. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

#include "OEMFString.h"

using namespace std;

OEMFString :: OEMFString(void)
{
	m_cstr = new char[1];
	m_cstr[0] = 0;
}

OEMFString :: OEMFString(const char * cstr)
{
	if (cstr)
	{
		m_cstr = new char[strlen(cstr) + 1];
		memcpy(m_cstr, cstr, strlen(cstr) + 1);
	}
	else
	{
		m_cstr = new char[1];
		m_cstr[0] = 0;
	}
}

OEMFString :: OEMFString(const OEMFString & str)
{
	m_cstr = new char[strlen(str.m_cstr) + 1];
	memcpy(m_cstr, str.m_cstr, strlen(str.m_cstr) + 1);
}

OEMFString :: ~OEMFString(void)
{
	delete [] m_cstr;
}

OEMFString OEMFString :: getPathToFileName(char * executablepath) const
{
	#if defined (__APPLE__) || defined (MACOSX)	

		string newstr(executablepath);
		int lastslashpos = newstr.find_last_of("/", newstr.length()-1);
		string pathstr(executablepath, 0, lastslashpos);
		pathstr = pathstr + "/../Resources/";
		pathstr = pathstr + m_cstr;
		string newpathstr;
		for (unsigned int i = 0; i < pathstr.length(); i++)
		{
			if (pathstr[i] == ' ' && 0)
			{
				newpathstr = newpathstr + "\\ ";
			}
			else
			{
				newpathstr = newpathstr + pathstr[i];
			}
		}
		
		return OEMFString(newpathstr.c_str());
//		char * newcstr = new char[4096];
//		int pathlen = strlen(executablepath);
//		int cntntrsrclen = strlen("../Contents/Resources/");
//		int selflen = strlen(m_cstr);
//		memcpy(newcstr, executablepath, pathlen);
//		memcpy(newcstr + pathlen, "../Contents/Resources/", cntntrsrclen);
//		memcpy(newcstr + pathlen + cntntrsrclen, m_cstr, selflen);
//		newcstr[pathlen + cntntrsrclen + selflen] = 0;
//		OEMFString newstr(newcstr);
//		// crash!
//		// *((int *) NULL) = 1;
//		delete [] newcstr;
//		return newstr;
	#else
		return OEMFString(m_cstr);
	#endif
}

