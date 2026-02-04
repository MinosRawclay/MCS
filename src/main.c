#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "libRepReq.h"
#include "moteur.h"

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

void rejoindrePartie (socket_t *sdSE) {
    char IPUtilisateur[50];
    int portUtilisateur;
    
    // recherche de la partie de l'utilisateur et connexion
    if (demanderAffichagePartiesUtilisateur(sdSE) == 0) {
        if (choisirPartieUtilisateur(sdSE, IPUtilisateur, &portUtilisateur) == 0) {
            printf("Connexion à %s:%d\n", IPUtilisateur, portUtilisateur);
            // TODO: lancement de la fonction client
        }
    }
}

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