#include "../include/socketEnregistrement.h"

void * threadLogic(void * arg){
	printf("Réussi thread");

	char msg[1];
	socket_t * sd = (socket_t * ) arg;
	reponse_t rep;
	requete_t req;
	// sockEcoute est une variable externe
	while(read(sd->fd, msg, 1) > 0){
		//aquisition req clt
		//mise en forme
		//si fin dialogue alors break
		recevoir(sd,(generic)&rep,(pFct)str2rep);
		fprintf(stderr,REQ_STR_OUT"\n",rep.idRep,rep.verbRep,rep.optRep);
		req = traiterRegister(&rep, sd);
		envoyer(sd, (generic)&req, (pFct)req2str);
		fprintf(stderr,REQ_STR_OUT"\n",req.idReq,req.verbReq,req.optReq);


	}
		CHECK(close(sd->fd),"-- PB close() --");

}

int main(){
	socket_t sa;
	socket_t sockEcoute;

    pthread_t th;
    pthread_attr_t attr;
	sockEcoute = creerSocketEcoute(IP_HOST, PORT);
	while(1){
		sa=accepterClt(sockEcoute);
	CHECK_ZERO(pthread_attr_init(&attr), "T ERROR main 1");
    CHECK_ZERO(pthread_attr_setdetachstate(&attr,
            PTHREAD_CREATE_DETACHED),
             "T ERROR main 2");
	CHECK_ZERO(pthread_create(&th, &attr, 
                (pFctThread) threadLogic,
                (void*) &sa), 
                "T ERROR main 3");

		printf("Réussi main");
	}
	
    
}