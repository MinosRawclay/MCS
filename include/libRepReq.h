#ifndef LIB_REP_REQ_H
#define LIB_REP_REQ_H

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


// Liste des codes de requêtes :
typedef enum requeteList {

	// Codes d'erreurs :
	ERR_MAUVAISE_CONNEXION = 1,
	ERR_PARTIE_PLEINE = 2,
	ERR_SERVEUR_PLEIN = 3,
	ERR_MAUVAIS_CODE = 4,
	ERR_SERVEUR_NON_DISPO = 5,


	REQ_LANCER_PARTIE = 100,

	// Fonctions de demande d'affichage :
	REQ_ENVOYER_DECK = 110,
	REQ_ENVOYER_PLI = 111,
	REQ_ENVOYER_SCORE = 112,

	// Fonctions de jeu :
	REQ_JOUER = 120,
	REQ_CARTE_LEGALE = 121,
	REQ_CHOIX_ATOUT = 122,
	REQ_CHOIX_ATOUT_COULEUR = 123,

	// Liste des codes de réponses :
	REP_ACK_LANCER_PARTIE = 200,
	REP_ACK_ENVOYER_DECK = 210,
	REP_ACK_ENVOYER_PLI = 211,
	REP_ACK_ENVOYER_SCORE = 212,
	REP_ENVOI_CARTE = 220,
	REP_ACK_CARTE_LEGALE = 221,
	REP_CHOIX_ATOUT = 222,
	REP_CHOIX_ATOUT_COULEUR = 223,

	// Liste des codes de communication :
	COM_CONNECTER = 301,
	COM_DECONNECTER = 302,
	COM_CREER_PARTIE = 303,
	COM_REJOINDRE_PARTIE = 304,
	COM_REJOINDRE_ALEA = 305,

	// Codes réponses communication :
	REP_ACK_CONNECTER = 401,
	REP_LIST_USERS = 403,


} requeteList_t;


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


requete_t traiterRegister(reponse_t * rep, socket_t * sDial);

#endif