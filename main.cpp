#define indreanet
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
#ifdef __WIN32__              // compile for WIN32
#include <winsock.h>          // using winsock     
#else                         // compile for other operating systems 
#include <sys/socket.h>       // using BSD sockets
#include <sys/types.h>      
#include <netinet/in.h>     
#include <arpa/inet.h>      
#define SOCKET int 
#define SOCKET_ERROR -1
#endif
//------------------------------------------------------------------------------
#include <cstdlib>
#include <iostream>
#include <time.h> 
#include <stdio.h>
#include <vector>
#include "osinfo.h"
using namespace std;
//------------------------------------------------------------------------------
#define CMES 0
#define AMES 1
#define PMES 2
#define PAMS 3
#define NOTC 4
#define NOTP 5
#define JOIN 10
#define PART 11
#define NICK 12
#define QUIT 13
//------------------------------------------------------------------------------
struct ircuser{ 
    char *user; char *host; char *server;
    char *nick;     char *mode; char *realname; time_t lasttime; char lasttype;
	char *lastsaid; int  lines; char *oldnick; char userlevel;};
	

struct ircchannel{char *channel; vector <ircuser *> users; FILE *logfile;};

//------------------------------------------------------------------------------
vector <ircchannel*> channels;
char *botnick;
SOCKET sServer;
char *ostype,*osrelease,*osname,*machine;
//bool joined;
//------------------------------------------------------------------------------

void joinchannel(char *channel){
	char temp[500];
	sprintf(temp,"JOIN %s\xd\xa",channel);
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

void getChannelNick (int &a, int &b, char *channel, char *nick)
{
	//--------------------------------------------------------------------------
	for ( a = 0; (a < channels.size()) &&
		                      (strcmp (channel,channels[a]->channel) != 0);a++);
	if ( a < channels.size() ) { // channel bestaat 
	//--------------------------------------------------------------------------
		for ( b = 0; ( b < channels[a]->users.size() ) &&
			              (strcmp(nick,channels[a]->users[b]->nick) != 0); b++); 
		if ( b < channels[a]->users.size() ) { // user bestaat 	
	//--------------------------------------------------------------------------
		} else b=-1; 
	} else a=-1;
}
//------------------------------------------------------------------------------

bool isop(char *channel, char *nick){	
	int a,b,c;
	getChannelNick(a,b,channel,nick);
	if ( a != -1) { 
		if ( b != -1 ) {
			for ( c = 0; c < strlen(channels[a]->users[b]->mode);){
				// ~(q)&(a)@(o)%(h)
				if (channels[a]->users[b]->mode[c]== '~') return true;
				if (channels[a]->users[b]->mode[c]== '&') return true;
				if (channels[a]->users[b]->mode[c]== '@') return true;
				if (channels[a]->users[b]->mode[c]== '%') return true;
				c++; // staat hier vanwege mode[c]
			}
		}
	}
    return false;
}
//------------------------------------------------------------------------------

bool botisop(char *channel){
	return isop(channel, botnick);
}
//------------------------------------------------------------------------------

bool iscsregged(char *mode){
	int c;
	bool result=false;
	for ( c = 0; c < strlen(mode);){
		result = result || (mode[c]== 'r');
		c++; // staat hier vanwege mode[c]
	}
return result;
}

bool iscsregged(char *channel,char *nick){	
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

void spltstr(char *data, int &NrParam, char **Param,char nrspace){
    
    char teller=0;
    char *Ptr = data; 
    bool done=false;
    NrParam = 0;
    Param[NrParam] = data;
    while(*Ptr != 0){
	if(*Ptr == 0x0A) *Ptr = 0x00;
        if(*Ptr == 0x0D) *Ptr = 0x00;
        if(*Ptr == 0x20 && !done){
	    *Ptr = 0x00;
            NrParam++;Param[NrParam] = Ptr + 1;
	}
        if(*Ptr == ':' && NrParam > 2 && !done) { 
            done=true;
     	Param[NrParam] = Ptr + 1; 
	}
        Ptr++;
    }
    done = false;

}
//------------------------------------------------------------------------------

void splitnickuser ( char *Ptr, char *&nick, char *&mask ){
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

void sendNICK(char *nick){
     char temp[128];
     sprintf(temp,"NICK %s\xd\xa",nick);
     send (sServer,temp,strlen(temp),0);     
}

//------------------------------------------------------------------------------

void sendPRIVMSG(char *target, char *message){
	char temp[500];
	sprintf(temp,"PRIVMSG %s %s\xD\xA",target,message);	
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

void sendNOTICE(char *target, char *message){
	char temp[500];
	sprintf(temp,"NOTICE %s %s\xD\xA",target,message);	
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

void sendACTION(char *target, char *message){
	char temp[500];
	sprintf(temp,"PRIVMSG %s \x01\x41\x43TION %s\x1\xD\xA",target,message);	
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

char userlevel (char *mode, char *channel, char *nick, char *host){
//test
	
		
	if (strcmp(host,"Indre-7A2D8200.xs4all.nl")==0)
		return 100; //thuis indreanet
	if (strcmp(host,"Chat4all-51B52190.xs4all.nl")==0)
		return 100; //thuis chat4all
	
	if (strcmp(host,"52E386A0.CD152A2C.3B842763.IP")==0)
		return 100; // school indeanet
	if (strcmp(host,"E2A638CC.801811FA.C98607E8.IP")==0)
		return 100; // school chat4all
	
	if (iscsregged(mode)) return 25;
	

	//
	
	// --> hard coded for now .... will be in file later
	return 0;
}
//------------------------------------------------------------------------------

void userlist (char *channel, char *user, char *host, char *server,
	char *nick,    char *mode, char *realname)
{
    int a,b;
	getChannelNick(a,b,channel,nick);
	if ( a != -1) { 
		if ( b != -1 ) {
				if (strcmp(mode,channels[a]->users[b]->mode )!= 0 ){
					delete channels[a]->users[b]->mode;
					channels[a]->users[b]->mode = new char[1+strlen(mode)];
					strcpy (channels[a]->users[b]->mode,mode);
				}
				
				if (channels[a]->users[b]->lasttype == 'P' ||	
					channels[a]->users[b]->lasttype == 'Q' ){
						channels[a]->users[b]->lasttype = 'J';
						channels[a]->users[b]->lasttime=time(NULL);
				}
			} 
			else{ // user bestaat niet in channel;
		
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
				newuser->userlevel=userlevel(mode,channel,nick,host);
				// wth???? // channels[channels.size()-1]->users.push_back(newuser);
				channels[a]->users.push_back(newuser);	
				fprintf(channels[a]->logfile,"%s joined %s\n",nick,channel);
					
	
				
			}
		}else{//channel bestaat niet
		
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
		newuser->userlevel=userlevel(mode,channel,nick,host);
			
		ircchannel *newchannel;
		newchannel = new ircchannel;
		newchannel->logfile = fopen (channel,"a");
    	newchannel->channel = new char[1+strlen(channel)];
		strcpy (newchannel->channel,channel);
		channels.push_back(newchannel);
		channels[channels.size()-1]->users.push_back(newuser); 
		fprintf(newchannel->logfile,"%s joined %s\n",nick,channel);
	}
}	
//------------------------------------------------------------------------------

void setmode(char *channel, char *nick, char *mode){
	if (botisop(channel)) {
		char temp[128];
		sprintf(temp,"MODE %s %s %s\xd\xa",channel,mode,nick);
		send (sServer,temp,strlen(temp),0);
	}
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
				       P[1],channel,channels[a]->users[b]->lastsaid) ;
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
		   		
		


//------------------------------------------------------------------------------
void verwerk (char type, char *nick, char *host, char *to, char *data)
{
	//		if (strcmp(to,botnick)==0){ // verander dit, botnick variable
	int a,b;
	if (to) 
		getChannelNick(a,b,to,nick);
	
	if (type==PMES || type==PAMS || type==NOTP ){
		if (type==PMES) { //MESSAGE PRIVÉ
			fprintf(channels[a]->logfile,">%s< %s \n",nick,data);			
			fflush(channels[a]->logfile);
		}			
		if (type==PAMS) { //ACTION PRIVÉ
			fprintf(channels[a]->logfile,">%s %s<\n",nick,data);			
			fflush(channels[a]->logfile);
		}				
		if (type==NOTP) { //NOTICE PRIVÉ
			fprintf(channels[a]->logfile,"#%s# %s \n",nick,data);			
			fflush(channels[a]->logfile);
		}
	}
	if (type==CMES || type==AMES|| type==NOTC ){
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
		
	
	if (type==JOIN){
		printf("%s joined %s\n",nick, to);
		char temp[128]="WHO ";
		strcpy(temp+4,to);
		send(sServer,temp,strlen(temp),0);
		send (sServer,"\xD\xA",2,0);
	}
	
	if (type==NICK){
		if (strcmp(botnick,nick)==0){
			delete botnick;
			botnick = new char[1+strlen(data)];
			strcpy(botnick,data);
		}
		int a=0,b;
		
		while ( a < channels.size() ){
			for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcmp (nick ,channels[a]->users[b]->nick) != 0); b++);
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
				channels[a]->users[b]->lasttime=time(NULL);
				channels[a]->users[b]->lines=0;	
				channels[a]->users[b]->lasttype='P';
			}
		}
	}

	if (type==QUIT){
		printf("%s quit\n",nick);
		int a=0,b;
		
		while ( a < channels.size() ){
			for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcmp (nick ,channels[a]->users[b]->nick) != 0); b++);
			if ( b < channels[a]->users.size() ) {// user bestaat in channel;
				
				channels[a]->users[b]->lasttype='Q';
				delete channels[a]->users[b]->lastsaid;
				channels[a]->users[b]->lastsaid = new char[1+strlen(data)];
				strcpy(channels[a]->users[b]->lastsaid,data);
			}
			a++;
		}
	}
}
//------------------------------------------------------------------------------
void irc_received(char *data){
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
  
    if (strlen(Param[1])==4){
		if (strncmp (Param[1],"JOIN",4) == 0 ||
			strncmp (Param[1],"PART",4) == 0 ||
			strncmp (Param[1],"QUIT",4) == 0 ||
		    strncmp (Param[1],"NICK",4) == 0 ){ 
			//if (NrParam == 2 ) {  
				bool done = false; 
				{
				char* nick;
          		char* mask=NULL;

				splitnickuser(Param[0],nick,mask);	
					
        		// --- kies hier ---- 
				if (strncmp (Param[1],"JOIN",4) == 0) 
					verwerk(JOIN,nick,mask,Param[2]+1,NULL);
				if (strncmp (Param[1],"PART",4) == 0) 
					verwerk(PART,nick,mask,Param[2],Param[3]);
				if (strncmp (Param[1],"NICK",4) == 0) 
					verwerk(NICK,nick,mask,NULL,Param[2]+1);
				if (strncmp (Param[1],"QUIT",4) == 0) 
					verwerk(QUIT,nick,mask,NULL,Param[2]);
				

			}
		}
	}
	
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
							verwerk(AMES,nick,mask,Param[2],Param[3]+8);
						}
					}
					
					if (strlen(Param[3])==9){
              			if (strncmp (Param[3],"\x01VERSION\x01",9) == 0){
                			printf("CTCP VERSION reveived from %s\n",nick);
                            sendNOTICE(nick,"\x1VERSION bscp-bot pre-alpha testing\x1");
						}
					}
				}
		  		else  
		  		{
					//printf("<%s> %s\n",nick,Param[3]);			
					verwerk(CMES,nick,mask,Param[2],Param[3]);
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
			joinchannel("#bscp-testing");
			joinchannel("#test");
			#else
			joinchannel("#blaatschaap");
			joinchannel("#test");
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
				//botnick = new char[1+strlen(nick)];
                //strcpy(botnick,nick);
				//char temp[25]="NICK ";      
		  		//strcpy (temp+5,nick);
          		//strcpy (temp+(strlen(temp)),"\xD\xA");		  
	      		//send (sServer,temp,strlen(temp),0);  
	  		}
	
  		}
		
    }

}

//------------------------------------------------------------------------------
int connect(char *ip, int port)
{
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
//------------------------------------------------------------------------------
void login ()
{
	printf("Connected... logging in as %s ...\n",botnick);
    char temp[128];
    sendNICK(botnick);
    sprintf(temp,"USER bscp_cbot bscp_host bscp_server :BlaatSchaap Coding Projects 2007 IRC BOT\xd\xa");
    send (sServer,temp, strlen(temp), 0);
}

void receivedata(){
    char temp[666];
	int received_size=0;
	while(1){
        if (!(recv(sServer, temp+received_size, 1, 0))) return; //test dit disconnected detection.
        received_size++;
        if (temp[received_size-1] == 0x0A  ) { 
		    temp[received_size] = 0x00; //unless
		    irc_received(temp);
      	    received_size=0;
        }
    }
}
//------------------------------------------------------------------------------
int main(int argc, char *argv[]){
    getOSinfo(ostype,osrelease,osname,machine);
    int port = 6667;
#ifdef indreanet	
    char *ip="195.28.165.175";
#else		
    char *ip="62.75.201.175";
#endif	
	if (!(connect(ip,port)))
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
