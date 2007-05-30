
/*
BlaatSchaap Coding Projects Summer 2007 : Detect Current OS


File    : osinfo.cpp
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
     *  Implement M$-Windows-syle Operating Systems



*/


#ifdef __WIN32__              // compile for WIN32
// insert winblows code here
void getOSinfo(char *&ostype, char *&osrelease, char *&osname, char *&machine){
}
#else
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

void getOSinfo(char *&ostype, char *&osrelease, char *&osname, char *&machine){
	FILE *myfile;
	
	system("uname -s >./ostype");
	system("uname -r >./osrelease");
	system("uname -m >./machine");
	
	myfile= fopen ("./ostype","rt");	
	if (myfile){
		fseek(myfile,0,2);
		int filesize=ftell(myfile);
		fseek(myfile,0,0);
		ostype = new char[filesize+1];
		fread(ostype,filesize,1,myfile);
		fclose(myfile);
		remove("./ostype");
	}
	else{
		ostype = new char[8];
		strcpy(ostype,"Unknown");
	}
	
	myfile= fopen ("./osrelease","rt");	
	if (myfile){
		fseek(myfile,0,2);
		int filesize=ftell(myfile);
		fseek(myfile,0,0);
		osrelease = new char[filesize+1];
		fread(osrelease,filesize,1,myfile);
		fclose(myfile);
		remove("./osrelease");
	}
	else{
		osrelease = new char[8];
		strcpy(osrelease,"Unknown");
	}
	
	myfile= fopen ("./machine","rt");	
	if (myfile){
		fseek(myfile,0,2);
		int filesize=ftell(myfile);
		fseek(myfile,0,0);
		machine = new char[filesize+1];
		fread(machine,filesize,1,myfile);
		fclose(myfile);
		remove("./machine");
	}
	else{
		machine = new char[8];
		strcpy(machine,"Unknown");
	}
	
	
	ostype[strlen(ostype)-1]=0;
	osrelease[strlen(osrelease)-1]=0;
	machine[strlen(machine)-1]=0;
	osname= new char [strlen(ostype) + strlen(osrelease) + 2];
    strcpy(osname,ostype);
	machine[strlen(machine)+1]=0x00;
	machine[strlen(machine)]=0x20;
	strcpy(osname+strlen(osname),osrelease);
	
	
	//printf("OSTYPE = %s\n",ostype);
	//printf("OSRELEASE = %s\n",osrelease);
	//printf("MACHINE = %s\n",machine);
	
	//delete ostype;
	//delete osrelease;
	//delete machine;
}
#endif
