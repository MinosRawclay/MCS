/**
 * @file users.c
 * @brief Gestion des utilisateurs et des parties côté serveur
 * @details Ce fichier contient l'ensemble des fonctions permettant de :
 *          - gérer les utilisateurs connectés (création, identification,
 *            déconnexion)
 *          - stocker et restaurer les utilisateurs depuis un fichier
 *          - gérer les destinations de communication entre utilisateurs
 *          - initialiser et contrôler les parties de jeu
 *
 *          Les utilisateurs sont stockés dans une structure globale `users`
 *          contenant un tableau d'utilisateurs et le nombre total courant.
 *
 * @author Alexandre BREVIERE
 * @date 04/02/2026
 */

#include <string.h>
#include <arpa/inet.h>
#include "../include/libRepReq.h"
#include "../include/users.h"

/*============================================================================*/
/*                           DÉFINITION DES MACROS                            */
/*============================================================================*/

/**
 * @def CHECK(sts, msg)
 * @brief Vérifie si une fonction système a échoué
 * @details Si le statut `sts` vaut -1, affiche le message d'erreur associé
 *          puis termine le programme.
 */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}

/**
 * @def CHECK_ZERO(sts, msg)
 * @brief Vérifie une erreur de retour non nul (threading)
 * @details Si `sts` est différent de 0, affiche un message d'erreur et
 *          termine le programme.
 */
#define CHECK_ZERO(sts, msg) if ((sts)!=0) {fprintf(stderr, "erreur threading: %s\n", msg); exit(-1);}

/**
 * @def CHECK_NULL(sts, msg)
 * @brief Vérifie un pointeur NULL
 * @details Si `sts` est NULL, affiche le message d'erreur et termine
 *          l'exécution du programme.
 */
#define CHECK_NULL(sts, msg) if ((sts)==NULL) {perror(msg); exit(-1);}

/*============================================================================*/
/*                         VARIABLES GLOBALES                                 */
/*============================================================================*/

/**
 * @brief Structure globale contenant tous les utilisateurs connectés
 */
users_t users;

/*============================================================================*/
/*                         FONCTIONS UTILISATEURS                             */
/*============================================================================*/

/**
 * @brief Affiche la liste des utilisateurs connectés
 * @param cde Chaîne indiquant le contexte d'appel (création, identification, etc.)
 * @return 0
 */
int afficherUsers(char *cde) {
    printf("[%s] Liste des users [%d]\n", cde, users.nbUsers);	
    for (int i=0; i < users.nbUsers; i++)
        if (users.tab[i].sDial != NULL)
            printf("\tUser [%d:%s], Socket [%d], IP [%s], Dest [%i]\n",
                i, users.tab[i].name, users.tab[i].sDial->fd,
                inet_ntoa((users.tab[i].sDial->addrDst).sin_addr),
                users.tab[i].indDest);
        else
            printf("\tUser [%d:%s], Socket [-1], IP [0.0.0.0], Dest [%i]\n",
                i, users.tab[i].name, users.tab[i].indDest);
    return 0;
}

/**
 * @brief Recherche un utilisateur par son nom
 * @param nom Nom de l'utilisateur recherché
 * @return Indice de l'utilisateur s'il est trouvé, -1 sinon
 */
int trouverUser(name_t nom) {
    for (int i = 0; i < MAX_USERS; i++)
        if (strcmp(nom, users.tab[i].name) == 0)
            return i;
    return -1;
}

/**
 * @brief Crée un nouvel utilisateur
 * @param nom Nom de l'utilisateur
 * @param sDial Socket de communication associée
 * @return Indice du nouvel utilisateur ou -1 si la liste est pleine
 */
int creerUser(name_t nom, socket_t *sDial) {
    if (users.nbUsers == MAX_USERS) return -1;

    strncpy(users.tab[users.nbUsers].name, nom, MAX_NAME - 1);
    if (strlen(nom) == MAX_NAME - 1)
        users.tab[users.nbUsers].name[MAX_NAME - 1] = '\0';

    users.tab[users.nbUsers].sDial = sDial;
    users.tab[users.nbUsers].indDest = -1;
    users.nbUsers++;

    afficherUsers("créer");
    return users.nbUsers - 1;
}

/**
 * @brief Identifie ou crée un utilisateur à partir d'une requête réseau
 * @param sDial Socket du client
 * @return Indice de l'utilisateur identifié ou -1 en cas d'échec
 */
int identifierUser(socket_t *sDial, requete_t * req) {
    int index = -1;


        if ((index = trouverUser(req->optReq)) == -1)
            index = creerUser(req->optReq, sDial);
        else {
            users.tab[index].sDial = sDial;
            users.tab[index].indDest = -1;
        }
    

        return index;
}

/**
 * @brief Déconnecte un utilisateur
 * @param indUser Indice de l'utilisateur à déconnecter
 */
void deconnecterUser(int indUser) {
    printf("Déconnexion : User [%s], Socket [%d], IP [%s]\n",
           users.tab[indUser].name,
           users.tab[indUser].sDial->fd,
           inet_ntoa((users.tab[indUser].sDial->addrDst).sin_addr));

    CHECK(close(users.tab[indUser].sDial->fd), "--close()--");

    users.tab[indUser].sDial = NULL;
    users.tab[indUser].indDest = -1;

    afficherUsers("déconnecter");
}

/**
 * @brief Modifie la destination de communication d'un utilisateur
 * @param indUser Indice de l'utilisateur source
 * @param destName Nom de l'utilisateur destination
 */
void modifierDest(int indUser, name_t destName) {
    users.tab[indUser].indDest = trouverUser(destName);
    afficherUsers("modifier");
}

/**
 * @brief Récupère l'indice de destination d'un utilisateur
 * @param indUser Indice de l'utilisateur
 * @return Indice de l'utilisateur destination
 */
int indiceDest(int indUser) {
    return users.tab[indUser].indDest;
}

/**
 * @brief Récupère le nom d'un utilisateur
 * @param indUser Indice de l'utilisateur
 * @return Nom de l'utilisateur ou NULL si invalide
 */
char *nameUser(int indUser) {
    return (indUser == -1) ? NULL : users.tab[indUser].name;
}

/**
 * @brief Récupère la socket associée à un utilisateur
 * @param indUser Indice de l'utilisateur
 * @return Pointeur vers la socket ou NULL si invalide
 */
socket_t *socketUser(int indUser) {
    return (indUser == -1) ? NULL : users.tab[indUser].sDial;
}

/*============================================================================*/
/*                         PERSISTANCE DES DONNÉES                            */
/*============================================================================*/

/**
 * @brief Charge les utilisateurs depuis un fichier
 */
void lireUsers(void) {
    FILE *fp;
    CHECK_NULL(fp = fopen("users.dat", "r"), "--fopen()--");
    CHECK(users.nbUsers = fread(users.tab, sizeof(user_t), MAX_USERS, fp), "--fread()--");
    CHECK(fclose(fp), "--fclose()--");
}

/**
 * @brief Sauvegarde les utilisateurs dans un fichier
 */
void ecrireUsers(void) {
    FILE *fp;
    CHECK_NULL(fp = fopen("users.dat", "w"), "--fopen()--");
    CHECK(fwrite(users.tab, sizeof(user_t), users.nbUsers, fp), "--fwrite()--");
    CHECK(fclose(fp), "--fclose()--");
}

/*============================================================================*/
/*                         GESTION DES PARTIES                                */
/*============================================================================*/


int userFromSocket(socket_t * sDial){
    for(int i = 0; i < users.nbUsers; i++){
        if(users.tab[i].sDial.fd == sDial.fd)
            return i;
    }
    return -1;
}
/**
 * @brief Crée une nouvelle partie et définit l'utilisateur comme hôte
 * @param sDial Socket de l'utilisateur hôte
 */
void creerPartieBDD(socket_t *sDial) {
    int index = userFromSocket(sDial);
    if (index == -1) return; // Sécurité

    // On récupère l'ADRESSE de l'utilisateur dans le tableau global
    user_t *hostPtr = &users.tab[index];

    // On modifie directement la structure globale
    hostPtr->party.list[0] = hostPtr; 
    hostPtr->party.nbJoueurs = 1;
    
    printf("DEBUG: Partie créée pour %s (Index: %d)\n", hostPtr->name, index);
}

/**
 * @brief Vérifie si une partie est pleine ou invalide
 * @param idUser Indice de l'utilisateur hôte
 * @return 1 si la partie est pleine ou invalide, 0 sinon
 */
int isFull(int idUser) {
    return users.tab[idUser].party.nbJoueurs >= 4 ||
           users.tab[idUser].party.nbJoueurs <= 0;
}

void ipPort(requete_t * req, reponse_t * rep){
    // Get game connection information (IP and port)
            socket_t * sa = socketUser(trouverUser(rep->optRep));
            char buffer[64]; 
    
            char *ip_text = inet_ntoa(sa->addrDst.sin_addr);
    
            int port = ntohs(sa->addrDst.sin_port);
            
            snprintf(buffer, sizeof(buffer), "%s|%d", ip_text, port);
            req->idReq = 405;
            strcpy(req->optReq, buffer);
}

int getNbUsers(){
    return users.nbUsers;
}