/*
BlaatSchaap Coding Projects Summer 2007 : SPLIT IRC DATA


File    : general.cpp
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


  stripmirc;

*/

#include <stdio.h>
#include <string.h>

void strip_mIRC(char *data) {

    char *cleandata = new char[1 + strlen(data)];
    memset(cleandata, 0, 1 + strlen(data)); // iig
    // memset (cleandata,0,sizeof(cleandata)); //iig
    //  zeker voldoende geheugen ...
    signed int dpos = 0, cpos = 0, count;
    while (dpos < strlen(data)) {

        if (data[dpos] == 3) {
            dpos++;
            count = 0;
            while (dpos < strlen(data) && count < 2 && data[dpos] > 0x29 && data[dpos] < 0x40) {
                dpos++;
                count++;
            }
        } else if (data[dpos] < 0x20)
            dpos++;
        cleandata[cpos] = data[dpos];
        cpos++;
        dpos++;
    }
    if (cleandata[0] == ' ') {
        strcpy(data, cleandata + 1);
    } else
        strcpy(data, cleandata);
    printf("%s\n", data);
}
void spltstr(char *data, int &NrParam, char **Param, char nrspace) {

    // char teller=0;
    char *Ptr = data;
    bool done = false;
    NrParam = 0;
    Param[NrParam] = data;
    while (*Ptr != 0) {

        if (*Ptr == 0x0A)
            *Ptr = 0x00;
        if (*Ptr == 0x0D)
            *Ptr = 0x00;
        if (*Ptr == 0x20 && !done) {
            *Ptr = 0x00;
            NrParam++;
            Param[NrParam] = Ptr + 1;
            if (nrspace == NrParam)
                done = true;
        }
        if (*Ptr == ':' && NrParam > 2 && !done) {
            done = true;
            Param[NrParam] = Ptr + 1;
        }
        Ptr++;
    }
}
