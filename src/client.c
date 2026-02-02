
// TODO : Documentation
// TODO : Mode DEBUG ?

#include <stdio.h>
#include "../include/moteur.h"
#include "../include/data.h"
#include "../include/libDial.h"
#include "../include/libRepReq.h"
#include "../include/session.h"


// DANS MOTEUR.H :

// Pas terminée
void showCard(enum card c){

    const char colorSymbols[4] = {'♥', '⯁', '♠', '♣'};

    printf("%s\n", colorSymbols[c / 8]);

}


/**
 *	\fn			void showPlayerCards(const player_t *player)
 *	\brief		Affiche les cartes d'un joueur
 *	\note		
 *	\result		
 */
void showPlayerCards(const player_t *player){

    if (player == NULL){
        return;
    }

    for (char i = 0; i < nbCards / 2 - 1; i++){
        printf("%d : %s\t\t|\t\t%d : %s\n", i, showCard(player->cards[i]), i + 1, showCard(player->cards[i]));
    }

    // Nombre de cartes impaire :
    if (nbCards % 2 == 1){
        printf("%d : %s\t\t|\n", nbCards - 1, showCard(player->cards[nbCards - 1]));
    }
}

// FIN


/**
 *	\fn			void threadClient(void)
 *	\brief		Traitement du client
 *	\note		
 *	\result		
 */
void threadClient(void){

    player_t player;

    // Socket d'appel :
    socket_t saServer;

    requete_t request;
    reponse_t response;
    

    // Connexion avec le serveur :
    saServer = creerSocketAdr(serverAddr, serverPort);

    printf("Vous avez rejoint la partie de ...\n");

    PAUSE("Appuyez sur ENTRER lorsque vous êtes prêt...");

    // Attente début de la partie :
    waitForRequest(&saServer, &request, REQ_LANCER_PARTIE);


    // Partie lancée :

    // Début d'une manche :

    gameRequestHandler(&saServer, &request);

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

    request->idReq = -1;

    while (request.idReq != requestCode){
        recevoir(sockEch, request, str2req);

        if (request.idReq != requestCode){

            // Réponse PAS OK à faire

            envoyer(sockEch, response, rep2str);
        } else {

            // Réponse OK à faire
            envoyer(sockEch, response, rep2str);

        }
    }

    return;
}



void gameRequestHandler(socket_t *sockEch, requete_t *request, player_t *currentPlayer){

    while (1){

        recevoir(sockEch, request, str2req);

        switch (request->idReq){

            case REQ_ENVOYER_DECK:
                getCards(sockEch, currentPlayer);
                break;

            case REQ_ENVOYER_PLI:
                getPli();
                break;

            case REQ_CHOIX_ATOUT:
                keepTrump();
                break;

            case REQ_ENVOYER_SCORE:
                // + message, option pour relancer etc... ?
                afficherScore();

                break;
            
            default:
                perror("Requête reçu non prévue par le client...");
                break;
        }

    }

}

void getCards(socket_t *sockEch, player_t *currentPlayer){

    requete_t *request;
    reponse_t *response;

    player_t *player;

    recevoir(sockEch, request, str2req);

    response->idRep = REQ_OK;
    strcpy(response->verbRep, "OK");

    envoyer(sockEch, response, rep2str);

    // Ajout des cartes au deck :
    memcpy(currentPlayer->cards + len(currentPlayer->cards), player->cards, sizeof(len(player->cards)));

    
}

void getPli(socket_t *sockEch, player_t *currentPlayer){

    requete_t *request;
    reponse_t *response;

    pli_t pli;


    recevoir(sockEch, request, str2req);

    // Affichage du pli :
    printf("Le pli est : ...\n");

    envoyer(sockEch, REP_ACK_ENVOYER_PLI, ...);

    return;

}


void keepTrump(){

    requete_t *request;
    reponse_t *response;

    pli_t pli;

    char userResponse;

    recevoir(sockEch, request, str2req);

    // Affichage du pli :
    printf("L'atout est : ...\n");

    // Demande pour savoir si l'utilisateur veut prendre le pli :
    printf("Prendre l'atout (y-N) ? ");
    
    userResponse = getchar();

    // fgets(userResponse, 1 * sizeof(char), stdin);
    fflush(stdin);

    // Prise du pli :
    if (tolower(userResponse) == 'y'){
        response->idRep = REQ_TAKE_PLI;
        strcpy(response->verbRep, "take");

        currentPlayer->cards[...] = pli;

    // Non prise du pli :
    } else {
        response->idRep = REQ_LET_PLI;
        strcpy(response->verbRep, "let");
    }

    // Envoi de la réponse au serveur :
    envoyer(sockEch, response, rep2str);
}


void playMove(socket_t *sockEch, player_t *currentPlayer){

    enum card selectedCard = -1;

    reponse_t response;

    printf("C'est à vous de jouer !\n\n");

    printf("Affichage de vos cartes :\n");
    showCards(currentPlayer);

    while (selectedCard < 0 || nbCards - 1 < selectedCard){
        
        printf("Entrez l'indice de la carte à jouer (entre 0 et %d) : ", nbCards - 1);
        
        selectedCard = atoi(getchar());
        fflush(stdin);
    }

    // Création de la réponse pour le serveur :
    response.idRep = REP_ENVOI_CARTE;
    strcpy(response.verbRep, "PLAY");
    response.optRep = selectedCard;

    // Envoi de la réponse au serveur :
    envoyer(sockEch, response, ...);

}
// ...
