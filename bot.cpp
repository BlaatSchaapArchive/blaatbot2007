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
		"%s is %d sec geleden voor het laatst actief geweest",
			   P[1],time(NULL)-channels[a]->users[b]->lasttime);
				sendPRIVMSG(channel,temp);
			
			switch (channels[a]->users[b]->lasttype){
			case 'J' : 
				sprintf(temp,"%s kwam toen binnen", P[1]);
				break;
			case 'T' : 
				sprintf(temp,"%s zei toen %s",
				       P[1],channels[a]->users[b]->lastsaid) ;
				break;
			case 'A' : 
				sprintf(temp,"%s deed toen * %s %s *",
				       P[1],P[1],channels[a]->users[b]->lastsaid) ;
				break;
			case 'P' : 
				sprintf(temp,"%s ging toen weg (%s)",
			    P[1],channels[a]->users[b]->lastsaid);
				break;
			case 'Q' : 
				sprintf(temp,"%s ging toen weg (%s)",
			    P[1],channels[a]->users[b]->lastsaid);
				break;}
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
		sprintf(temp,"Your userlevel is %d",channels[a]->users[b]->userlevel);
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
	}
}
