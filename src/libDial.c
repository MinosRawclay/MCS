#include "../include/libDial.h"

void DialC(socket_t sd) {
    requete_t req; 
    reponse_t rep;
    int nbOctets;

    while(1) {
        recevoir(&sd, (generic)&req, (pFct)str2req);

        switch (req.idReq) {
            case 101:
                traiterReq101(&req);
                break;
            case 102:
                traiterReq102(&req);
                break;
            case 103:
                traiterReq103(&req);
                break;
            default:
                printf("Requête inconnue : %d\n", req.idReq);
                break;
        }

        rep.idRep = 201;
        strcpy(rep.verbRep, "Verb");
		strcpy(rep.optRep, "Opt");
        envoyer(&sd, (generic)&rep, (pFct)rep2str);
    }

    printf("Fermeture du dialogue.\n");
    CHECK(shutdown(sd.fd, SHUT_WR), "-- PB shutdown() --");
}

void DialSrv(socket_t sockEcoute){
	socket_t sa;		// socket de dialogue avec un client
	reponse_t rep;
	requete_t req;
	// sockEcoute est une variable externe
	while(1){
		req.idReq = 101;
		strcpy(req.verbReq, "Verb");
		strcpy(req.optReq, "opt");
		sa = accepterClt(sockEcoute);
		//aquisition req clt
		//mise en forme
		//si fin dialogue alors break
		envoyer(&sa,(generic)&req,(pFct)req2str);
		recevoir(&sa,(generic)&rep,(pFct)str2rep);
		traiterRep(&rep);
	}
		CHECK(close(sa.fd),"-- PB close() --");

}