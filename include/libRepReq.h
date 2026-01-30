#include "data.h"
#include "users.h"

//1 req2str(req,str)
#define REQ_STR_OUT "%i:%s:%s" 

//2 str2req(str,req)
#define REQ_STR_IN "%i:%[^:]:%[^\n]" 

//3 rep2str(rep, str)
#define REP_STR_OUT "%i:%s:%s"

//4 str2rep(str rep)
#define REP_STR_IN "%i:%[^:]:%[^\n]"

typedef struct requete{
	short idReq;
	char verbReq[20];
	char optReq[100]; // a modifier selon le protocole applicatif
} requete_t;

typedef struct reponse{
	short idRep;
	char verbRep[20];
	char optRep[100]; // a modifier selon le protocole applicatif
} reponse_t;


// prototype des fonctions
void req2str(const requete_t * req, char * str);
void str2req(const char * str, requete_t * req);
void rep2str(const reponse_t * rep, char * str);
void str2rep(const char * str, reponse_t * rep);

void traiterReq101(requete_t * req);
void traiterReq102(requete_t * req);
void traiterReq103(requete_t * req);
void traiterRep(reponse_t * rep);

void DialSrv(socket_t sockEcoute);
void DialC(socket_t sd);