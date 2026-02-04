#include "../include/libRepReq.h"

void req2str(const requete_t * req, char* str){
	//CHECK
	sprintf(str,REQ_STR_OUT,req->idReq,req->verbReq,req->optReq);
	#ifdef DEBUG
		fprintf(stderr,"requete recu");
		fprintf(stderr,REQ_STR_OUT"\n",req->idReq,req->verbReq,req->optReq);
	#endif
}

void str2req(const char* str, requete_t  * req){
	//CHECK
	sscanf(str,REQ_STR_IN,&req->idReq,req->verbReq,req->optReq);
	#ifdef DEBUG
		fprintf(stderr,"requete recu");
		fprintf(stderr,REQ_STR_OUT"\n",req->idReq,req->verbReq,req->optReq);
	#endif
}

void rep2str(const reponse_t * rep, char* str){
	//CHECK
	sprintf(str,REP_STR_OUT,rep->idRep,rep->verbRep,rep->optRep);
	#ifdef DEBUG
		fprintf(stderr,"reponse recu");
		fprintf(stderr,REQ_STR_OUT"\n",rep->idRep,rep->verbRep,rep->optRep);
	#endif
}

void str2rep(const char* str, reponse_t  * rep){
	//CHECK
	sscanf(str,REP_STR_IN,&rep->idRep,rep->verbRep,rep->optRep);
	#ifdef DEBUG
		fprintf(stderr,"reponse recu");
		fprintf(stderr,REQ_STR_OUT"\n",rep->idRep,rep->verbRep,rep->optRep);
	#endif
}

void traiterReq101(requete_t * req){
	fprintf(stderr,REQ_STR_OUT"\n",req->idReq,req->verbReq,req->optReq);
}

void traiterReq102(requete_t * req){
	fprintf(stderr,REQ_STR_OUT"\n",req->idReq,req->verbReq,req->optReq);
}

void traiterReq103(requete_t * req){
	fprintf(stderr,REQ_STR_OUT"\n",req->idReq,req->verbReq,req->optReq);
}

void traiterRep(reponse_t * rep){
		fprintf(stderr,REQ_STR_OUT"\n",rep->idRep,rep->verbRep,rep->optRep);
}

requete_t traiterRegister(reponse_t * rep, socket_t * sDial){
	int index;
	requete_t req;
	switch(rep->idRep){
		case 301:
			index=trouverUser( rep->optRep);
			if(index==-1)req.idReq = 3;
			else req.idReq = 401;
			break;

		case 302:
			deconnecterUser(identifierUser(sDial));
			break;

		case 303:
			creerPartie(sDial);
			req.idReq=401;
			break;

		case 304:
			if(isFull(rep->optRep)){ 
				req.idReq=2;
			}
			else{
				modifierDest(identifierUser(sDial), rep->optRep);
				req.idReq = 401;
			}
			break;

		case 305:
			for(int i = 0; i < MAX_USERS; i++){
				if(!isFull(i)){
					modifierDest(identifierUser(sDial),i );
					req.idReq=401;
					break;
					req.idReq=402;
					strcpy(req.optReq, "join aléatoire");
				}
			}
			break;

		default:
			req.idReq = 402;
			strcpy(req.optReq, "Demande inconnue");
			break;
	}
	return req;
}