//#define indreanet
//#define loud

/*
BlaatSchaap Coding Projects Summer 2007 : IRC BOT IN C / C++


File    : main.cpp
License : Zlib
--------------------------------------------------------------------------------
Copyright (c) 2007 André van Schoubroeck

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
--------------------------------------------------------------------------------

    
            '438' = nick niet gewijzigd 'change too fast'
            '432' = nick niet gewijzigd 'illegal chars'
                


*/

//------------------------------------------------------------------------------
#include <cstdlib>
#include <iostream>
#include <time.h> 
#include <stdio.h>
#include <vector>

using namespace std;
char *ostype,*osrelease,*osname,*machine;

// for now --> remove after clasification.
char *botnick;
#ifdef __WIN32__
SOCKET sServer;
#else
int sServer;
#endif
//------------------------------------------------------------------------------

#include "osinfo.h"
#include "irc.h"


//------------------------------------------------------------------------------
int main(int argc, char *argv[]){
    getOSinfo(ostype,osrelease,osname,machine);
    int port = 6667;
#ifdef indreanet	
    char *ip="195.28.165.175";
#else		
    char *ip="62.75.201.175";
#endif	
	if (!(connect_irc(ip,port)))
	{
	botnick = new char[1+sizeof("bscp-test")];
	strcpy(botnick,"bscp-test");
        login ();
        receivedata();
	printf("Connection lost\n");
	delete ostype;delete osrelease;delete osname;delete machine;

	}
}
//------------------------------------------------------------------------------
