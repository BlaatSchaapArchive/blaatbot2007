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

TODO : 
	Strip mIRC colors ( general.cpp )
	seen commands : tijd in uu:mm:ss ( bot.cpp ) (bestaat hier een functie voor)
	Server en Channels in files
	Userlevels in files / Security model uitwerken
	IRCuser / IRCchannel naar klasses omschrijven.
	Activeringsteken instelbaar maken
	
	
	
	

*/

//------------------------------------------------------------------------------
#include <cstdlib>
#include <iostream>
#include <time.h> 
#include <stdio.h>
#include <vector>

#include "fcom.h"

using namespace std;

//char *ostype,*osrelease,*osname,*machine;

//------------------------------------------------------------------------------

#include "osinfo.h"
#include "irc.h"

cOS os;
IRCclient IRC;
//------------------------------------------------------------------------------
int main(int argc, char *argv[]){
//    getOSinfo(ostype,osrelease,osname,machine);
    int port = 6667;
    
//    ReadFile();
//#ifdef indreanet
//   char *ip="195.28.165.175";
//#elifdef chat4all
   char *ip="85.25.252.111";
//#endif    
    
    if (!(IRC.connect_irc(ip,port))){                            
        IRC.login ();
//        IRC.receivedata();
        
    printf("Connection lost\n");
   // delete[] ostype;delete[] osrelease;delete[] osname;delete[] machine;

    }
    else printf("Connection error!\n");
}
//------------------------------------------------------------------------------
