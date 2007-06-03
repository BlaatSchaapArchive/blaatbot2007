#define loud
#define indreanet

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

vector <ircchannel*> channels;

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


//------------------------------------------------------------------------------

//char *botnick;

//------------------------------------------------------------------------------

void joinchannel(char *channel){
	char temp[500];
	sprintf(temp,"JOIN %s\xd\xa",channel);
	send (sServer,temp,strlen(temp),0);
}
//------------------------------------------------------------------------------

void getChannelNick (int &a, int &b, char *channel, char *nick){
	printf("getChannelNick\n");
	//--------------------------------------------------------------------------
	for ( a = 0; (a < channels.size()) &&
		                      (strcasecmp (channel,channels[a]->channel) != 0);a++);
	if ( a < channels.size() ) { // channel bestaat 
	//--------------------------------------------------------------------------
		for ( b = 0; ( b < channels[a]->users.size() ) &&
			              (strcasecmp(nick,channels[a]->users[b]->nick) != 0); b++); 
		if ( b < channels[a]->users.size() ) { // user bestaat 	
	//--------------------------------------------------------------------------
		} else { b=-1; printf("Unknown user\n");}
	} else { a=-1; printf("Unknown channel\n");}
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
	
		
	if (strcasecmp(host,"Indre-7A2D8200.xs4all.nl")==0)
		return 100; //thuis indreanet
	if (strcasecmp(host,"Chat4all-51B52190.xs4all.nl")==0)
		return 100; //thuis chat4all
	
	if (strcasecmp(host,"52E386A0.CD152A2C.3B842763.IP")==0)
		return 100; // school indeanet
	if (strcasecmp(host,"E2A638CC.801811FA.C98607E8.IP")==0)
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
				if (strcasecmp(mode,channels[a]->users[b]->mode )!= 0 ){
					delete channels[a]->users[b]->mode;
					channels[a]->users[b]->mode = new char[1+strlen(mode)];
					strcpy (channels[a]->users[b]->mode,mode);
				}
				
				if (channels[a]->users[b]->lasttype == 'P' ||	
					channels[a]->users[b]->lasttype == 'Q' ){
						channels[a]->users[b]->lasttype = 'J';
						channels[a]->users[b]->lasttime=time(NULL);
						channels[a]->users[b]->lines=0;
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
void verwerk (char type, char *nick, char *host, char *to, char *data)
{
	//		if (strcasecmp(to,botnick)==0){ // verander dit, botnick variable
	int a,b;
	if (to) 
		getChannelNick(a,b,to,nick);
	//if ( a == -1 ) return;
    //if ( b == -1 ) return;
	
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
				channels[a]->users[b]->lasttime=time(NULL);
				channels[a]->users[b]->lasttype='P';
// --
				delete channels[a]->users[b]->lastsaid;
				channels[a]->users[b]->lastsaid = new char[1+strlen(data)];
				strcpy(channels[a]->users[b]->lastsaid,data);
// --
				
			}
		}
	}

	if (type==QUIT){
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
}
//------------------------------------------------------------------------------
void irc_received(char *data){
	//quit begin testng
	char *rawdata = new char[1+strlen(data)];
	strcpy(rawdata,data);

	/*
	int NrPq;
    char *Pq[3];
	spltstr(rawdata,NrPq,Pq,2); /// uhuh
    // quit end ... testing 
    */
	
	
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
				if (strncmp (Param[1],"QUIT",4) == 0){

					
					char *Pq[3]; int NrPq;
					// -- dit heeft invloed op nick ????
					// -- rawdata is een **kopie** van data
					// -- waarom heeft deze spltstr invloed op nick
					// -- 
					spltstr(rawdata,NrPq,Pq,2);
					
					verwerk(QUIT,nick,mask,NULL,Pq[2]+1);
				}
				
				
				

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
					
				//	if (strcasecmp(Param[2],botnick)== 0)
				//	verwerk(PMES,nick,mask,Param[2],Param[3]);	
			//		else
					verwerk(CMES,nick,mask,Param[2],Param[3]);
											
					//verify this
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

int connect_irc(char *ip, int port){
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
