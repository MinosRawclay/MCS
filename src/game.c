/**
 * @file game.c
 * @brief Include of Implementation of the Belote card game
 * @details This program implements the main program for the user application
 *          of the French card game Belote for 4 players in 2 teams.
 * @author Vaniet Quentin
 * @date 04/02/2026
 */

#include "../include/servApp.h"

// déclarations des constantes
#define IP_SERVEUR_ENREGISTREMENT "127.0.0.1"
#define PORT_SERVEUR_ENREGISTREMENT 5000

// ==================== MAIN FUNCTION =========================================

/**
 * @brief Main entry point of the Belote game application
 * @details Initializes connection to the registration server, handles user
 *          authentication, and provides a menu-driven interface for game options:
 *          - Create a new game (hosting)
 *          - Join a specific user's game
 *          - Join a random available game
 *          - Quit the application
 * 
 *          The program runs in a continuous loop until the user chooses to quit.
 * @return 0 on successful program termination
 */
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