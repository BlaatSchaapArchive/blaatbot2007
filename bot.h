#ifndef _BOT_H
#define _BOT_H
class cBot {
    bool isop(char *channel, char *nick);
    bool iscsregged(char *mode);
    bool isService(char *nick);
    bool iscsregged(char *channel, char *nick);
    void verwerk(char type, char *nick, char *host, char *to, char *data);

  public:
    void command(int type, char *nick, char *host, char *channel, char *data);
    char userlevel(char *mode, char *channel, char *nick, char *host);
    bool isop(char *channel);
};

#endif /* _BOT_H */
