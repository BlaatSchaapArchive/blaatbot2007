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




*/
#include "osinfo.h"

//------------------------------------------------------------------------------
// Open Watcom compiler heeft geen __WIN32__ gedefineerd
#ifdef __NT__
#define __WIN32__
#endif
//------------------------------------------------------------------------------
#if defined(__WIN32__) || defined(__WIN64__) // 32 of 64

#include <stdio.h>
#include <string.h>
#include <windows.h>

cOS::cOS() {

    char ostype_[128] = "Unknown";
    char osname_[128] = "Unknown";
    char osrelease_[128] = "Unknown";
    char machine_[128] = "Unknown";

    OSVERSIONINFO winver;
    ZeroMemory(&winver, sizeof(OSVERSIONINFO));
    winver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&winver);
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    switch (winver.dwPlatformId) {
    case 0:
        strcpy(ostype_, "Win32s");
        break;
    case 1:
        strcpy(ostype_, "Windows 9x");
        break;
    case 2:
        strcpy(ostype_, "Windows NT");
        break;
    case 3:
        strcpy(ostype_, "Windows CE");
        break;
    default:
        strcpy(ostype_, "Unknown");
    }

    sprintf(osrelease_, "%d.%d.%d", winver.dwMajorVersion, winver.dwMinorVersion, winver.dwBuildNumber);

    if ((winver.dwMajorVersion < 5) && (winver.dwPlatformId == 2)) {
        sprintf(osname_, "Windows NT %s %s", osrelease_, winver.szCSDVersion);
    }
    if ((winver.dwMajorVersion == 5) && (winver.dwMinorVersion == 0) && (winver.dwPlatformId == 2)) {
        sprintf(osname_, "Windows 2000 %s", winver.szCSDVersion);
    }
    if ((winver.dwMajorVersion == 5) && (winver.dwMinorVersion == 1) && (winver.dwPlatformId == 2)) {
        sprintf(osname_, "Windows XP %s", winver.szCSDVersion);
    }
    if ((winver.dwMajorVersion == 5) && (winver.dwMinorVersion == 2) && (winver.dwPlatformId == 2)) {
        if (si.wProcessorArchitecture)
            sprintf(osname_, "Windows XP %s", winver.szCSDVersion);
        // 64 bit windows XP
        else
            sprintf(osname_, "Windows 2003 %s", winver.szCSDVersion);
    }
    if ((winver.dwMajorVersion == 6) && (winver.dwMinorVersion == 0) && (winver.dwPlatformId == 2)) {
        sprintf(osname_, "Windows Vista %s", winver.szCSDVersion);
    }

    if ((winver.dwMajorVersion < 4) && (winver.dwMinorVersion == 0) && (winver.dwPlatformId == 1)) {
        sprintf(osname_, "Windows 95 %s", winver.szCSDVersion);
    }
    if ((winver.dwMajorVersion < 4) && (winver.dwMinorVersion == 10) && (winver.dwPlatformId == 2)) {
        sprintf(osname_, "Windows 98 %s", winver.szCSDVersion);
    }
    if ((winver.dwMajorVersion < 4) && (winver.dwMinorVersion == 90) && (winver.dwPlatformId == 2)) {
        sprintf(osname_, "Windows ME %s", winver.szCSDVersion);
    }
    if ((winver.dwPlatformId == 1)) {
        sprintf(osname_, "Win32s %s", winver.szCSDVersion);
    }

    switch (si.wProcessorArchitecture) {
        // current CPU Architectures
    case PROCESSOR_ARCHITECTURE_INTEL: // 0
        strcpy(machine_, "i386");
        break;
    case PROCESSOR_ARCHITECTURE_IA64: // 6
        strcpy(machine_, "IA64");
        break;
    case PROCESSOR_ARCHITECTURE_AMD64: // 9
        strcpy(machine_, "x86-64");
        break;
        // obsolete CPU Architectures (NT 3)
    case PROCESSOR_ARCHITECTURE_MIPS: // also used in WinCE devices?
        strcpy(machine_, "MIPS");
        break;
    case PROCESSOR_ARCHITECTURE_ALPHA:
        strcpy(machine_, "Alpha");
        break;
    case PROCESSOR_ARCHITECTURE_PPC:
        strcpy(machine_, "PowerPC");
        break;
        // Mobile CPU Architectures (WinCE)
        // PROCESSOR_ARCHITECTURE_MIPS already in obsolete
    case PROCESSOR_ARCHITECTURE_SHX:
        strcpy(machine_, "SHX");
        break;
    case PROCESSOR_ARCHITECTURE_ARM:
        strcpy(machine_, "ARM");
        break;
    default:
        strcpy(machine_, "Unknown");
    }

    type = new char[strlen(ostype_) + 1];
    strcpy(type, ostype_);
    name = new char[strlen(osname_) + 1];
    strcpy(name, osname_);
    release = new char[strlen(osrelease_) + 1];
    strcpy(release, osrelease_);
    machine = new char[strlen(machine_) + 1];
    strcpy(machine, machine_);
}
#elif defined(__OS2__)
// OS/2
#include <stdio.h>
#include <string.h>

cOS::cOS() {
    // moet nog onderzoeken hoe dit onder OS/2 werkt. Heb momenteel geen
    // OS/2 installatie.
    type = new char[5];
    strcpy(type, "OS/2");
    name = new char[5];
    strcpy(name, "OS/2");
    release = new char[4];
    strcpy(release, "N/A");
    machine = new char[4];
    strcpy(machine, "N/A");
}

#elif defined(__WINDOWS__)
// Win16

#include <stdio.h>
#include <string.h>

cOS::cOS() {

    // ff onderzoeken

    type = new char[6];
    strcpy(type, "Win16");
    name = new char[6];
    strcpy(name, "Win16");
    release = new char[4];
    strcpy(release, "N/A");
    machine = new char[4];
    strcpy(machine, "N/A");
}

#else
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

cOS::cOS() {
    FILE *myfile;

    system("uname -s >./ostype");
    system("uname -r >./osrelease");
    system("uname -m >./machine");

    myfile = fopen("./ostype", "rt");
    if (myfile) {
        fseek(myfile, 0, 2);
        int filesize = ftell(myfile);
        fseek(myfile, 0, 0);
        type = new char[filesize + 1];
        fread(type, filesize, 1, myfile);
        fclose(myfile);
        remove("./ostype");
    } else {
        type = new char[8];
        strcpy(type, "Unknown");
    }

    myfile = fopen("./osrelease", "rt");
    if (myfile) {
        fseek(myfile, 0, 2);
        int filesize = ftell(myfile);
        fseek(myfile, 0, 0);
        release = new char[filesize + 1];
        fread(release, filesize, 1, myfile);
        fclose(myfile);
        remove("./osrelease");
    } else {
        release = new char[8];
        strcpy(release, "Unknown");
    }

    myfile = fopen("./machine", "rt");
    if (myfile) {
        fseek(myfile, 0, 2);
        int filesize = ftell(myfile);
        fseek(myfile, 0, 0);
        machine = new char[filesize + 1];
        fread(machine, filesize, 1, myfile);
        fclose(myfile);
        remove("./machine");
    } else {
        machine = new char[8];
        strcpy(machine, "Unknown");
    }

    type[strlen(type) - 1] = 0;
    release[strlen(release) - 1] = 0;
    machine[strlen(machine) - 1] = 0;
    name = new char[strlen(type) + strlen(release) + 2];
    strcpy(name, type);
    name[strlen(name) + 1] = 0x00;
    name[strlen(name)] = 0x20;
    strcpy(name + strlen(name), release);
}
#endif

cOS::~cOS() {
    delete[] type;
    delete[] release;
    delete[] name;
    delete[] machine;
}
