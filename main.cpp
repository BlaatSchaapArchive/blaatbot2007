//#define indreanet

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
//#include <string>
#include <vector>
using namespace std;
//------------------------------------------------------------------------------
SOCKET sServer;
bool joined;

//------------------------------------------------------------------------------
//

#define TextMessage 0
#define ActionMessage 1
#define NoticeMessage 2
#define JOIN 10
#define PART 11
#define NICK 12
#define QUIT 13

struct ircuser{ 
    char *user; char *host; char *server;
    char *nick;     char *mode; char *realname; time_t lasttime;
	char *lastsaid; int  lines; char *oldnick; int userlevel;};
	
struct ircchannel{char *channel; vector <ircuser*> users;};

vector <ircchannel*> channels;

void userlist (char *channel, char *user, char *host, char *server,
               char *nick,    char *mode, char *realname)
{
	// 352 messages
	// dit is conecpt, moet helemaal anders.
	// per channel ircchannel met daarin een vector met ircusers
	// 
    int a,b;
	for ( a = 0; (a < channels.size()) && 
			                  (strcmp (channel,channels[a]->channel) != 0);a++);
	if ( a < channels.size() ) { // channel bestaat
			for ( b = 0; ( b < channels[a]->users.size() ) && 
					      (strcmp(nick,channels[a]->users[b]->nick) != 0); b++);
			if ( b < channels[a]->users.size() ) // user bestaat in channel;
			{
				// user bestaat, doe niets... // --> mode aanpassen misschien?
				if (strcmp(mode,channels[a]->users[b]->mode )!= 0 ){
					delete channels[a]->users[b]->mode;
					channels[a]->users[b]->mode = new char[strlen(mode)];
					strcpy (channels[a]->users[b]->mode,mode);
				}
			
				
			} 
			else{ // user bestaat niet in channel;
		
				ircuser *newuser;
				newuser = new ircuser;

				newuser->user = new char[strlen(user)];
				strcpy (newuser->user,user);
				newuser->host = new char[strlen(host)];
				strcpy (newuser->host,host);
				newuser->server = new char[strlen(server)];
				strcpy (newuser->server,server);
				newuser->nick = new char[strlen(nick)];
				strcpy (newuser->nick,nick);
				newuser->mode = new char[strlen(mode)];
				strcpy (newuser->mode,mode);
				newuser->realname = new char[strlen(realname)];
				strcpy (newuser->realname,realname);
				newuser->lasttime = time(NULL);
				newuser->lastsaid = NULL;
				//strcpy (newuser->lastsaid,"joined");
				newuser->oldnick=NULL;
				newuser->lines = 0;
				channels[channels.size()-1]->users.push_back(newuser);	
			}
		}
		else{//channel bestaat niet
		
			ircuser *newuser;
			newuser = new ircuser;
			
			newuser->user = new char[strlen(user)];
			strcpy (newuser->user,user);
			newuser->host = new char[strlen(host)];
			strcpy (newuser->host,host);
			newuser->server = new char[strlen(server)];
			strcpy (newuser->server,server);
			newuser->nick = new char[strlen(nick)];
			strcpy (newuser->nick,nick);
			newuser->mode = new char[strlen(mode)];
			strcpy (newuser->mode,mode);
			newuser->realname = new char[strlen(realname)];
			strcpy (newuser->realname,realname);
			newuser->lasttime = time(NULL);
			newuser->lastsaid = NULL;
			newuser->lines = 0;		
			newuser->oldnick=NULL;
			
			ircchannel *newchannel;
			newchannel = new ircchannel;
    		newchannel->channel = new char[strlen(channel)];
			strcpy (newchannel->channel,channel);
			channels.push_back(newchannel);
			channels[channels.size()-1]->users.push_back(newuser); 
		}
		
	
	
	printf("%s\n",channels[channels.size()-1]->users[channels[channels.size()-1]->users.size()-1]->nick);
		
	
	
}	

void verwerk (char type, char *nick, char *host, char *to, char *data)
{
	if (type==0 || type==1 ){
		if (type==0) printf("<%s> %s\n",nick,data);
		if (type==1) printf("* %s %s *\n",nick,data);
			
		if (strcmp(to,"bscp-test")==0){ // verander dit, botnick variable
			printf("%s zit in privé\n",nick);
		} 
		else {
			//channel message
			int a,b;
			
			for ( a = 0; (a < channels.size()) && 
			                       (strcmp (to,channels[a]->channel) != 0);a++);
				for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcmp (nick ,channels[a]->users[b]->nick) != 0); b++);
			//printf("nu praat %s\n",channels[a]->users[b]->nick);
			delete channels[a]->users[b]->lastsaid;
			// toevoeg type message?				
			channels[a]->users[b]->lastsaid = new char[strlen(data)];
		    strcpy(channels[a]->users[b]->lastsaid,data);
			channels[a]->users[b]->lasttime = time(NULL);
			channels[a]->users[b]->lines++;
			if (channels[a]->users[b]->lines==10) {
				printf("geef voice aan %s\n",nick);
				char temp[128]="MODE ";
				strcpy (temp+5,to);
				strcpy (temp+strlen(temp)," +v ");
				strcpy (temp+strlen(temp),nick);
				strcpy (temp+strlen(temp),"\xD\xA");
				send (sServer,temp,strlen(temp),0);
				printf("%s",temp);
			}
			printf("%s heeft al %d keer iets gezegd.\n",channels[a]->users[b]->nick,channels[a]->users[b]->lines);
				
		}
		
			
			
		
		if (data[0]=='!'){
			if (strncmp("!test",data,5)==0){
				char temp[128]="PRIVMSG ";
				strcpy(temp+8,to);
				strcpy(temp+strlen(temp)," Blah Blah Blah\xD\xA");
				send (sServer,temp,strlen(temp),0);
			}
		}
			
		
		// blah 
	}

	if (type==2){
		printf("-%s-%s\n",nick, data);
		//blah
	}
	if (type==JOIN){
		printf("%s joined %s\n",nick, to);
		char temp[128]="WHO ";
		strcpy(temp+4,to);
		send(sServer,temp,strlen(temp),0);
		send (sServer,"\x0D\x0A",2,0);
	}
	if (type==NICK){
	//if (false){//debug
		int a,b;
		//for (a = 0; a < channels.size();a++)
		a=0;
		while ( a < channels.size() )
		{
			for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcmp (nick ,channels[a]->users[b]->nick) != 0); b++);
			if ( b < channels[a]->users.size() ) // user bestaat in channel;
			{
				//printf("nickchange %s\n", channels[a]->channel);
				if (channels[a]->users[b]->oldnick!=NULL) delete channels[a]->users[b]->oldnick;
				channels[a]->users[b]->oldnick=channels[a]->users[b]->nick;
				channels[a]->users[b]->nick= new char[strlen(data)];
				strcpy(channels[a]->users[b]->nick,data);
			}
			a++;
		}
        printf("done\n");		
		
	}
	
	if (type==PART){
		int a,b;
		for ( a = 0; (a < channels.size()) && 
			                       (strcmp (to,channels[a]->channel) != 0);a++);
				for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcmp (nick ,channels[a]->users[b]->nick) != 0); b++);
				
				delete channels[a]->users[b]->mode;
				channels[a]->users[b]->mode = new char[2];
				strcpy(channels[a]->users[b]->mode,"P");
				channels[a]->users[b]->lasttime=time(NULL);
				
		
		//blah
	}
	if (type==QUIT){
		int a,b;
		for (a = 0; a < channels.size();a++)
			for ( b = 0; ( b < channels[a]->users.size() ) && 
					   ( strcmp (nick ,channels[a]->users[b]->nick) != 0); b++);
			if ( b < channels[a]->users.size() )
			{
				delete channels[a]->users[b]->mode;
				channels[a]->users[b]->mode = new char[2];
				strcpy(channels[a]->users[b]->mode,"Q");
				channels[a]->users[b]->lasttime=time(NULL);
			}
		
	}
	
	
	//printf("blah");
}

void irc_received(char *data)
{
    int NrParam;
    char *Param[25];
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
        if(*Ptr == ':' && NrParam > 2) { 
			done=true;
      		Param[NrParam] = Ptr + 1; 
		}
        Ptr++;
	}
    done = false;

  //debug code//
  for ( int a = 0 ; a <= NrParam ; printf("Param %d of %d : (%d) %s\n",a,NrParam,strlen(Param[a]),Param[a]),a++);
  printf("\n");
  //debug code//

  	if (strlen(Param[0])==4) { 
		if (strncmp (Param[0],"PING",4) == 0){
			Param[0][1]='O';
      		Param[0][4]=' ';
			//printf("debug:pong:  %s\n",Param[0]);
      		send (sServer,Param[0], strlen(Param[0]), 0);
			send (sServer,"\x0D\x0A",2,0);
		}
	}
  
    if (strlen(Param[1])==4){
		if (strncmp (Param[1],"JOIN",4) == 0 ||
			strncmp (Param[1],"PARN",4) == 0 ||
		    strncmp (Param[1],"NICK",4) == 0 ){ 
			//if (NrParam == 2 ) {  
				{
				char* nick;
          		char* mask=NULL;
		  		//char* channel=Param[2]+1;
          		nick = Param[0]+1;
          		Ptr = nick;
          		while(*Ptr != 0 && !done) {
					if(*Ptr == '!'){
						*Ptr = 0;
              			mask = Ptr+1;
              			done = true;
					}
            		Ptr++;
				}
        		// --- kies hier ---- 
				if (strncmp (Param[1],"JOIN",4) == 0) 
					verwerk(JOIN,nick,mask,Param[2]+1,NULL);
				if (strncmp (Param[1],"PART",4) == 0) 
					verwerk(PART,nick,mask,Param[2],Param[3]);
				if (strncmp (Param[1],"NICK",4) == 0) 
					verwerk(NICK,nick,mask,NULL,Param[2]+1);
				if (strncmp (Param[1],"QUIT",4) == 0) 
					verwerk(QUIT,nick,mask,Param[2],Param[3]);

			}
		}
	}
	
    if (strlen(Param[1])==7){
    	if (strncmp (Param[1],"PRIVMSG",7) == 0){
			if (NrParam == 3 ) { 
          		char* nick;
          		char* mask=NULL;
          		nick = Param[0]+1;
          		Ptr = nick;
          		while(*Ptr != 0 && !done) {
            		if(*Ptr == '!'){
              			*Ptr = 0;
              			mask = Ptr+1;
              			done = true;
					}
            		Ptr++;
				}
                if (Param[3][0] == 1){ // CTCP or ACTION
            		if (strlen(Param[3])>7){
						if (strncmp (Param[3],"\x01\x41\x43TION",7) == 0){
							// "\x01ACTION" werkt niet, pakt die AC als hex
							// en zegt dan out of range. gekke compiler.
						    Param[3][strlen(Param[3])-1]=0x00;
							//printf("%s%s\n",nick, Param[3]+7);
							verwerk(1,nick,mask,Param[2],Param[3]+8);
						}
					}
					if (strlen(Param[3])==9){
              			if (strncmp (Param[3],"\x01VERSION\x01",9) == 0){
                			printf("CTCP VERSION reveived from %s\n",nick);
							char temp[128]="NOTICE ";
                			strncpy(temp+7,nick, strlen(nick));
                			strncpy(temp+7+strlen(nick),\
					" :\x01VERSION bscp-bot pre-alpha testing\x01\x0D\x0A",40);
                			send (sServer,temp, strlen(temp), 0);
						}
					}
				}
		  		else  
		  		{
					//printf("<%s> %s\n",nick,Param[3]);			
					verwerk(0,nick,mask,Param[2],Param[3]);
				}
			}
		}
    }
    if (strlen(Param[1])==3) {
		if (strncmp (Param[1],"001",3) == 0){
			printf("Login Successfull\n");
  		}
		if (strncmp (Param[1],"352",3) == 0){
		
			//userlist 
			if (NrParam == 9){
				//printf("%s %s\n",Param[3],Param[7]);
				// hoe gaan we dit aanpakken
				// 2 = mynick
				// 3 = channel
				// 4 = username
				// 5 = hostmask
				// 6 = server
				// 7 = nick van persoon
				// 8 = mode
				// 9 = realname
				userlist(Param[3],Param[4],Param[5],Param[6],
				         Param[7],Param[8],Param[9]);
			}
		}
		
		
		if (strncmp (Param[1],"376",3) == 0){
       		printf("Ready to join\n");
       		#ifdef indreanet    
			send (sServer,"JOIN #bscp-testing\xD\xA",20,0);
			send (sServer,"JOIN #test\xD\xA",12,0);
			#else
			//send (sServer,"JOIN #test\xD\xA",12,0);
			send (sServer,"JOIN #blaatschaap\xD\xA",19,0); //moet ik wel /xd/xa 
			#endif
		}
		if (strncmp (Param[1],"433",3) == 0){
	  		if (NrParam == 4 ) {
		  		printf("Nick error: %s\n", Param[4]);
		  		char *nick = Param[3];
      	  		nick[strlen(nick)+1]=0x00;
		  		nick[strlen(nick)]='_';	  
		  		printf("Retrying with %s ...\n",nick);	      
		  		char temp[25]="NICK ";      
		  		strcpy (temp+5,nick);
          		strcpy (temp+(strlen(temp)),"\x0D\x0A");		  
	      		send (sServer,temp,strlen(temp),0);  
	  		}
	
  		}
		
    }

}


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
//------------------------------------------------------------------------------
//
// code from mutstalk project
//
    //int check;
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

int login (char* nick)
{
	printf("Connected... logging in as %s ...\n",nick);
    char temp[9999]="NICK ";
    strncpy(temp+5,nick, strlen(nick));
    strncpy(temp+5+strlen(nick),"\x0D\x0A",3);
    send (sServer,temp, strlen(temp), 0);
    strcpy(temp,"USER bscp bscp bscp :bcsp\x0D\x0A");
    send (sServer,temp, strlen(temp), 0);
    int received_size=0;

	
	
// hoe effeciënt met de sockets om te gaan??
	 //goto test2;
test:
    recv(sServer, temp+received_size, 1, 0);
    received_size++;
    if (temp[received_size-1] == 0x0A  ) { 
		// dit werkt dus met UNIX en DOS regeleinden, 
		// maar MAC regeleinden \x0D dus niet....
		// en zo lijkt het niet toegestaan te zijn
		// te scannen naar een \0 om het einde
		// van het bericht te vinden
		// en de hele buffer ontvangen lijkt ook geen optie (zie code onder)
		// dus hoe kan dit het efficiënst mogelijk opgelost worden.
		// lijkt mij voorlopen voor IRC geen probleem, maar er is een
		// protocol dat bij sommige berichten geen regeleinden meestuurt.
		// dacht dat dat hij het dAmn protocol zo was....
		temp[received_size] = 0x00;
		irc_received(temp);
      	received_size=0;
    }
    goto test; 


test2:	
	received_size=recv(sServer, temp, sizeof(temp), 0); 
	printf("r:  %s\n",temp);
	irc_received(temp);
	goto test2;
}

int main(int argc, char *argv[])
{
    joined = false;
#ifdef indreanet	
    if (connect("195.28.165.175",6667)==0) //indreanet
#else		
    if (connect("62.75.201.175",6667)==0) //chat4all  
#endif		
    	login ("bscp-test");
    return EXIT_SUCCESS;
}
