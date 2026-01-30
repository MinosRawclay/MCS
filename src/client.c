
#include <stdio.h>
#include "../include/data.h"
#include "../include/libDial.h"
#include "../include/libRepReq.h"
#include "../include/session.h"


/**
 *	\fn			void threadClient(void)
 *	\brief		Traitement du client
 *	\note		si le paramètre deSerial vaut NULL alors quoi est une chaîne de caractères
 *	\result		paramètre quoi modifié avec le requête/réponse reçue
 *				paramètre sockEch modifié pour le mode DGRAM
 */
void threadClient(void){

    // Socket d'appel :
    socket_t saServer;

    requete_t request;
    reponse_t response;
    

    // Connexion avec le serveur :
    saServer = creerSocketAdr(adresseServeur, portServeur);

    printf("Vous avez rejoint la partie de ...\n");

    PAUSE("Appuyez sur ENTRER lorsque vous êtes prêt...");

    // Attente début de la partie :
    waitForRequest(&sa, &request, REQ_PARTIE_LANCEE);


    // Partie lancée :

    // Début d'une manche :

    traiterRequetesPartie(&saServer, &request);

    return;
}


void waitForRequest(socket_t *sockEch, requete_t *request, requeteList_t requestNb){

    request->idReq = -1;

    while (requete.idReq != REQ_PARTIE_LANCEE){
        recevoir(sockEch, request, str2req);

        if (requete.idReq != REQ_PARTIE_LANCEE){

            // Réponse PAS OK à faire

            envoyer(sockEch, response, str2req);
        } else {

            // Réponse OK à faire
            envoyer(sockEch, response, str2req);

        }
    }

    return;
}



void traiterRequetesPartie(socket_t *sockEch, requete_t *request){

    while (1){

        recevoir(sockEch, request, str2req);

        switch (request->idReq)
        {
        case constant expression:
            
            break;
        
        default:
            break;
        }

    }

}

void recupererCartes(){
    
}

void recupererPli(){

}

// ...
