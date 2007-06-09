#ifndef osinfo_h
#define osinfo_h

//------------------------------------------------------------------------------	
// Open Watcom compiler heeft geen __WIN32__ gedefineerd
// Dus moeten we dat zelf even doen. Later nog onderzoeken
// Dit is een tijdelijke oplossing, want de WatCom Compiler
// kent nog meerdere targets die om deze manier niet zullen
// compilen. (Win16, OS/2, DOS .... )
#if defined (__WATCOMC__) || (__WATCOM__)
#define __WIN32__
#endif    
//------------------------------------------------------------------------------	

void getOSinfo(char *&ostype, char *&osrelease, char *&osname, char *&machine);

#endif
