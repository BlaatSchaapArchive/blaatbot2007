#ifndef _IRC_H
#define _IRC_H
//------------------------------------------------------------------------------	
#define loud
#define indreanet
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
#include <vector>

void joinchannel(char *channel);
void getChannelNick (int &a, int &b, char *channel, char *nick);
bool isop(char *channel, char *nick);
bool botisop(char *channel);
bool iscsregged(char *mode);
//	bool iscsregged(char *channel,char *nick);
void splitnickuser ( char *Ptr, char *&nick, char *&mask);
void sendNICK(char *nick);
void sendPRIVMSG(char *target, char *message);
void sendNOTICE(char *target, char *message);
void sendACTION(char *target, char *message);
char userlevel (char *mode, char *channel, char *nick, char *host);
void userlist (char *channel, char *user, char *host, char *server,
               char *nick,    char *mode, char *realname);
void setmode(char *channel, char *nick, char *mode);
void irc_received(char *data);
void login ();
void receivedata();
int connect_irc(char *ip, int port);
void sendAWAY(char *reason);
void sendBACK();
//------------------------------------------------------------------------------	
	extern char *botnick;
	extern SOCKET sServer;
	extern char *ostype,*osrelease,*osname,*machine;
//------------------------------------------------------------------------------	
	struct ircuser{ 
    	char *user; 
		char *host; 
		char *server;
    	char *nick;     
		char *mode; 
		char *realname; 
		time_t lasttime; 
		char lasttype;
		char *lastsaid; 
		int  lines; 
		char *oldnick; 
		char userlevel;
	};
//------------------------------------------------------------------------------	
	struct ircchannel{
		char *channel; 
		vector <ircuser *> users; 
		FILE *logfile;
	};
//------------------------------------------------------------------------------
#endif /* _IRC_H */
