/**
 * @file servApp.c
 * @brief Include of Implementation of the Belote card game
 * @details This program implements the French card game Belote for 4 players in 2 teams.
 *          It handles card dealing, trump selection, trick playing, and scoring.
 * @author Vaniet Quentin
 * @date 04/02/2026
 */

#include "../include/servApp.h"

/**
 * @brief Displays the main menu with available game options
 * @details Shows options to create a game, join a user's game, join a random game, or quit
 */
void afficherMenu () {
    printf("Jouer : \n");
    printf("1- Creer une partie\n");
    printf("2- Rejoindre la partie d'un utilisateur\n");
    printf("3- Rejoindre une partie aleatoire\n");
    printf("4- Quitter\n");
}

/**
 * @brief Prompts the user to enter their menu choice
 * @return The integer value corresponding to the user's choice
 */
int choisir () {
    int choix;
    printf("Entrez votre choix : ");
    scanf("%d", &choix);
    return choix;
}

/**
 * @brief Establishes a connection to the registration server (SE)
 * @details Prompts the user for their username, sends a connection request
 *          to the server, and processes the response
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param nomUtilisateur Buffer to store the username entered by the user
 * @return 0 on successful connection, -1 on connection error
 */
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

/**
 * @brief Creates a new game session on the registration server
 * @details Sends a request to create a game associated with the given username
 *          and waits for server confirmation
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param nomUtilisateur The username of the player creating the game
 * @return 0 on successful game creation, -1 on error
 */
int creerPartieSe (socket_t *sdSE, char * nomUtilisateur) {
    requete_t req;
    reponse_t rep;

    // montage de la requête
    req.idReq = 303;
    strcpy(req.verbReq, "CreerPartie");
    strcpy(req.optReq, nomUtilisateur);

    // envoie de la requête
    envoyer(sdSE, (generic)&req, (pFct) req2str);

    // traitement de la réponse du serveur
    recevoir(sdSE, (generic)&rep, (pFct) str2rep);

    if (rep.idRep == 401) {
        printf("Partie créée avec succès !\n");
        return 0;
    }
    else {
        printf("Erreur lors de la création de la partie\n");
        return -1;
    }
}

/**
 * @brief Requests and displays the list of available games with users
 * @details Sends a request to retrieve all active games and displays
 *          the list of game hosts that can be joined
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @return 0 on success, -1 on error during retrieval
 */
int demanderAffichagePartiesUtilisateur (socket_t *sdSE) {
    requete_t req;
    reponse_t rep;

    // initialisation de la requête
    req.idReq = 306;
    strcpy(req.verbReq, "AfficherPartiesUtilisateur");
    strcpy(req.optReq, ""); // pas d'options

    // envoie de la requête
    envoyer(sdSE, (generic)&req, (pFct) req2str);

    // traitement de la réponse du serveur
    recevoir(sdSE, (generic)&rep, (pFct) str2rep);

    // affichage des parties
    if (rep.idRep == 401) {
        char * parties = strtok(rep.optRep, "|");
        while (parties != NULL) {
            printf("Partie : %s\n", parties);
            parties = strtok(NULL, "|");
        }
        return 0;
    }
    else {
        printf("Erreur lors de la récupération des utilisateurs étant dans des parties\n");
        return -1;
    }

}

/**
 * @brief Allows the user to select and join a specific user's game
 * @details Prompts for a username, sends a request to join their game,
 *          and retrieves the IP address and port if successful
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param IPUtilisateur Buffer to store the IP address of the selected game host
 * @param portUtilisateur Pointer to store the port number of the selected game host
 * @return 0 on success, -1 if the game is full or on error
 */
 int choisirPartieUtilisateur (socket_t *sdSE, char * IPUtilisateur, int * portUtilisateur) {
    char nomUtilisateur[50];
    requete_t req;
    reponse_t rep;
    printf("Entrez le nom de l'utilisateur dont vous voulez rejoindre la partie : ");
    scanf("%s", nomUtilisateur);

    // envoyer la demande 
    req.idReq = 304;
    strcpy(req.verbReq, "RejoindrePartieUtilisateur");
    strcpy(req.optReq, nomUtilisateur);
    envoyer(sdSE, (generic)&req, (pFct) req2str);

    recevoir(sdSE, (generic)&rep, (pFct) str2rep);
    if (rep.idRep == 405) {
        char * token = strtok(rep.optRep, ":");
        strcpy(IPUtilisateur, token);
        token = strtok(NULL, "|");
        *portUtilisateur = atoi(token);
        printf("Récupération des informations de la partie réussie !\n");
        return 0;
    }
    else if (rep.idRep == 002) {
        printf("partie de l'utilisateur %s déjà pleine.\n", nomUtilisateur);
        return -1;
    }
    else {
        printf("Erreur lors de la tentative de rejoindre la partie de %s\n", nomUtilisateur);
        return -1;
    }
}

/**
 * @brief Manages a game as the host server
 * @details Creates a listening socket on port 6000, registers the game with the
 *          registration server, waits for 3 remote players to connect, initializes
 *          all players (including the local host as player 1), launches the game
 *          engine, and handles cleanup after game completion
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param nomUtilisateur The username of the player hosting the game
 */
void gestionPartie (socket_t *sdSE, char *nomUtilisateur) {
    players_t players;
    socket_t playerSocks[3];
    socket_t se;
    
    // Créer la socket d'écoute pour accepter les clients
    se = creerSocketEcoute("0.0.0.0", 6000);
    
    if(creerPartieSe(sdSE, nomUtilisateur) == 0) {
        printf("Partie créée avec succès !\n");
        printf("En attente de joueurs pour rejoindre la partie...\n");
        
        // Initialiser le tableau des joueurs
        int nbPlayers = 0;
        
        // Ajouter le serveur comme joueur 0
        if (!addPlayer(players, &nbPlayers)) {
            printf("Erreur lors de l'initialisation du serveur comme joueur\n");
            return;
        }
        players[0]->sock = NULL;  // Le serveur est un joueur local (pas de socket)
        printf("Vous êtes le joueur 1 (serveur)\n");
        
        // Attendre que 3 joueurs se connectent
        for(int i = 0; i < 3; i++) {
            printf("En attente du joueur %d...\n", i + 2);
            
            // Accepter la connexion d'un client
            playerSocks[i] = accepterClt(se);
            
            // Ajouter ce joueur à la structure
            if (!addPlayer(players, &nbPlayers)) {
                printf("Erreur lors de l'ajout du joueur %d\n", i + 2);
                break;
            }
            
            // Assigner la socket au joueur distant
            players[nbPlayers - 1]->sock = &playerSocks[i];
            
            printf("Joueur %d connecté ! (%d/4)\n", i + 2, nbPlayers);
        }
        
        // Vérifier que tous les joueurs sont bien là
        if (nbPlayers == PLAYERS_MAX) {
            printf("\n=== Tous les joueurs sont connectés ===\n");
            printf("Démarrage de la partie de Belote...\n\n");
            
            // LANCER LE MOTEUR DE JEU
            game(players);
            
            printf("\n=== Partie terminée ===\n");
            
            // Nettoyage : fermer les connexions avec les clients
            for(int i = 1; i < PLAYERS_MAX; i++) {
                if (players[i] != NULL && players[i]->sock != NULL) {
                    close(players[i]->sock->fd);
                }
            }
            
            // Libérer la mémoire des joueurs
            for(int i = 0; i < PLAYERS_MAX; i++) {
                if (players[i] != NULL) {
                    free(players[i]);
                    players[i] = NULL;
                }
            }
        } else {
            printf("Erreur : impossible de démarrer la partie (joueurs manquants)\n");
        }
    }
    else {
        printf("Erreur lors de la création de la partie\n");
    }
}

/**
 * @brief Joins an existing game hosted by another user
 * @details First displays the list of available games, then prompts the user
 *          to select a specific game to join. Connection details are retrieved
 *          for further client-side connection (TODO: implement client launch)
 * @param sdSE Pointer to the socket descriptor for the registration server
 */
void rejoindrePartie (socket_t *sdSE) {
    char IPUtilisateur[50];
    int portUtilisateur;
    
    // recherche de la partie de l'utilisateur et connexion
    if (demanderAffichagePartiesUtilisateur(sdSE) == 0) {
        if (choisirPartieUtilisateur(sdSE, IPUtilisateur, &portUtilisateur) == 0) {
            printf("Connexion à %s:%d\n", IPUtilisateur, portUtilisateur);
            client(IPUtilisateur, portUtilisateur);
        }
    }
}

/**
 * @brief Joins a random available game or creates a new one if none exists
 * @details Sends a request to join any available game. If a game is found,
 *          retrieves its connection details. If no game is available, automatically
 *          creates and hosts a new game instead
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param nomUtilisateur The username of the player
 * @return 0 on success (joined or created), -1 on error
 */
int rejoindrePartieAleatoire (socket_t *sdSE, char *nomUtilisateur) {
    requete_t req;
    reponse_t rep;
    char IPPartie[50];
    int portPartie;

    // envoyer la demande 
    req.idReq = 305;
    strcpy(req.verbReq, "RejoindrePartieAleatoire");
    strcpy(req.optReq, "");
    envoyer(sdSE, (generic)&req, (pFct) req2str);

    recevoir(sdSE, (generic)&rep, (pFct) str2rep);
    if (rep.idRep == 405) {
        char * token = strtok(rep.optRep, "|");
        strcpy(IPPartie, token);
        token = strtok(NULL, "|");
        portPartie = atoi(token);
        // lancement du client
        client(IPPartie, portPartie);
        return 0;
    }
    else if (rep.idRep == 005) {
        printf("Pas de partie disponible, lancement d'une nouvelle partie\n");
        gestionPartie(sdSE, nomUtilisateur);
        return 0;
    }
    else {
        printf("Erreur lors de la tentative de rejoindre une partie aléatoire\n");
        return -1;
    }
}