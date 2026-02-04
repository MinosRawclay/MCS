#include "../include/servApp.h"

// déclarations des constantes
#define IP_SERVEUR_ENREGISTREMENT "127.0.0.1"
#define PORT_SERVEUR_ENREGISTREMENT 5000

int main () {
    char nomUtilisateur[50];
    int choix;
    socket_t sdSE;

    // initialisation de la connexion au serveur d'enregistrem
    sdSE = connecterClt2Srv(IP_SERVEUR_ENREGISTREMENT, PORT_SERVEUR_ENREGISTREMENT);

    // Connexion au Serveur d'Enregistrement
    while(connexionToSE(&sdSE, nomUtilisateur) != 0);

    // affichage du menu et traitement du choix
    while(1) {
        afficherMenu();
        choix = choisir();
        switch(choix) {
            case 1:
                // creation d'une nouvelle partie
                gestionPartie(&sdSE, nomUtilisateur);
                break;
            case 2:
                // rejoindre la partie d'un utilisateur
                rejoindrePartie(&sdSE);
                break;
            case 3:
                // recherche d'une partie aléatoire et connexion
                rejoindrePartieAleatoire(&sdSE, nomUtilisateur);
                break;
            case 4:
                printf("A la prochaine !\n");
                exit(0);
                break;
            default:
                printf("Demande non traitée, réessayez\n");
        }
    }
    return 0;
}