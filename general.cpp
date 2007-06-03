void spltstr(char *data, int &NrParam, char **Param,char nrspace){
    
    char teller=0;
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
			if (nrspace == NrParam) done=true;			
	}
        if(*Ptr == ':' && NrParam > 2 && !done) { 
            done=true;
     	Param[NrParam] = Ptr + 1;
		
	}
        Ptr++;
    }
    done = false;

}
