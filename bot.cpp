/*
BlaatSchaap Coding Projects Summer 2007 : IRC BOT IN C / C++


File    : bot.cpp
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


*/

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <vector>
using namespace std;
#include "bot.h"
#include "fcom.h"
#include "general.h"
#include "irc.h"
#include "osinfo.h"

cFileCommands filecommand;
extern IRCclient IRC;
extern cOS os; // uhuh waarom nu ineens een probleem
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

bool cBot::isop(char *channel, char *nick) {
    int a = 0, b = 0, c = 0;
    IRC.getChannelNick(a, b, channel, nick);
    if (a != -1) {
        if (b != -1) {
            while (c < strlen(IRC.channels[a]->users[b]->mode)) {
                // ~(q)&(a)@(o)%(h)
                if (IRC.channels[a]->users[b]->mode[c] == '~')
                    return true;
                if (IRC.channels[a]->users[b]->mode[c] == '&')
                    return true;
                if (IRC.channels[a]->users[b]->mode[c] == '@')
                    return true;
                if (IRC.channels[a]->users[b]->mode[c] == '%')
                    return true;
                c++; // staat hier vanwege mode[c]
            }
        }
    } // printf("\n");
    return false;
}

//------------------------------------------------------------------------------

bool cBot::isop(char *channel) {
    // naar bot
    return isop(channel, IRC.botnick);
}
//------------------------------------------------------------------------------
bool cBot::isService(char *nick) {
    printf("%s ", nick);
    if (strlen(nick) > 4)
        if (strcasecmp(nick + (strlen(nick) - 4), "serv") == 0)
            return true;
    return false;
}
//------------------------------------------------------------------------------

bool cBot::iscsregged(char *mode) {
    // naar bot?
    int c = 0;
    while (c < strlen(mode)) {
        if (mode[c] == 'r')
            return true;
        c++;
    }
    return false;
}

bool cBot::iscsregged(char *channel, char *nick) {
    // naar bot
    int a, b, c;
    IRC.getChannelNick(a, b, channel, nick);
    if (a != -1) {
        if (b != -1) {
            for (c = 0; c < strlen(IRC.channels[a]->users[b]->mode);) {

                if (IRC.channels[a]->users[b]->mode[c] == 'r')
                    return true;
                c++; // staat hier vanwege mode[c]
            }
        } else
            printf("nick not found\n");
    } else
        printf("channel not found\n");
    return false;
}
//------------------------------------------------------------------------------

void cBot::command(int type, char *nick, char *host, char *channel, char *data) {

    if (isService(nick))
        return; // lijkt nu me niet. maar later....

    int a, b;
    IRC.getChannelNick(a, b, channel, nick);
    char *target;
    if (strcasecmp(channel, IRC.botnick) == 0) { /* message in prive */
        target = new char[1 + strlen(nick)];
        strcpy(target, nick);
    } else {
        target = new char[1 + strlen(channel)];
        strcpy(target, channel);
    }

    if (strncmp("test", data, 4) == 0) {
        char temp[128] = "testing, the current botnick is ";
        strcpy(temp + strlen(temp), IRC.botnick);
        IRC.sendPRIVMSG(target, temp);
        delete[] target;
        return;
    }
    if (strncmp("os", data, 2) == 0) {
        char temp[128];
        sprintf(temp, "%s (%s %s %s)", os.name, os.type, os.release, os.machine);
        IRC.sendPRIVMSG(target, temp);
        delete[] target;
        return;
    }

    if (strcasecmp(channel, IRC.botnick) != 0) // message in een channel
        if (strncmp("lines", data, 5) == 0) {
            char *P[3] = {0, 0, 0};
            int NrP;
            spltstr(data, NrP, P, 2);
            a = -1;
            b = -1;
            printf("user:%s channel:%s\n", P[1], channel);
            if (P[1])
                IRC.getChannelNick(a, b, channel, P[1]);
            if (a != -1) {
                if (b != -1) {
                    char temp[128];
                    sprintf(temp, "%s has said %d lines.", P[1], IRC.channels[a]->users[b]->lines);
                    IRC.sendPRIVMSG(target, temp);
                    if (IRC.channels[a]->users[b]->lines < 50) {
                        sprintf(temp, "%s will be voiced after %d lines.", P[1], 50 - IRC.channels[a]->users[b]->lines);
                        IRC.sendPRIVMSG(target, temp);
                    }
                }
                printf("user unknown\n");
            }
            printf("channel unknown\n");
            delete[] target;
            return;
        }
    //------------------------------------------------------------------------------
    if (strcasecmp(channel, IRC.botnick) != 0) // message in een channel
        if (strncmp("seen", data, 4) == 0) {
            char *P[3] = {0, 0, 0};
            int NrP;
            spltstr(data, NrP, P, 2);
            a = -1;
            b = -1;
            printf("user:%s channel:%s\n", P[1], channel);
            if (P[1])
                IRC.getChannelNick(a, b, channel, P[1]);
            if (a != -1) {
                if (b != -1) {
                    char temp[128];
                    sprintf(temp, "%s was seen %d sec ago", P[1], time(NULL) - IRC.channels[a]->users[b]->lasttime);
                    IRC.sendPRIVMSG(channel, temp);

                    switch (IRC.channels[a]->users[b]->lasttype) {
                    case 'J':
                        sprintf(temp, "%s entered the chatroom", P[1]);
                        break;
                    case 'T':
                        sprintf(temp, "%s said %s", P[1], IRC.channels[a]->users[b]->lastsaid);
                        break;
                    case 'A':
                        sprintf(temp, "%s did * %s %s *", P[1], P[1], IRC.channels[a]->users[b]->lastsaid);
                        break;
                    case 'P':
                        sprintf(temp, "%s parted (%s)", P[1], IRC.channels[a]->users[b]->lastsaid);
                        break;
                    case 'Q':
                        sprintf(temp, "%s quit (%s)", P[1], IRC.channels[a]->users[b]->lastsaid);
                        break;
                    case 'K':
                        sprintf(temp, "%s was kicked (%s)", P[1], IRC.channels[a]->users[b]->lastsaid);
                        break;
                    case 'k':
                        sprintf(temp, "%s was killed (%s)", P[1], IRC.channels[a]->users[b]->lastsaid);
                        break;
                    }
                    IRC.sendPRIVMSG(channel, temp);
                } else {
                    char temp[128];
                    sprintf(temp, "I\'m sorry, I haven\'t seen %s", P[1]);
                    IRC.sendPRIVMSG(channel, temp);
                }
            }
            delete[] target;
            return;
        }

    // if (strncmp(":id",data,3)==0) IRC.sendPRIVMSG("nickserv","identify bscp2007");
    // test

    if (strncmp("ul", data, 2) == 0) {
        char temp[128];
        if (strcasecmp(channel, IRC.botnick) != 0)
            sprintf(temp, "Your userlevel is %d, mode %s", IRC.channels[a]->users[b]->userlevel, IRC.channels[a]->users[b]->mode);
        else
            sprintf(temp, "Your userlevel is %d, mode %s", IRC.pm.userlevel, IRC.pm.mode);
        IRC.sendPRIVMSG(target, temp);
        printf(":ul\n");
        delete[] target;
        return;
    }
    int ul = 0;

    if (strcasecmp(channel, IRC.botnick) == 0)
        ul = IRC.pm.userlevel;
    else
        ul = IRC.channels[a]->users[b]->userlevel;
    if (ul > 35) {
        if (strncmp("isclone", data, 7) == 0) {
            char *P[3];
            int NrP;
            spltstr(data, NrP, P, 2);
            IRC.getChannelNick(a, b, channel, P[1]);
            char temp[128] = {0};
            if (a != -1) {
                if (b != -1) {
                    int c;
                    for (c = 0; c < IRC.channels[a]->users.size(); c++) {
                        if (strcmp(IRC.channels[a]->users[c]->nick, IRC.channels[a]->users[b]->nick))
                            if (!strcmp(IRC.channels[a]->users[c]->host, IRC.channels[a]->users[b]->host)) {
                                sprintf(temp, "%s and %s are clones", IRC.channels[a]->users[b]->nick,
                                        IRC.channels[a]->users[c]->nick);
                                IRC.sendPRIVMSG(target, temp);
                                delete[] target;
                                return;
                            }
                    }
                    sprintf(temp, "%s has no clones", IRC.channels[a]->users[b]->nick);
                    IRC.sendPRIVMSG(target, temp);
                    delete[] target;
                    return;
                } else
                    IRC.sendPRIVMSG(target, "Unknown Nick");
            } // channel not found
        }
    }
    if (ul > 50) {
        if (strncmp("nick", data, 4) == 0) {
            char *P[3];
            int NrP;
            spltstr(data, NrP, P, 2);
            IRC.sendNICK(P[1]);
            delete[] target;
            return;
        }

        if (strncmp("join", data, 4) == 0) {
            char *P[3];
            int NrP;
            spltstr(data, NrP, P, 2);
            IRC.joinchannel(P[1]);
            delete[] target;
            return;
        }
        if (strncmp("part", data, 4) == 0) {
            char *P[3];
            int NrP;
            spltstr(data, NrP, P, 2);
            if (NrP)
                IRC.partchannel(P[1]);
            else
                IRC.partchannel(channel);
            delete[] target;
            return;
        }
        if (strncmp("quit", data, 4) == 0) {
            IRC.sendQUIT(NULL);
            delete[] target;
            return;
        }
        if (strncmp("refresh", data, 7) == 0) {
            IRC.sendPRIVMSG(target, "Refreshing File Commands");
            filecommand.Refresh();
            delete[] target;
            return;
        }
    }

    char *P[3] = {NULL, NULL, NULL};
    int NrP;
    spltstr(data, NrP, P, 2);
    // char *etemp=antwoord(P[0]+1, nick ,P);
    char *etemp = filecommand.Reply(P[0], nick, P);
    if (etemp) {
        IRC.sendPRIVMSG(target, etemp);
        delete[] etemp;
        delete[] target;
        return;
    }
    delete[] target;
}
//------------------------------------------------------------------------------
// void verwerk (char type, char *nick, char *host, char *to, char *data){
//        if (strcasecmp(to,botnick)==0){ // verander dit, botnick variable

//}

char cBot::userlevel(char *mode, char *channel, char *nick, char *host) {
    // naar bot ?

    if (strcasecmp(host, "Indre-7A2D8200.xs4all.nl") == 0)
        return 100; // thuis indreanet
    if (strcasecmp(host, "Chat4all-51B52190.xs4all.nl") == 0)
        return 100; // thuis chat4all

    if (strcasecmp(channel, "") != 0) // pm
        if (isop(channel, nick))
            return 50; //->test

    //    if (iscsregged(mode)) return 25; //debug??

    if ((iscsregged(mode)) & (!(strcasecmp(nick, "andre"))))
        return 100;
    if ((iscsregged(mode)) & (!(strcasecmp(nick, "andrevs"))))
        return 100;

    return 0;
}
