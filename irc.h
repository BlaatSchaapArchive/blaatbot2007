#ifndef _IRC_H
#define _IRC_H

//#ifdef __cplusplus
//extern "C"
//{
//#endif
	
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
	void verwerk (char type, char *nick, char *host, char *to, char *data);
	void irc_received(char *data);
	void login ();
	void receivedata();
	int connect_irc(char *ip, int port);
	

	


	
	extern char *botnick;
	extern SOCKET sServer;
	extern char *ostype,*osrelease,*osname,*machine;
	
	
	
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
	

	struct ircchannel{
		char *channel; 
		vector <ircuser *> users; 
		FILE *logfile;
	};

	
	

	
	
//#ifdef __cplusplus
//}
//#endif

#endif /* _IRC_H */
