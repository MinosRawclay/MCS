/**
 *	\file		client.c
 *	\brief		Spécification des fonctions relatives aux clients
 *	\author		Maxime NEUQUELMAN
 *	\date		04/02/2026
 *	\version	1.0
 */
#ifndef CLIENT_H
#define CLIENT_H

#include <session.h>
#include <libRepReq.h>
#include <moteur.h>


/**
 *	\fn			void clientHandler(const char *serverAddr, int serverPort)
 *	\brief		Traitement du client
 *	\note		
 *	\result		
 */
void clientHandler(char *serverAddr, int serverPort);


/**
 *	\fn			void waitForRequest(socket_t *sockEch, requete_t *request, requeteList_t requestCode)
 *	\brief		Attend une requête avec un certain code
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		request : requête modifiée par la fonction
 *	\param 		requestCode : code de requête attendu
 */
void waitForRequest(socket_t *sockEch, requete_t *request, requeteList_t requestCode);


/**
 *	\fn			void gameRequestHandler(socket_t *sockEch, requete_t *request, player_t *currentPlayer)
 *	\brief		Fonction qui traite les requêtes relatives aux clients
 *	\param 		sockEch : socket d'échange utilisée
 *	\param 		request : requête modifiée par la fonction
 *	\param 		currentPlayer : joueur actuel
 */
void gameRequestHandler(socket_t *sockEch, requete_t *request, player_t *currentPlayer);


/**
 *	\fn			void getCards(socket_t *sockEch, player_t *currentPlayer, const player_t *newPlayer)
 *	\brief		Récupère les cartes envoyées par le serveur et les ajoute au joueur actuel
 *	\param 		sockEch : socket d'échange utilisée
 *	\param 		currentPlayer : joueur actuel
 *	\param 		newPlayer : Deck de carte envoyé par le serveur
 */
void getCards(socket_t *sockEch, player_t *currentPlayer, const player_t *newPlayer);


/**
 *	\fn			void getPli(socket_t *sockEch, player_t *currentPlayer, const pli_t *pli)
 *	\brief		Affiche le pli et envoie un acquittement au serveur
 *	\param 		sockEch : socket d'échange utilisée
 *	\param 		currentPlayer : joueur actuel
 *	\param 		pli : Pli reçu par le serveur
 */
void getPli(socket_t *sockEch, player_t *currentPlayer, const pli_t *pli);


/**
 *	\fn			void keepTrump(socket_t *sockEch, player_t *currentPlayer, const enum card atout)
 *	\brief		Permet à l'utilisateur de décider s'il souhaite prendre l'atout ou non
 *	\param 		sockEch : socket d'échange utilisée
 *	\param 		currentPlayer : joueur actuel
 *	\param 		pli : Pli reçu par le serveur
 */
void keepTrump(socket_t *sockEch, player_t *currentPlayer, const enum card atout);


/**
 *	\fn			void playMove(socket_t *sockEch, player_t *currentPlayer)
 *	\brief		Permet au client de jouer un coup
 *	\param 		sockEch : socket d'échange utilisée
 *	\param 		currentPlayer : joueur actuel
 */
void playMove(socket_t *sockEch, player_t *currentPlayer);


/**
 *	\fn			int getCardsAmount(const player_t *player)
 *	\brief		Renvoie le nombre de carte que possède un joueur
 *	\param 		player : joueur dont on souhaite connaître son nombre de cartes
 *  \result     Nombre de cartes que possède player
 */
int getCardsAmount(const player_t *player);


/**
 *	\fn			void req2player(const requete_t *request, player_t *player)
 *	\brief		Transforme les données d'une requête en structure player_t
 *	\param 		request : requête qui contient les informations sur la structure player_t
 *	\param 		player : Joueur que l'on souhaite récupérer à partir de la requête
 */
void req2player(const requete_t *request, player_t *player);


/**
 *	\fn			void req2pli(const requete_t *request, pli_t *pli)
 *	\brief		Transforme les données d'une requête en pli_t
 *	\param 		request : requête qui contient les informations sur la structure pli_t
 *	\param 		pli : Pli que l'on souhaite récupérer à partir de la requête
 */
void req2pli(const requete_t *request, pli_t *pli);


/**
 *	\fn			
 *	\brief		Transforme les données d'une requête en carte
 *	\param 		request : requête qui contient les informations sur la structure pli_t
 *	\param 		carte : Carte que l'on souhaite récupérer à partir de la requête
 */
void req2card(const requete_t *request, enum card *c);

#endif