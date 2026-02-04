
/**
 *	\file		client.c
 *	\brief		Fonctions relatives aux clients
 *	\author		Maxime NEUQUELMAN
 *	\date		04/02/2026
 *	\version	1.0
 */

#include <stdio.h>

#include <ctype.h>

#include <client.h>
#include <moteur.h>
#include <data.h>
#include <libDial.h>
#include <libRepReq.h>
#include <session.h>

/**
 *	\fn			void clientHandler(char *serverAddr, int serverPort)
 *	\brief		Traitement du client
 *	\note		
 *	\result		
 */
void clientHandler(char *serverAddr, int serverPort){

    player_t player;

    // Socket d'appel :
    socket_t saServer;

    requete_t request;
    

    // Connexion avec le serveur :
    saServer = connecterClt2Srv(serverAddr, serverPort);

    printf("Vous avez rejoint la partie de %sn", serverAddr);

    // PAUSE("Appuyez sur ENTRER lorsque vous êtes prêt...");

    // Attente début de la partie :
    waitForRequest(&saServer, &request, REQ_LANCER_PARTIE);

    // Partie lancée :

    // Début d'une manche :

    gameRequestHandler(&saServer, &request, &player);

    return;
}


/**
 *	\fn			void waitForRequest(socket_t *sockEch, requete_t *request, requeteList_t requestCode)
 *	\brief		Attend une requête avec un certain code
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		request : requête modifiée par la fonction
 *	\param 		requestCode : code de requête attendu
 */
void waitForRequest(socket_t *sockEch, requete_t *request, requeteList_t requestCode){

    reponse_t response;

    request->idReq = -1;

    while (request->idReq != requestCode){
        recevoir(sockEch, request, (pFct) req2str);

        if (request->idReq != requestCode){

            response.idRep = ERR_MAUVAIS_CODE;
            strcpy(response.verbRep, "WRONG CODE");

            envoyer(sockEch, &response, (pFct) rep2str);

        } else {

            response.idRep = REP_ACK_LANCER_PARTIE;
            strcpy(response.verbRep, "OK");

            // Réponse OK à faire
            envoyer(sockEch, &response, (pFct) rep2str);

        }
    }

    return;
}



void gameRequestHandler(socket_t *sockEch, requete_t *request, player_t *currentPlayer){

    player_t newPlayer;
    enum card trumps[1];
    pli_t pli;
    int score1, score2;

    reponse_t response;

    while (1){

        recevoir(sockEch, request, (pFct) req2str);

        switch (request->idReq){

            case REQ_ENVOYER_DECK:

                // requestToCards
                req2player(request, &newPlayer);

                getCards(sockEch, currentPlayer, &newPlayer);
                break;


            case REQ_ENVOYER_PLI:

                // req2pli(request, &pli);
                str2Cards(request->optReq, pli, 4);

                getPli(sockEch, currentPlayer, &pli);
                break;


            case REQ_CHOIX_ATOUT:

                str2Cards(request->optReq, trumps, 1);

                keepTrump(sockEch, currentPlayer, trumps[0]);
                break;

            
            case REP_CHOIX_ATOUT_COULEUR:

                str2Cards(request->optReq, trumps, 1);

                keepTrump2(sockEch, currentPlayer, trumps[0]);

                break;


            case REQ_ENVOYER_SCORE:

                sscanf(request->optReq, "%d|%d", &score1, &score2);

                // + message, option pour relancer etc... ?
                afficherScore(score1, score2);

                response.idRep = REP_ACK_ENVOYER_SCORE;
                strcpy(response.verbRep, "OK");

                envoyer(sockEch, &response, NULL);

                // Fin de la manche
                return;

                break;
            
            default:
                perror("Requête reçu non prévue par le client...");
                break;
        }

    }

}


void getCards(socket_t *sockEch, player_t *currentPlayer, const player_t *newPlayer){

    reponse_t response;

    // recevoir(sockEch, request, (pFct) str2req);

    response.idRep = REP_ACK_ENVOYER_DECK;
    strcpy(response.verbRep, "OK");

    envoyer(sockEch, &response, (pFct) rep2str);

    // Ajout des cartes au deck :
    memcpy(currentPlayer->cards + getCardsAmount(currentPlayer), newPlayer->cards, getCardsAmount(newPlayer) * sizeof(int));

    
}

void getPli(socket_t *sockEch, player_t *currentPlayer, const pli_t *pli){
;
    reponse_t response;

    response.idRep = REP_ACK_ENVOYER_PLI;
    strcpy(response.verbRep, "OK");

    // Affichage du pli :
    afficherCards((enum card *) pli, 4);

    envoyer(sockEch, &response, (pFct) rep2str);

    return;

}


void keepTrump(socket_t *sockEch, player_t *currentPlayer, const enum card atout){

    reponse_t response;
    int yesNoResult = 0;

    response.idRep = REP_CHOIX_ATOUT;

    // Affichage de l'atout :
    printf("Affichage de l'atout :\n");
    afficherCards((enum card *) atout, 1);

    // Demande pour savoir si l'utilisateur veut prendre l'atout :
    yesNoResult = askYesNo("Prendre l'atout");
    

    // Prise du pli :
    if (yesNoResult){
        
        strcpy(response.verbRep, "take");
        strcpy(response.optRep, "1");

        currentPlayer->cards[getCardsAmount(currentPlayer)] = atout;

    // Non prise du pli :
    } else {

        strcpy(response.verbRep, "let");
        strcpy(response.optRep, "0");
    }

    // Envoi de la réponse au serveur :
    envoyer(sockEch, &response, (pFct) rep2str);
}


void keepTrump2(socket_t *sockEch, player_t *currentPlayer, const enum card atout){

    reponse_t response;
    int yesNoResult = 0;
    enum colorCard couleurChoisie = NONE;
    int temp = -1;

    response.idRep = REP_CHOIX_ATOUT;

    // Affichage de l'atout :
    printf("Affichage de l'atout :\n");
    afficherCards((enum card *) atout, 1);

    // Demande pour savoir si l'utilisateur veut prendre l'atout :
    yesNoResult = askYesNo("Prendre l'atout");
    

    // Prise du pli :
    if (yesNoResult){
        
        strcpy(response.verbRep, "take");

        currentPlayer->cards[getCardsAmount(currentPlayer)] = atout;

        while (temp < 1 || 4 < temp){
            printf("Veuillez choisir la couleur de l'atout :\n");
            printf("1 : trèfle\t| 3 : coeur\n");
            printf("2 : carreau\t| 4 : pique\n");

            scanf("%d", &temp);

        }

        switch (temp){
            case 1:
                couleurChoisie = T;
                break;

            case 2:
                couleurChoisie = C;
                break;

            case 3:
                couleurChoisie = H;
                break;

            case 4:
                couleurChoisie = P;
                break;
        
        }

        sprintf(response.optRep, "%d", couleurChoisie);

    // Non prise du pli :
    } else {

        strcpy(response.verbRep, "let");
        sprintf(response.optRep, "%d", couleurChoisie);
    }

    // Envoi de la réponse au serveur :
    envoyer(sockEch, &response, (pFct) rep2str);
}


void playMove(socket_t *sockEch, player_t *currentPlayer){

    enum card selectedCard = -1;

    reponse_t response;

    printf("C'est à vous de jouer !\n\n");

    printf("Affichage de vos cartes :\n");
    afficherCards(currentPlayer->cards, getCardsAmount(currentPlayer));

    const char nbCards = getCardsAmount(currentPlayer);

    while (selectedCard < 0 || nbCards - 1 < selectedCard){
        
        printf("Entrez l'indice de la carte à jouer (entre 0 et %d) : ", nbCards - 1);
        
        scanf("%d", &selectedCard);
        fflush(stdin);
    }

    // Création de la réponse pour le serveur :
    response.idRep = REP_ENVOI_CARTE;
    strcpy(response.verbRep, "PLAY");

    sprintf(response.optRep, "%d", selectedCard);

    // Envoi de la réponse au serveur :
    envoyer(sockEch, &response, (pFct) rep2str);

}


int getCardsAmount(const player_t *player){

    for (int i = 0; i < NB_CARD_HAND; i++){
        if (player->cards[i] == NOTHING){
            return i;
        }
    }

    return NB_CARD_HAND;

}


/**
 *	\fn			void req2player(const requete_t *request, player_t *player)
 *	\brief		Transforme les données d'une requête en structure player_t
 *	\param 		request : requête qui contient les informations sur la structure player_t
 *	\param 		player : Joueur que l'on souhaite récupérer à partir de la requête
 */
void req2player(const requete_t *request, player_t *player){

    int num = 0;
    int cards[NB_CARD_HAND];
    

    //          num s  cards :
    // sscanf(request->optReq, "%d|%d:%d:%d:%d:%d:%d:%d:%d:%d", &num, cards[0], cards[0], cards[0], cards[0], cards[0], cards[0], cards[0], cards[0]);

    string_to_card(request->optReq, cards);

    player->num = num;
    memcpy(player->cards, cards, sizeof(cards));

    //sscanf(str,REQ_STR_IN,&req->idReq,req->verbReq,req->optReq);

}


/**
 *	\fn			void req2pli(const requete_t *request, pli_t *pli)
 *	\brief		Transforme les données d'une requête en pli_t
 *	\param 		request : requête qui contient les informations sur la structure pli_t
 *	\param 		pli : Pli que l'on souhaite récupérer à partir de la requête
 */
void req2pli(const requete_t *request, pli_t *pli){

    // ...
        //          num s  cards :
    // sscanf(request->optReq, "%d:%d:%d:%d", &pli[0], &pli[1], &pli[2], &pli[3]);

    string_to_card(request->optReq, (enum card *) pli);

}


/**
 *	\fn			void req2card(const requete_t *request, enum card *c)
 *	\brief		Transforme les données d'une requête en carte
 *	\param 		request : requête qui contient les informations sur la structure pli_t
 *	\param 		carte : Carte que l'on souhaite récupérer à partir de la requête
 */
void req2card(const requete_t *request, enum card *c){

    sscanf(request->optReq, "%d", c);

}


/**
 *	\fn			void afficherScore(int score1, int score2)
 *	\brief		Affiche le score des équipes
 *	\param 		score1 : score de la 1ère équipe
 *	\param 		score2 : score de la 2nde équipe
 */
void afficherScore(int score1, int score2){
    printf("======================================\n");
    printf("=       Affichage des scores         =\n");
    printf("======================================\n");

    printf("Équipe 1 : \t%d\n", score1);
    printf("Équipe 2 : \t%d\n", score2);
};


/**
 *	\fn			int askYesNo(const char *message)
 *	\brief		Demande oui ou non et renvoie le résultat
 *	\param 		message : message à afficher
 *	\result     0 : non ; 1 : oui
 */
int askYesNo(const char *message){

    // Demande :
    printf("%s (y-N) ? ", message);
    
    char userResponse = getchar();

    fflush(stdin);

    // Retour du résultat :
    return tolower(userResponse) == 'y';

}
