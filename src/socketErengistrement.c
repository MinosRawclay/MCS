#include "../include/socketEnregistrement.h"

void * threadLogic(void * arg){
	socket_t * sd = (socket_t * ) arg;
	reponse_t rep;
	requete_t req;
	// sockEcoute est une variable externe
	while(1){
		//aquisition req clt
		//mise en forme
		//si fin dialogue alors break
		recevoir(&sa,(generic)&rep,(pFct)str2rep);
		traiterRegister(&rep);
	}
		CHECK(close(sa.fd),"-- PB close() --");

}

