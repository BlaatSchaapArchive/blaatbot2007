/*
BlaatSchaap Coding Projects Summer 2007 : IRC PROTOCOL IMPELENTATION IN C / C++


File    : irc.cpp
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

            '305' = back 
			'306' = away
            
			'421' = unknown command
			
			'438' = bick change too fast
            '432' = illegal chars in nick
			

*/

#include <cstdlib>
#include <iostream>
#include <time.h> 
#include <stdio.h>
#include <vector>

using namespace std;

#include "irc.h"
#include "bot.h"
#include "osinfo.h"
#include "general.h"
#include "main.h"

//vector <ircchannel*> channels;
//char *botnick;
//SOCKET sServer;

// IRCclient::


//------------------------------------------------------------------------------
void IRCclient::irc_message (char type, char *nick, char *host, char *to, char *data){
		int a,b;
	if (to) 
		getChannelNick(a,b,to,nick);
	
	if (type==PMES || type==PAMS || type==NOTP ){
		prive = fopen (nick,"a");
		
		splituserhost(host,pm.user,pm.host);
		pm.nick = nick;
		//pm.mode = ?? --> is user csregged? geen check nu
		pm.userlevel=userlevel("","",pm.nick,pm.host);
		
		if (type==PMES) { //MESSAGE PRIVÉ
			
			fprintf(prive,"<%s> %s \n",nick,data);		

			// copy-paste van CMES
			if (data[0]=='!') botcommand(type,nick, host, to, data);			
			
			
			//if (!(isService(nick)))
			//	sendPRIVMSG(nick,"PM not implemented yet");
		}			
		if (type==PAMS) { //ACTION PRIVÉ
			
			fprintf(prive,"*%s %s*\n",nick,data);			
			
			//if (!(isService(nick)))
			//	IRC.sendPRIVMSG(nick,"PM not implemented yet");
		}				
		if (type==NOTP) { //NOTICE PRIVÉ
			
			fprintf(prive,"#%s# %s \n",nick,data);			\
			
			
			//if (!(isService(nick)))
			//		sendPRIVMSG(nick,"Private Notices not implemented yet");
		}
		fclose(prive);
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
			
			// uhm dit moet uit IRCclient en naar ergens de botcode
			// dus ... uhm ... moet de hele string gepasst worden
			// naar de botcode?
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
	
	
	
	
	
	
	if (type==JOIN){
		printf("%s joined %s\n",nick, to);
		char temp[128];
		sprintf(temp,"WHO %s\xd\xa",to);
		send(sServer,temp,strlen(temp),0);

	}
	
	if (type==MODE){
		printf("%s in %s sets MODE %s\n",nick, to, data);
		// verder uitwerken //
        //update userlist.
		char temp[128];
		sprintf(temp,"WHO %s\xd\xa",to);
		send(sServer,temp,strlen(temp),0);
    }
	if (type==NICK){
		if (strcasecmp(botnick,nick)==0){
			delete botnick;
			botnick = new char[1+strlen(data)];
			strcpy(botnick,data);
		}
		int a=0,b;
		
		while ( a < channels.size() ){
			for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcasecmp (nick ,channels[a]->users[b]->nick) != 0); b++);
			if ( b < channels[a]->users.size() ) {// user bestaat in channel;
				
				if (channels[a]->users[b]->oldnick!=NULL) 
										delete channels[a]->users[b]->oldnick;
				channels[a]->users[b]->oldnick=channels[a]->users[b]->nick;
				channels[a]->users[b]->nick= new char[1+strlen(data)];
				strcpy(channels[a]->users[b]->nick,data);
			}
			a++;
		}
	}
	
	if (type==PART){
		printf("PART\n");
		int a,b;
 
		getChannelNick(a,b,to,nick);
		if ( a != -1) { 
			if ( b != -1 ) {
				if (strcasecmp(nick,botnick)==0){
                    //blah
                    printf("BOT left %s",to);
					fprintf(channels[a]->logfile,"BOT left %s",to);
                    int c;
                    for ( c = 0; c < channels[a]->users.size(); c++){
                        delete channels[a]->users[c]->user;
                        delete channels[a]->users[c]->host;
                        delete channels[a]->users[c]->server;
                        delete channels[a]->users[c]->nick;
                        delete channels[a]->users[c]->mode;
                        delete channels[a]->users[c]->realname;
                        if (channels[a]->users[c]->lastsaid)
                            delete channels[a]->users[c]->lastsaid;
                        if (channels[a]->users[c]->oldnick)
                            delete channels[a]->users[c]->oldnick;
						
                    }
					fclose(channels[a]->logfile);
                    channels.erase(channels.begin()+a);
                    
                } 
                else{ 
                    channels[a]->users[b]->lasttime=time(NULL);
				    channels[a]->users[b]->lasttype='P';
				    delete channels[a]->users[b]->lastsaid;
				    channels[a]->users[b]->lastsaid = new char[1+strlen(data)];
				    strcpy(channels[a]->users[b]->lastsaid,data);
                }
            }
		}
	}
//------------------------------------------------------------------------------
	if (type==KICK){
		printf("KICK\n");
		int a,b;
 
		getChannelNick(a,b,to,nick);
		if ( a != -1) { 
			if ( b != -1 ) {
				if (strcasecmp(nick,botnick)==0){
                    //blah
                    printf("BOT got kicked %s",to);
					fprintf(channels[a]->logfile,"BOT got kicked %s",to);
                    int c;
                    for ( c = 0; c < channels[a]->users.size(); c++){
                        delete channels[a]->users[c]->user;
                        delete channels[a]->users[c]->host;
                        delete channels[a]->users[c]->server;
                        delete channels[a]->users[c]->nick;
                        delete channels[a]->users[c]->mode;
                        delete channels[a]->users[c]->realname;
                        if (channels[a]->users[c]->lastsaid)
                            delete channels[a]->users[c]->lastsaid;
                        if (channels[a]->users[c]->oldnick)
                            delete channels[a]->users[c]->oldnick;
						
                    }
					fclose(channels[a]->logfile);
                    channels.erase(channels.begin()+a);
                } 
                else{ 
                    channels[a]->users[b]->lasttime=time(NULL);
				    channels[a]->users[b]->lasttype='K';
				    delete channels[a]->users[b]->lastsaid;
				    channels[a]->users[b]->lastsaid = new char[1+strlen(data)];
				    strcpy(channels[a]->users[b]->lastsaid,data);
                }
            }
		}
	}
//------------------------------------------------------------------------------
	if (type==QUIT){
		if (strcasecmp(nick,botnick)==0){
            printf("BOT quit %s",to);
			// verify code with codeguard.
            int c,d;
		   	for ( c = 0 ; c < channels.size();c++){
            	for ( d = 0; d < channels[c]->users.size(); d++){
                	fprintf(channels[c]->logfile,"BOT quit %s",to);
					delete channels[c]->users[d]->user;
                    delete channels[c]->users[d]->host;
                    delete channels[c]->users[d]->server;
                    delete channels[c]->users[c]->nick;
                    delete channels[c]->users[d]->mode;
                    delete channels[c]->users[d]->realname;
                    if (channels[c]->users[d]->lastsaid)
                        delete channels[c]->users[d]->lastsaid;
                    if (channels[c]->users[d]->oldnick)
                        delete channels[c]->users[d]->oldnick;
					
                }
				fclose(channels[c]->logfile);
                channels.erase(channels.begin()+c);
			}
		}else
		printf("%s quit\n",nick);
		int a=0,b;
		
		while ( a < channels.size() ){
			for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcasecmp (nick ,channels[a]->users[b]->nick) != 0); b++);
			if ( b < channels[a]->users.size() ) {// user bestaat in channel;
// --				
				channels[a]->users[b]->lasttype='Q';
				delete channels[a]->users[b]->lastsaid;
				channels[a]->users[b]->lastsaid = new char[1+strlen(data)];
				strcpy(channels[a]->users[b]->lastsaid,data);
				
			}
			a++;
		}
	}



//------------------------------------------------------------------------------
	if (type==KILL){
		if (strcasecmp(to,botnick)==0){
            printf("BOT got killed %s",to);
			// verify code with codeguard.
            int c,d;
		   	for ( c = 0 ; c < channels.size();c++){
            	for ( d = 0; d < channels[c]->users.size(); d++){
					fprintf(channels[c]->logfile,"BOT got killed %s",to);
                	delete channels[c]->users[d]->user;
                    delete channels[c]->users[d]->host;
                    delete channels[c]->users[d]->server;
                    delete channels[c]->users[c]->nick;
                    delete channels[c]->users[d]->mode;
                    delete channels[c]->users[d]->realname;
                    if (channels[c]->users[d]->lastsaid)
                        delete channels[c]->users[d]->lastsaid;
                    if (channels[c]->users[d]->oldnick)
                        delete channels[c]->users[d]->oldnick;
					
                }
				fclose(channels[c]->logfile);
                channels.erase(channels.begin()+c);
			}
		}else
		printf("%s got killed\n",nick);
		int a=0,b;
		
		while ( a < channels.size() ){
			for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcasecmp (to ,channels[a]->users[b]->nick) != 0); b++);
			if ( b < channels[a]->users.size() ) {// user bestaat in channel;
// --				
				channels[a]->users[b]->lasttype='k';
				delete channels[a]->users[b]->lastsaid;
				channels[a]->users[b]->lastsaid = new char[1+strlen(data)];
				strcpy(channels[a]->users[b]->lastsaid,data);
				
			}
			a++;
		}
	}
}


//------------------------------------------------------------------------------


void IRCclient::joinchannel(char *channel){
	char temp[500];
	sprintf(temp,"JOIN %s\xd\xa",channel);
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

void IRCclient::partchannel(char *channel){
	char temp[500];
	sprintf(temp,"PART %s\xd\xa",channel);
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

void IRCclient::getChannelNick (int &a, int &b, char *channel, char *nick){

	//--------------------------------------------------------------------------
	for ( a = 0; (a < channels.size()) &&
		                      (strcasecmp (channel,channels[a]->channel) != 0);a++);
	if ( a < channels.size() ) { // channel bestaat 
	//--------------------------------------------------------------------------
		for ( b = 0; ( b < channels[a]->users.size() ) &&
			              (strcasecmp(nick,channels[a]->users[b]->nick) != 0); b++); 
		if ( b < channels[a]->users.size() ) { // user bestaat 	
	//--------------------------------------------------------------------------
		} else  b=-1; 
	} else  a=-1; 
}
//------------------------------------------------------------------------------
void IRCclient::splitnickuser ( char *Ptr, char *&nick, char *&mask ){
	Ptr++;
	nick=Ptr;
	bool done=false;
	while(*Ptr != 0 && !done) {
		if(*Ptr == '!'){
			*Ptr = 0;
    		mask = Ptr+1;
    		done = true;
		}
		Ptr++;
	}
}

//------------------------------------------------------------------------------
void IRCclient::splituserhost ( char *Ptr, char *&user, char *&host ){
	Ptr++;
	user=Ptr;
	bool done=false;
	while(*Ptr != 0 && !done) {
		if(*Ptr == '@'){
			*Ptr = 0;
    		host = Ptr+1;
    		done = true;
		}
		Ptr++;
	}
}
//------------------------------------------------------------------------------


void IRCclient::sendNICK(char *nick){
     char temp[128];
     sprintf(temp,"NICK %s\xd\xa",nick);
     send (sServer,temp,strlen(temp),0);     
}
//------------------------------------------------------------------------------
void IRCclient::sendAWAY(char *reason){
     char temp[666];
     sprintf(temp,"AWAY :%s\xd\xa",reason);
     send (sServer,temp,strlen(temp),0);     
}
//------------------------------------------------------------------------------
void IRCclient::sendBACK(){
     send (sServer,"AWAY\xd\xa",6,0);     
}
//------------------------------------------------------------------------------
void IRCclient::sendQUIT(char *reason){
	//implement quit reson message
     send (sServer,"QUIT \xd\xa",6,0);     
}
//------------------------------------------------------------------------------
void IRCclient::sendPRIVMSG(char *target, char *message){
	char temp[500];
	sprintf(temp,"PRIVMSG %s %s\xD\xA",target,message);	
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

void IRCclient::sendNOTICE(char *target, char *message){
	char temp[500];
	sprintf(temp,"NOTICE %s %s\xD\xA",target,message);	
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

void IRCclient::sendACTION(char *target, char *message){
	char temp[500];
	sprintf(temp,"PRIVMSG %s \x01\x41\x43TION %s\x1\xD\xA",target,message);	
	send (sServer,temp,strlen(temp),0);
}

//------------------------------------------------------------------------------
char IRCclient::userlevel (char *mode, char *channel, char *nick, char *host){
// naar bot ?
	
		
	if (strcasecmp(host,"Indre-7A2D8200.xs4all.nl")==0)
		return 100; //thuis indreanet
	if (strcasecmp(host,"Chat4all-51B52190.xs4all.nl")==0)
		return 100; //thuis chat4all
	
	if (strcasecmp(host,"52E386A0.CD152A2C.3B842763.IP")==0)
		return 100; // school 0160 indeanet
	if (strcasecmp(host,"E2A638CC.801811FA.C98607E8.IP")==0)
		return 100; // school 0160 chat4all
	
	if (strcmp(channel,"")!=0) //pm
    if (isop(channel,nick)) return 50; //->test
	
	if (iscsregged(mode)) return 25; //debug??
	
	return 0;
}
//------------------------------------------------------------------------------

void IRCclient::userlist (char *channel, char *user, char *host, char *server,
	char *nick,    char *mode, char *realname)
{
    int a,b;
	getChannelNick(a,b,channel,nick);
	if ( a != -1) { 
		if ( b != -1 ) {
				if (strcasecmp(mode,channels[a]->users[b]->mode )!= 0 ){
					delete channels[a]->users[b]->mode;
					channels[a]->users[b]->mode = new char[1+strlen(mode)];
					strcpy (channels[a]->users[b]->mode,mode);
				}
				
				channels[a]->users[b]->userlevel=userlevel(mode,channel,nick,host);
				
				if (channels[a]->users[b]->lasttype == 'P' ||	
					channels[a]->users[b]->lasttype == 'Q' ){
						channels[a]->users[b]->lasttype = 'J';
						channels[a]->users[b]->lasttime=time(NULL);
						channels[a]->users[b]->lines=0;
				}
			} 
			else{ // user bestaat niet in channel;
	//	             printf("mode %s\n",mode);
				ircuser *newuser;
				newuser = new ircuser;

				newuser->user = new char[1+strlen(user)];
				strcpy (newuser->user,user);
				newuser->host = new char[1+strlen(host)];
				strcpy (newuser->host,host);
				newuser->server = new char[1+strlen(server)];
				strcpy (newuser->server,server);
				newuser->nick = new char[1+strlen(nick)];
				strcpy (newuser->nick,nick);
				newuser->mode = new char[1+strlen(mode)];
				strcpy (newuser->mode,mode);
				newuser->realname = new char[1+strlen(realname)];
				strcpy (newuser->realname,realname);
				newuser->lasttime = time(NULL);
				newuser->lastsaid = NULL;
				newuser->lasttype = 'J';
				newuser->oldnick=NULL;
				newuser->lines = 0;
				//newuser->userlevel=userlevel(mode,channel,nick,host);
				// wth???? // channels[channels.size()-1]->users.push_back(newuser);
				channels[a]->users.push_back(newuser);	
				fprintf(channels[a]->logfile,"%s joined %s\n",nick,channel);
             //lelijke code!!!
				channels[a]->users[channels[a]->users.size()-1]->userlevel=userlevel(mode,channel,nick,host);	
	
				
			}
		}else{//channel bestaat niet
//		        printf("mode %s\n",mode);
		ircuser *newuser;
		newuser = new ircuser;
			
		newuser->user = new char[1+strlen(user)];
		strcpy (newuser->user,user);
		newuser->host = new char[1+strlen(host)];
		strcpy (newuser->host,host);
		newuser->server = new char[1+strlen(server)];
		strcpy (newuser->server,server);
		newuser->nick = new char[1+strlen(nick)];
		strcpy (newuser->nick,nick);
		newuser->mode = new char[1+strlen(mode)];
		strcpy (newuser->mode,mode);
		newuser->realname = new char[1+strlen(realname)];
		strcpy (newuser->realname,realname);
		newuser->lasttime = time(NULL);
		newuser->lastsaid = NULL;
		newuser->lines = 0;		
		newuser->oldnick=NULL;
		newuser->lasttype = 'J';
		//newuser->userlevel=userlevel(mode,channel,nick,host);
			
		ircchannel *newchannel;
		newchannel = new ircchannel;
		newchannel->logfile = fopen (channel,"a");
    	newchannel->channel = new char[1+strlen(channel)];
		strcpy (newchannel->channel,channel);
		channels.push_back(newchannel);
		channels[channels.size()-1]->users.push_back(newuser); 
		fprintf(newchannel->logfile,"%s joined %s\n",nick,channel);
		// lelijke code
		channels[channels.size()-1]->users[channels[channels.size()-1]->users.size()-1]->userlevel=userlevel(mode,channel,nick,host);	
	}
}	
//------------------------------------------------------------------------------

void IRCclient::setmode(char *channel, char *nick, char *mode){
	if (botisop(channel)) {
		char temp[128];
		sprintf(temp,"MODE %s %s %s\xd\xa",channel,mode,nick);
		send (sServer,temp,strlen(temp),0);
	}
}
//------------------------------------------------------------------------------
void IRCclient::irc_received(char *data){
	char *rawdata = new char[1+strlen(data)];
	strcpy(rawdata,data);
    int NrParam;
    char *Param[25];
	spltstr(data,NrParam,Param,24);

	#ifdef loud
  //debug code//
    for ( int a = 0 ; a <= NrParam ; printf("Param %d of %d : (%d) %s\n",a,NrParam,strlen(Param[a]),Param[a]),a++);
    printf("\n");
  //debug code//
   #endif

  	if (strlen(Param[0])==4) { 
		if (strncmp (Param[0],"PING",4) == 0){
			Param[0][1]='O';
      		Param[0][4]=' ';
	  		send (sServer,Param[0], strlen(Param[0]), 0);
			send (sServer,"\xD\xA",2,0);
		}
	}
	
	if (strlen(Param[0])==5) { 
		if (strncmp (Param[0],"ERROR",4) == 0){
            printf("ERROR\n");
			// doe iets
		}
	}
  
    if (strlen(Param[1])==4){
		if (strncmp (Param[1],"JOIN",4) == 0 ||
			strncmp (Param[1],"PART",4) == 0 ||
			strncmp (Param[1],"QUIT",4) == 0 ||
		    strncmp (Param[1],"NICK",4) == 0 ||
		    strncmp (Param[1],"KICK",4) == 0 ||
		    strncmp (Param[1],"KILL",4) == 0 ||
		    strncmp (Param[1],"MODE",4) == 0 ){              
			//if (NrParam == 2 ) {  

				{
				char* nick;
          		char* mask=NULL;

				splitnickuser(Param[0],nick,mask);	
					
        		// --- kies hier ---- 
				if (strncmp (Param[1],"JOIN",4) == 0) 
					irc_message(JOIN,nick,mask,Param[2]+1,NULL);
				if (strncmp (Param[1],"PART",4) == 0) 
					irc_message(PART,nick,mask,Param[2],Param[3]);
				if (strncmp (Param[1],"KICK",4) == 0) 
					irc_message(PART,nick,mask,Param[2],Param[3]);				
				if (strncmp (Param[1],"NICK",4) == 0) 
					irc_message(NICK,nick,mask,NULL,Param[2]+1);
				if (strncmp (Param[1],"QUIT",4) == 0){
					char *P[3]; int NrP;
					spltstr(rawdata,NrP,P,2);
					irc_message(QUIT,nick,mask,NULL,P[2]+1);
				}
				if (strncmp (Param[1],"MODE",4) == 0){
                	char *P[4]; int NrP;
                    spltstr(rawdata,NrP,P,3);
				   	irc_message(MODE,nick,mask,P[2],P[3]);//--
				}
				if (strncmp (Param[1],"KILL",4) == 0){
                	char *P[4]; int NrP;
                    spltstr(rawdata,NrP,P,3);
				   	irc_message(KILL,nick,mask,P[2],P[3]);//--
				}
			}
		}
	}
//------------------------------------------------------------------------------
	if (strlen(Param[1])==6){
    	if (strncmp (Param[1],"NOTICE",6) == 0){
			if (NrParam == 3 ) { 
          		char* nick;
          		char* mask=NULL;
          		splitnickuser(Param[0],nick,mask);	
				if (strcasecmp(Param[2],botnick)== 0)
					irc_message(NOTP,nick,mask,Param[2],Param[3]);	
				else
					irc_message(NOTC,nick,mask,Param[2],Param[3]);
									
			}	
		}
	}
//------------------------------------------------------------------------------
    if (strlen(Param[1])==7){
    	if (strncmp (Param[1],"PRIVMSG",7) == 0){
			if (NrParam == 3 ) { 
          		char* nick;
          		char* mask=NULL;
          		splitnickuser(Param[0],nick,mask);	
				
                if (Param[3][0] == 1){ // CTCP or ACTION
            		if (strlen(Param[3])>7){
						if (strncmp (Param[3],"\x01\x41\x43TION",7) == 0){
							// "\x01ACTION" werkt niet, pakt die AC als hex
							// en zegt dan out of range. gekke compiler.
						    Param[3][strlen(Param[3])-1]=0x00;
							//printf("%s%s\n",nick, Param[3]+7);
							if (strcasecmp(Param[2],botnick)== 0)
								irc_message(PAMS,nick,mask,Param[2],Param[3]+8);
							else
								irc_message(AMES,nick,mask,Param[2],Param[3]+8);
							
						}
					}
					
					if (strlen(Param[3])==9){
              			if (strncmp (Param[3],"\x01VERSION\x01",9) == 0){
                			printf("CTCP VERSION reveived from %s\n",nick);
                            sendNOTICE(nick,"\x1VERSION bscp-bot pre-alpha testing\x1");
						}
					}
				}
		  		else {
					if (strcasecmp(Param[2],botnick)== 0)
					irc_message(PMES,nick,mask,Param[2],Param[3]);	
					else
					irc_message(CMES,nick,mask,Param[2],Param[3]);
				}
			}
		}
    }
    if (strlen(Param[1])==3) {
		if (strncmp (Param[1],"001",3) == 0){
			printf("Login Successfull\n");
  		}
		if (strncmp (Param[1],"352",3) == 0){
			if (NrParam == 9){
				userlist(Param[3],Param[4],Param[5],Param[6],
				         Param[7],Param[8],Param[9]);
			}
		}
		if (strncmp (Param[1],"376",3) == 0){
       		printf("Ready to join\n");
       		#ifdef indreanet    
			sendPRIVMSG("nickserv","identify bscp2007");
			joinchannel("#bscp-testing");
			joinchannel("#test");
			#else
			sendPRIVMSG("nickserv","identify bscp2007");
			joinchannel("#blaatschaap");
			joinchannel("#country-roads");
			#endif
		}
		if (strncmp (Param[1],"433",3) == 0){
	  		if (NrParam == 4 ) {
		  		printf("Nick error: %s\n", Param[4]);
		  		char *nick = Param[3];
      	  		nick[strlen(nick)+1]=0x00;
		  		nick[strlen(nick)]='_';	  
		  		printf("Retrying with %s ...\n",nick);	      
                sendNICK(nick);
		  		delete botnick;

	  		}
	
  		}
		
    }

}


void IRCclient::login ()
{
	botnick = new char[1+sizeof("BlaatBot2007")];
    sprintf(botnick,"BlaatBot2007");

	printf("Connected... logging in as %s ...\n",botnick);
    char temp[128];
    sendNICK(botnick);
    sprintf(temp,"USER bscp_cbot bscp_host bscp_server :BlaatSchaap Coding Projects 2007 IRC BOT\xd\xa");
    send (sServer,temp, strlen(temp), 0);
	receivedata();
}

void IRCclient::receivedata(){
    char temp[666];
	int received_size=0;
	while(1){
		//test dit disconnected detection.
        if (!(recv(sServer, temp+received_size, 1, 0))) return; 
        received_size++;
        if (temp[received_size-1] == 0x0A  ) { 
		    temp[received_size] = 0x00; //detect end of line.
		    irc_received(temp); 
      	    received_size=0;
        }
    }
}

int IRCclient::connect_irc(char *ip, int port){
#ifdef __WIN32__
    SOCKADDR_IN saServer;             // WINSOCK
    WSADATA wsda;                     
    WSAStartup(MAKEWORD(1,1), &wsda); 
#else     
    struct sockaddr_in saServer;      // BSD SOCKETS
#endif   

printf("Connecting to %s:%d...\n",ip,port);	

	
    
    sServer=socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(sServer==SOCKET_ERROR)
        return -1;

    saServer.sin_addr.s_addr=inet_addr(ip);
    saServer.sin_family=AF_INET;
    saServer.sin_port=htons(port);
    if(connect(sServer, (struct sockaddr *)&saServer, sizeof(saServer))\
		==SOCKET_ERROR)
        return -1;
//------------------------------------------------------------------------------
  return 0;
}
