#include <stdio.h>
#include <string.h>
#include "libRepReq.h"

// déclarations des constantes
#define IP_SERVEUR_ENREGISTREMENT "127.0.0.1"
#define PORT_SERVEUR_ENREGISTREMENT 5000

int main () {
    char nomUtilisateur[50];
    int choix;
    socket_t sdSE;
    requete_t req;
    reponse_t rep;

    // initialisation de la connexion au serveur d'enregistrement
    sdSE = connecterClt2Srv(IP_SERVEUR_ENREGISTREMENT, PORT_SERVEUR_ENREGISTREMENT);

    // Connexion au Serveur d'Enregistrement
    while(connexionToSE(&sdSE, nomUtilisateur) != 0);

    // affichage du menu et traitement du choix
    afficherMenu();
    choix = choisir();
    switch(choix) {
        case 1:
            // creation du nouvelle partie et mise en place des threads necessaires
            req.idReq = 303;
            strcpy(req.verbReq, "CreerPartie");
            strcpy(req.optReq, nomUtilisateur);
            envoyer(&sdSE, (generic)&req, (pFct) req2str);
            break;
        case 2:
            printf("Rejoindre la partie d'un utilisateur\n");
            // recherche de la partie de l'utilisateur et connexion
            demanderAffichagePartiesUtilisateur();
            break;
        case 3:
            printf("Rejoindre une partie aleatoire\n");
            break;
        case 4:
            printf("Quitter\n");
            break;
        default:
            printf("Demande non traitée, réessayez\n");
    }
}

void afficherMenu () {
    printf("Jouer : \n");
    printf("1- Creer une partie\n");
    printf("2- Rejoindre la partie d'un utilisateur\n");
    printf("3- Rejoindre une partie aleatoire\n");
    printf("4- Quitter\n");
}

int choisir () {
    int choix;
    printf("Entrez votre choix : ");
    scanf("%d", &choix);
    return choix;
}



int connexionToSE (socket_t *sdSE, char * nomUtilisateur) {

    requete_t reqConnexion;
    reponse_t repServeurE;

    printf("Bienvenue sur Rebelotte !\n");
    printf("entrer votre pseudo : ");
    scanf("%s", nomUtilisateur);

    // montage de la requête
    reqConnexion.idReq = 301;
    strcpy(reqConnexion.verbReq, "Connexion");
    strcpy(reqConnexion.optReq, nomUtilisateur);

    // envoie de la requête
    envoyer(sdSE, (generic)&reqConnexion, (pFct) req2str);

    // traitement de la réponse du serveur
    recevoir(sdSE, (generic)&repServeurE, (pFct) str2rep);

    if (repServeurE.idRep == 401) {
        printf("Connexion reussie ! Bienvenue %s\n", nomUtilisateur);
        return 0;
    }
    else if (repServeurE.idRep == 402) {
        printf("Erreur de connexion\n");
        return -1;
    }
}

int creerPartieSE (socket_t *sdSE, char * nomUtilisateur) {
    // à implémenter
}