#ifndef _BOT_H
#define _BOT_H

bool isop(char *channel, char *nick);
bool botisop(char *channel);
bool iscsregged(char *mode);
//	bool iscsregged(char *channel,char *nick);
void botcommand(int type,char *nick, char *host, char *channel, char *data);
void verwerk (char type, char *nick, char *host, char *to, char *data);


#endif /* _BOT_H */
