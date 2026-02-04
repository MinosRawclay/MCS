/**
 * @file users.h
 * @brief Spécification du gestionnaire d'utilisateurs et de leurs sessions
 * @details Ce fichier définit les structures de données et les prototypes
 *          nécessaires à la gestion des utilisateurs côté serveur :
 *          - stockage des utilisateurs connectés
 *          - association utilisateur ↔ socket
 *          - gestion des destinations de communication
 *          - gestion des parties (création, nombre de joueurs)
 *
 *          Il est utilisé par les modules serveur responsables de
 *          l’authentification, de la communication et de la gestion des parties.
 *
 * @author Samir El Khattabi
 * @date 23 Janvier 2026
 * @version 1.0
 */

#ifndef USERS_H
#define USERS_H

#include "session.h"

typedef struct requete requete_t;
typedef struct reponse reponse_t;


/*============================================================================*/
/*                           CONSTANTES                                       */
/*============================================================================*/

/**
 * @def MAX_USERS
 * @brief Nombre maximal d'utilisateurs autorisés sur le serveur
 */
#define MAX_USERS 16

/**
 * @def MAX_NAME
 * @brief Taille maximale du nom d'un utilisateur (incluant le caractère '\0')
 */
#define MAX_NAME 64

/*============================================================================*/
/*                       TYPES ET STRUCTURES DE DONNÉES                        */
/*============================================================================*/

/**
 * @typedef name_t
 * @brief Type représentant le nom d'un utilisateur
 */
typedef char name_t[MAX_NAME];

struct user_s;
typedef struct user_s user_t;

/**
 * @struct party_t
 * @brief Structure représentant une partie de jeu
 * @details Une partie peut contenir jusqu'à 4 joueurs.
 *          Le tableau `list` contient des pointeurs vers les utilisateurs
 *          participant à la partie.
 */
typedef struct {
    user_t *list[4];   /**< Tableau des joueurs de la partie */
    int nbJoueurs;     /**< Nombre actuel de joueurs dans la partie */
} party_t;

/**
 * @struct user_t
 * @brief Représente un utilisateur du système
 * @details Un utilisateur est identifié par un nom, une socket de dialogue,
 *          un éventuel destinataire pour la communication, et une partie
 *          associée.
 */
struct user_s {
    name_t name;        /**< Nom de l'utilisateur */
    socket_t *sDial;    /**< Socket de dialogue associée */
    int indDest;        /**< Index de l'utilisateur destinataire (-1 si aucun) */
    party_t party;      /**< Partie associée à l'utilisateur */
};

/**
 * @struct users_t
 * @brief Structure globale de gestion des utilisateurs
 * @details Contient l'ensemble des utilisateurs enregistrés et le nombre
 *          courant d'utilisateurs actifs.
 */
typedef struct {
    user_t tab[MAX_USERS]; /**< Tableau des utilisateurs */
    int nbUsers;           /**< Nombre d'utilisateurs enregistrés */
} users_t;

/*============================================================================*/
/*                     PROTOTYPES DES FONCTIONS                                */
/*============================================================================*/

/**
 * @brief Affiche l'état actuel de la table des utilisateurs
 * @param cde Chaîne descriptive indiquant le contexte d'appel
 * @return 0
 */
int afficherUsers(char *cde);

/**
 * @brief Recherche un utilisateur à partir de son nom
 * @param nom Nom de l'utilisateur recherché
 * @return Index de l'utilisateur dans la table, ou -1 s'il n'existe pas
 */
int trouverUser(name_t nom);

/**
 * @brief Crée un nouvel utilisateur
 * @param nom Nom du nouvel utilisateur
 * @param sDial Socket de dialogue associée
 * @return Index du nouvel utilisateur ou -1 si la table est pleine
 */
int creerUser(name_t nom, socket_t *sDial);

/**
 * @brief Identifie un utilisateur à partir d'une requête réseau
 * @details Si l'utilisateur existe déjà, ses informations sont mises à jour.
 *          Sinon, un nouvel utilisateur est créé.
 * @param sDial Socket sur laquelle la requête est reçue
 * @return Index de l'utilisateur identifié ou -1 en cas d'erreur
 */

int identifierUser(socket_t *sDial, requete_t * req);
/**
 * @brief Déconnecte un utilisateur
 * @details Ferme la socket associée et réinitialise les informations
 *          de communication de l'utilisateur.
 * @param indUser Index de l'utilisateur à déconnecter
 */
void deconnecterUser(int indUser);

/**
 * @brief Définit le destinataire de communication d'un utilisateur
 * @param indUser Index de l'utilisateur source
 * @param destName Nom de l'utilisateur destinataire
 */
void modifierDest(int indUser, name_t destName);

/**
 * @brief Récupère l'index du destinataire d'un utilisateur
 * @param indUser Index de l'utilisateur
 * @return Index du destinataire ou -1 si aucun
 */
int indiceDest(int indUser);

/**
 * @brief Récupère le nom d'un utilisateur
 * @param indUser Index de l'utilisateur
 * @return Pointeur vers le nom ou NULL si index invalide
 */
char *nameUser(int indUser);

/**
 * @brief Récupère la socket associée à un utilisateur
 * @param indUser Index de l'utilisateur
 * @return Pointeur vers la socket ou NULL si index invalide
 */
socket_t *socketUser(int indUser);

/**
 * @brief Charge les utilisateurs depuis le fichier de sauvegarde
 * @details Les utilisateurs sont lus depuis le fichier "users.dat".
 */
void lireUsers(void);

/**
 * @brief Sauvegarde les utilisateurs dans le fichier de persistance
 * @details Les utilisateurs sont écrits dans le fichier "users.dat".
 */
void ecrireUsers(void);

/**
 * @brief Crée une nouvelle partie et désigne l'utilisateur comme hôte
 * @param sDial Socket de l'utilisateur créateur
 */
void creerPartieBDD(socket_t *sDial);

/**
 * @brief Vérifie si une partie est pleine ou invalide
 * @param idUser Index de l'utilisateur hôte
 * @return 1 si la partie est pleine ou invalide, 0 sinon
 */
int isFull(int idUser);


int userFromSocket(socket_t * sDial);


void ipPort(requete_t * req, reponse_t * rep);
#endif /* USERS_H */
