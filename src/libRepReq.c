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

void traiterRegister(reponse_t * rep){
	switch(rep->idRep){
		case 300:
		
			break;

		case 301:
			break;

		case 302:
			break;

		case 303:
			break;

		case 304:
			break;

		case 305:
			break;

		default:
			break;
	}
}