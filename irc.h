#ifndef _IRC_H
#define _IRC_H
//------------------------------------------------------------------------------	
//#define loud
#define indreanet
//------------------------------------------------------------------------------
// begin borland compilation fix
#if defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
#define strcasecmp strcmpi
#endif
// end borland compilation fix
//------------------------------------------------------------------------------	
//#ifdef __WIN32__              // compile for WIN32
//#if defined(__WIN32__) || defined(_Windows) // alle windows systemen

#if defined(__WIN32__) || defined(__WIN64__) // alleen 32 of 64 bit .. 
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

#define NICK 10
#define MODE 11
#define JOIN 12
#define PART 13
#define KICK 14
#define KILL 15
#define QUIT 16
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
#include <vector>

class IRCclient{
	
private:

	void splitnickuser ( char *Ptr, char *&nick, char *&mask);
	void splituserhost ( char *Ptr, char *&user, char *&host );
	void userlist (char *channel, char *user, char *host, char *server,
               char *nick,    char *mode, char *realname);
	void irc_received(char *data);
	void receivedata();
	void irc_message (char type, char *nick, char *host, char *to, char *data);
	SOCKET sServer;
	
	FILE *prive;  // en die file in het dingetje .... 
	

public:
	vector <ircchannel*> channels;

//	vector <ircuser *> pm; // allemaal bijhouden ... of een temp object
// maybe zelfs een 'temp-channel' als pm kanaal???
    ircuser pm;

	char *botnick;

	void joinchannel(char *channel);
	void partchannel(char *channel);

	void sendNICK(char *nick);
	void sendPRIVMSG(char *target, char *message);
	void sendNOTICE(char *target, char *message);
	void sendACTION(char *target, char *message);
	void sendAWAY(char *reason);
	void sendBACK();
	void sendQUIT(char *reason);

	char userlevel (char *mode, char *channel, char *nick, char *host);
	void setmode(char *channel, char *nick, char *mode);

	void getChannelNick (int &a, int &b, char *channel, char *nick);

	void login ();
	int connect_irc(char *ip, int port);
	
	
};
//------------------------------------------------------------------------------	
	//extern char *botnick;
	//extern SOCKET sServer;
	extern char *ostype,*osrelease,*osname,*machine;
//------------------------------------------------------------------------------
#endif /* _IRC_H */
