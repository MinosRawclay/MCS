
// TODO : Documentation
// TODO : Mode DEBUG ?

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


// // DANS MOTEUR.H :

// // Pas terminée
// void showCard(enum card c){

//     const char colorSymbols[4] = {'♥', '⯁', '♠', '♣'};

//     printf("%s\n", colorSymbols[c / 8]);

// }


// /**
//  *	\fn			void showPlayerCards(const player_t *player)
//  *	\brief		Affiche les cartes d'un joueur
//  *	\note		
//  *	\result		
//  */
// void showPlayerCards(const player_t *player){

//     if (player == NULL){
//         return;
//     }

//     for (char i = 0; i < nbCards / 2 - 1; i++){
//         printf("%d : %s\t\t|\t\t%d : %s\n", i, showCard(player->cards[i]), i + 1, showCard(player->cards[i]));
//     }

//     // Nombre de cartes impaire :
//     if (nbCards % 2 == 1){
//         printf("%d : %s\t\t|\n", nbCards - 1, showCard(player->cards[nbCards - 1]));
//     }
// }

// // FIN


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

    printf("Vous avez rejoint la partie de ...\n");

    // PAUSE("Appuyez sur ENTRER lorsque vous êtes prêt...");

    // Attente début de la partie :
    waitForRequest(&saServer, &request, REQ_LANCER_PARTIE);

    // Partie lancée :

    // Début d'une manche :

    gameRequestHandler(&saServer, &request, &player);

    // ...

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



                break;


            case REQ_ENVOYER_SCORE:
                // + message, option pour relancer etc... ?
                // afficherScore();

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
    printf("Le pli est : ...\n");
    afficherCards((enum card *) pli, 4);

    envoyer(sockEch, &response, (pFct) rep2str);

    return;

}


void keepTrump(socket_t *sockEch, player_t *currentPlayer, const enum card atout){

    reponse_t response;

    char userResponse;

    // Affichage de l'atout :
    printf("L'atout est : ...\n");

    // Demande pour savoir si l'utilisateur veut prendre le pli :
    printf("Prendre l'atout (y-N) ? ");
    
    userResponse = getchar();

    // fgets(userResponse, 1 * sizeof(char), stdin);
    fflush(stdin);

    response.idRep = REP_CHOIX_ATOUT;

    // Prise du pli :
    if (tolower(userResponse) == 'y'){
        
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
 *	\fn			
 *	\brief		Transforme les données d'une requête en carte
 *	\param 		request : requête qui contient les informations sur la structure pli_t
 *	\param 		carte : Carte que l'on souhaite récupérer à partir de la requête
 */
void req2card(const requete_t *request, enum card *c){

    sscanf(request->optReq, "%d", c);

}


void afficherScore(int score1, int score2){
    printf("c les score\n");
};