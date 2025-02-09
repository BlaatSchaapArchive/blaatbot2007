//---------------------------------------------------------------------------
// Borland compatibility.
#if defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
#define strcasecmp strcmpi
#endif
//---------------------------------------------------------------------------
#include "fcom.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;
//---------------------------------------------------------------------------
cFileCommands::cFileCommands() { ReadFile(); }
//---------------------------------------------------------------------------
void cFileCommands::Clean() {
    while (fcommands.size() > 0) {
        unsigned int i;
        i = fcommands.size() - 1;
        delete[] fcommands[i].command;
        delete[] fcommands[i].action;
        fcommands.erase(fcommands.begin() + i);
    }
}
//---------------------------------------------------------------------------
void cFileCommands::Refresh() {
    Clean();
    ReadFile();
}
//---------------------------------------------------------------------------
cFileCommands::~cFileCommands() { Clean(); }
//---------------------------------------------------------------------------
int cFileCommands::ReadFile() {
    FILE *myfile;
    myfile = fopen("bar.ini", "rb");
    if (myfile) {
        char temp[666];
        int tel = 0;
        char command[128], action[666]; //,finalaction[666]={0};
        memset(command, 0, sizeof(command));
        memset(action, 0, sizeof(action));
        memset(temp, 0, sizeof(temp));

        bool done = false;
        bool nlc = false;
        while (!(feof(myfile))) {
            fread(temp + tel, 1, 1, myfile);
            // tel++;
            if (1 + tel > sizeof(temp))
                tel = 0;
            if (temp[tel] == 0x0A) {
                temp[tel] = 0;
                if (temp[tel - 1] == 0x0D)
                    temp[tel - 1] = 0;
                if (strncmp(temp, "[!", 2) == 0) {
                    char *hulp = strchr(temp, ']');
                    if (hulp)
                        *hulp = 0;
                    strcpy(command, temp + 2);
                    done = false;
                    nlc = true;
                    tel = -1;
                } else if (nlc) {

                    strcpy(action, temp); // zo misschien?
                    done = true;
                    nlc = false;
                } else
                    tel = -1;
            }
            if (done) {
                sCommands temp;
                temp.action = new char[1 + strlen(action)];
                temp.command = new char[1 + strlen(command)];
                strcpy(temp.action, action);
                strcpy(temp.command, command);
                fcommands.push_back(temp);
                done = false;
                memset(command, 0, sizeof(command));
                memset(action, 0, sizeof(action));
                tel = -1;
            }
            tel++;
        }
        fclose(myfile);
    } else
        return 1;
    return 0;
}
//---------------------------------------------------------------------------
char *cFileCommands::Reply(char *command, char *nick, char **pars) {
    printf("filecommand~!\n");
    int a;
    char *temp, *action;

    for (a = 0; (a < fcommands.size()) && (strcasecmp(command, fcommands[a].command) != 0); a++)
        ;
    if (a < fcommands.size()) {
        temp = new char[666];
        strcpy(temp, fcommands[a].action);
    } else
        return NULL;
    action = new char[666];
    strcpy(action, temp);
    char *hulp;
    bool done = false;
    while (!done) {
        hulp = strchr(temp, '$');
        if (hulp) {
            if (*(hulp + 1) == 'n') {
                char deel1[666];
                char deel2[666];
                *hulp = 0;
                strcpy(deel1, temp);
                strcpy(deel2, hulp + 2);
                sprintf(action, "%s%s%s", deel1, nick, deel2);
                strcpy(temp, action);
            } else if ((*(hulp + 1) >= '2') && (*(hulp + 1) <= '9')) {
                char p = *(hulp + 1) - 49;
                char deel1[666];
                char deel2[666];
                *hulp = 0;
                strcpy(deel1, temp);
                strcpy(deel2, hulp + 2);
                sprintf(action, "%s%s%s", deel1, pars[p], deel2);
                strcpy(temp, action);
            } else {
                done = true;
                strcpy(temp, "file error");
            }
        } else
            done = true;
    }
    memset(temp, 0, sizeof(temp));
    delete[] temp;
    return action;
}
//---------------------------------------------------------------------------
