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
#include <iostream>
#include <time.h> 
#include <stdio.h>
#include <vector>
using namespace std;
#include "bot.h"
#include "osinfo.h"
#include "general.h"
#include "irc.h"

extern vector <ircchannel*> channels;
//------------------------------------------------------------------------------	
//------------------------------------------------------------------------------

bool isop(char *channel, char *nick){
	int a=0,b=0,c=0;
	getChannelNick(a,b,channel,nick);
	if ( a != -1) { 
		if ( b != -1 ) {
			while ( c < strlen(channels[a]->users[b]->mode) ){
				// ~(q)&(a)@(o)%(h)
                if (channels[a]->users[b]->mode[c]== '~') return true;
				if (channels[a]->users[b]->mode[c]== '&') return true;
				if (channels[a]->users[b]->mode[c]== '@') return true;
				if (channels[a]->users[b]->mode[c]== '%') return true;
				c++; // staat hier vanwege mode[c]
			} 
		} 
	}// printf("\n");
    return false;
} 
//------------------------------------------------------------------------------

bool botisop(char *channel){
     // naar bot
	return isop(channel, botnick);
}
//------------------------------------------------------------------------------

bool iscsregged(char *mode){
     // naar bot?
	int c=0;
	while ( c < strlen(mode)){
		if (mode[c]== 'r') return true;
		c++; 
	}
return false;
}

bool iscsregged(char *channel,char *nick){	
     //naar bot
	int a,b,c;
	bool result=false;
	getChannelNick(a,b,channel,nick);
	if ( a != -1) { 
		if ( b != -1 ) {
			for ( c = 0; c < strlen(channels[a]->users[b]->mode);){
				
				result = result || (channels[a]->users[b]->mode[c]== 'r');
				c++; // staat hier vanwege mode[c]
			}
		}else printf("nick not found\n");
	}else printf("channel not found\n");
    return result;
}
//------------------------------------------------------------------------------

void botcommand(int type,char *nick, char *host, char *channel, char *data){
	int a,b;
	getChannelNick(a,b,channel,nick); 	
			
	if (strncmp("!test",data,5)==0){
		char temp[128]="testing, the current botnick is ";
		strcpy(temp+strlen(temp),botnick);
		sendPRIVMSG(channel,temp);

		return;
	}
	if (strncmp("!os",data,3)==0){
		char temp[128];
		sprintf(temp,"%s (%s %s %s)",osname, ostype, osrelease, machine);
		sendPRIVMSG(channel,temp);
		return;
	}	
	
		if (strncmp("!seen",data,5)==0){
			char *P[3];	int NrP;
			spltstr(data,NrP,P,2);
	    a=-1;b=-1;
			printf("user:%s channel:%s\n",P[1],channel);
		if (data+5!=NULL) getChannelNick(a,b,channel,P[1]);
			if ( a != -1){
				if ( b != -1){
					char temp[128];
					sprintf(temp,
		"%s was seen %d sec ago",
			   P[1],time(NULL)-channels[a]->users[b]->lasttime);
				sendPRIVMSG(channel,temp);
			
			switch (channels[a]->users[b]->lasttype){
			case 'J' : 
				sprintf(temp,"%s entered the chatroom", P[1]);
				break;
			case 'T' : 
				sprintf(temp,"%s said %s",
				       P[1],channels[a]->users[b]->lastsaid) ;
				break;
			case 'A' : 
				sprintf(temp,"%s did * %s %s *",
				       P[1],P[1],channels[a]->users[b]->lastsaid) ;
				break;
			case 'P' : 
				sprintf(temp,"%s parted (%s)",
			    P[1],channels[a]->users[b]->lastsaid);
				break;
			case 'Q' : 
				sprintf(temp,"%s quit (%s)",
			    P[1],channels[a]->users[b]->lastsaid);
				break;
			case 'K' : 
				sprintf(temp,"%s was kicked (%s)",
			    P[1],channels[a]->users[b]->lastsaid);
				break;
			case 'k' : 
				sprintf(temp,"%s was killed (%s)",
			    P[1],channels[a]->users[b]->lastsaid);
				break;
			}
				sendPRIVMSG(channel,temp);
			}else 
			{
				char temp[128];
				sprintf(temp,"I\'m sorry, I haven\'t seen %s",P[1]);
				sendPRIVMSG(channel,temp);
			}
			
		}
		return;
	}		
	
	if (strncmp("!ul",data,3)==0){
		char temp[128];
		sprintf(temp,"Your userlevel is %d, mode %s",
                    channels[a]->users[b]->userlevel,
                    channels[a]->users[b]->mode);
		sendPRIVMSG(channel,temp);
	}
	if (channels[a]->users[b]->userlevel > 50 ){
		if (strncmp("!nick",data,5)==0){
            char *P[3];	int NrP;
			spltstr(data,NrP,P,2);
            sendNICK(P[1]);
        }
	
		if (strncmp("!join",data,5)==0){
            char *P[3];	int NrP;
			spltstr(data,NrP,P,2);
			joinchannel(P[1]);
		}
		if (strncmp("!part",data,5)==0){
            char *P[3];	int NrP;
			spltstr(data,NrP,P,2);
			if (NrP) partchannel(P[1]); else partchannel(channel);
		}
	}
}
//------------------------------------------------------------------------------
void verwerk (char type, char *nick, char *host, char *to, char *data){
	//		if (strcasecmp(to,botnick)==0){ // verander dit, botnick variable
	int a,b;
	if (to) 
		getChannelNick(a,b,to,nick);
	//
    
	
	if (type==PMES || type==PAMS || type==NOTP ){
		if (type==PMES) { //MESSAGE PRIVÉ
			//fprintf(channels[a]->logfile,">%s< %s \n",nick,data);			
			//fflush(channels[a]->logfile);
			sendPRIVMSG(nick,"PM not implemented yet");
		}			
		if (type==PAMS) { //ACTION PRIVÉ
			//fprintf(channels[a]->logfile,">%s %s<\n",nick,data);			
			//fflush(channels[a]->logfile);
			sendPRIVMSG(nick,"PM not implemented yet");
		}				
		if (type==NOTP) { //NOTICE PRIVÉ
			//fprintf(channels[a]->logfile,"#%s# %s \n",nick,data);			
			//fflush(channels[a]->logfile);
			if ( (strcasecmp(nick,"NickServ")!=0) 
              && (strcasecmp(nick,"ChanServ")!=0)) // --> aanpassen naar *Serv?
				sendPRIVMSG(nick,"Private Notices not implemented yet");
		}
	}
	if (type==CMES || type==AMES|| type==NOTC ){
		if ( a == -1 ) return;
		if ( b == -1 ) return;
		delete (channels[a]->users[b]->lastsaid); 
		channels[a]->users[b]->lasttype=type;
		channels[a]->users[b]->lastsaid = new char[1+strlen(data)];
	    strcpy(channels[a]->users[b]->lastsaid,data);
		channels[a]->users[b]->lasttime = time(NULL);
		channels[a]->users[b]->lines++;
		if (type==CMES) {//MESSAGE CHANNEL
			fprintf(channels[a]->logfile,"<%s> %s\n",nick,data);
			fflush(channels[a]->logfile);
			channels[a]->users[b]->lasttype = 'T';
			if (data[0]=='!') botcommand(type,nick, host, to, data);
		}
		if (type==AMES) {//ACTION CHANNEL
			fprintf(channels[a]->logfile,"* %s %s *\n",nick,data);			
			fflush(channels[a]->logfile);
			channels[a]->users[b]->lasttype = 'A';
		}
		if (type==NOTC) { //NOTICE CHANNEL
			fprintf(channels[a]->logfile,"-%s- %s\n",nick,data);
			fflush(channels[a]->logfile);
			channels[a]->users[b]->lasttype = 'N';
		}
		if (channels[a]->users[b]->lines==50) {//maak setbaar
			setmode(to,nick,"+v");
		}
		printf("%s heeft al %d keer iets gezegd.\n",
			          channels[a]->users[b]->nick,channels[a]->users[b]->lines);
	}
}
