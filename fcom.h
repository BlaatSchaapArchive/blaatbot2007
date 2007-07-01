#ifndef _FCOM_H
#define _FCOM_H
#include <vector>
using namespace std;

class cFileCommands{

	private:

		struct sCommands {
    		char *command;
    		char *action;
    	};

		int ReadFile();
		vector <sCommands> fcommands;  
	
	public:
		
		void Refresh();		
		void Clean();
		char * Reply(char * command,char * nick,char **pars);
		cFileCommands();
		~cFileCommands();
};

#endif
