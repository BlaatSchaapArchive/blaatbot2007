/*
BlaatSchaap Coding Projects : IRC BOT IN C / C++
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
//#include <string>
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
#define QUIT 12

void verwerk (char type, char *nick, char *host, char *to, char *data)
{
	if (type==0){
		printf("<%s> %s\n",nick,data);
		// blah 
	}
	if (type==1){
		printf("%s %s\n",nick, data);
		//blah
	}
	if (type==2){
		printf("-%s-%s\n",nick, data);
		//blah
	}
	if (type==10){
		printf("%s joined %s\n",nick, to);
		char temp[128]="WHO ";
		strcpy(temp+4,to);
		send(sServer,temp,strlen(temp),0);
		send (sServer,"\x0D\x0A",2,0);
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
		if (strncmp (Param[1],"JOIN",4) == 0){
			if (NrParam == 2 ) {  
				char* nick;
          		char* mask=NULL;
		  		char* channel=Param[2]+1;
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
        		//printf("%s joined %s\n",nick, channel);		  
				verwerk(10,nick,mask,channel,NULL);
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
				printf("%s %s\n",Param[3],Param[7]);
				// hoe gaan we dit aanpakken
				// 2 = mynick
				// 3 = channel
				// 4 = username
				// 5 = hostmask
				// 6 = server
				// 7 = nick van persoon
				// 8 = mode
				// 9 = realname
			}
		}
		
		
		if (strncmp (Param[1],"376",3) == 0){
       		printf("Ready to join\n");
       		//send (sServer,"JOIN #bscp-testing\x0D\x0A",20,0);
			send (sServer,"JOIN #test\x0D\x0A",12,0);
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
    //if (connect("195.28.165.175",6667)==0) //indreanet
    if (connect("62.75.201.175",6667)==0) //chat4all  
    	login ("bscp-test");
    return EXIT_SUCCESS;
}
