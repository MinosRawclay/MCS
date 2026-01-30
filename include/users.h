/**
 *	\file		users.h
 *	\brief		Spécification du gestionnaire d'utilisateurs et de leurs sessions
 *	\author		Samir El Khattabi
 *	\date		23 Janvier 2026
 *	\version	1.0
 */

#ifndef USERS_H
#define USERS_H

#include "session.h"

/**
 *	\def		MAX_USERS
 *	\brief		Nombre maximal de joueurs/utilisateurs autorisés
 */
#define MAX_USERS	16

/**
 *	\def		MAX_NAME
 *	\brief		Taille maximale du nom d'un utilisateur (incluant le \0)
 */
#define MAX_NAME    64

/**
 *	\def		IP_HOST
 *	\brief		IP du socket d écoute
 */
#define IP_HOST  "192.168.0.1"

/**
 *	\def		PORT
 *	\brief		Port du socket d ecoute
 */
#define PORT    50000

/*
 * S T R U C T U R E S   DE   D O N N E E S
 */

/**
 *	\typedef	name_t
 *	\brief		Type pour le nom de l'utilisateur
 */
typedef char name_t[MAX_NAME];

/**
 *	\struct		user_t
 *	\brief		Représente un utilisateur connecté ou enregistré
 */
typedef struct {
	name_t name; 		/**< Nom de l'utilisateur */
	socket_t *sDial;	/**< Pointeur vers la socket de dialogue active */
	int indDest;		/**< Index de l'interlocuteur (ligne destinataire) */
} user_t;

/**
 *	\struct		users_t
 *	\brief		Structure globale de gestion des utilisateurs
 */
typedef struct {
	user_t tab[MAX_USERS];	/**< Tableau des utilisateurs */
	int nbUsers;			/**< Nombre actuel d'utilisateurs enregistrés */
} users_t;

/*
 * P R O T O T Y P E S   DES   F O N C T I O N S
 */

/**
 *	\fn			int afficherUsers(char *cde)
 *	\brief		Affiche l'état actuel de la table des utilisateurs sur la sortie standard
 *	\param 		cde : Chaîne de caractères descriptive (contexte de l'appel)
 *	\return		0 
 */
int afficherUsers(char *cde); 

/**
 *	\fn			int trouverUser(name_t nom)
 *	\brief		Recherche un utilisateur par son nom
 *	\param 		nom : Le nom de l'utilisateur recherché
 *	\return		L'index de l'utilisateur dans le tableau, ou -1 s'il n'existe pas
 */
int trouverUser(name_t nom);

/**
 *	\fn			int creerUser(name_t nom, socket_t *sDial)
 *	\brief		Ajoute un nouvel utilisateur à la table
 *	\param 		nom : Nom du nouvel utilisateur
 *	\param 		sDial : Pointeur vers sa socket de dialogue
 *	\return		L'index du nouvel utilisateur, ou -1 si la table est pleine
 */
int creerUser(name_t nom, socket_t *sDial);

/**
 *	\fn			int identifierUser(socket_t *sDial)
 *	\brief		Gère la phase d'identification (lecture de la requête ID)
 *	\param 		sDial : Socket sur laquelle on attend l'identification
 *	\return		L'index de l'utilisateur identifié/créé, ou -1 en cas d'erreur
 */
int identifierUser(socket_t *sDial);

/**
 *	\fn			void deconnecterUser(int indUser)
 *	\brief		Ferme la connexion d'un utilisateur et réinitialise ses paramètres
 *	\param 		indUser : Index de l'utilisateur à déconnecter
 */
void deconnecterUser(int indUser);

/**
 *	\fn			void modifierDest(int indUser, name_t destName)
 *	\brief		Définit l'interlocuteur d'un utilisateur
 *	\param 		indUser : Index de l'utilisateur source
 *	\param 		destName : Nom de l'utilisateur cible
 */
void modifierDest(int indUser, name_t destName);

/**
 *	\fn			int indiceDest(int indUser)
 *	\brief		Récupère l'index du destinataire d'un utilisateur
 *	\param 		indUser : Index de l'utilisateur source
 *	\return		Index du destinataire (-1 si aucun)
 */
int indiceDest(int indUser);

/**
 *	\fn			char * nameUser(int indUser)
 *	\brief		Récupère le nom d'un utilisateur par son index
 *	\param 		indUser : Index de l'utilisateur
 *	\return		Pointeur vers la chaîne de caractères du nom, ou NULL si index invalide
 */
char * nameUser(int indUser);

/**
 *	\fn			socket_t *socketUser(int indUser)
 *	\brief		Récupère la socket associée à un utilisateur
 *	\param 		indUser : Index de l'utilisateur
 *	\return		Pointeur vers la structure socket_t, ou NULL si index invalide
 */
socket_t *socketUser(int indUser);

/**
 *	\fn			void lireUsers(void)
 *	\brief		Charge la base des utilisateurs depuis le fichier "users.dat"
 */
void lireUsers(void);

/**
 *	\fn			void ecrireUsers(void)
 *	\brief		Sauvegarde la base actuelle des utilisateurs dans le fichier "users.dat"
 */
void ecrireUsers(void);

#endif /* USERS_H */