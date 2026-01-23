#ifndef USERS_H
#define USERS_H
#include "include/session.h"

#define MAX_NAME 64
typedef char name_t[MAX_NAME];
typedef struct {
	name_t name;
	socket_t *sDial;
	int indDest;
} user_t;
#define MAX_USERS	16
typedef struct {
	user_t tab[MAX_USERS];
	int nbUsers;
} users_t;

int trouverUser(char *nom);
int creerUser(char *nom, socket_t *sDial);
int identifierUser(socket_t *sDial);
void deconnecterUser(int indUser);
void modifierDest(int indUser, name_t dest);
int indiceDest(int indUser);
char * nameUser(int indUser);
socket_t *socketUser(int indUser); 
void lireUsers(void);
void ecrireUsers(void);
#endif /* USERS_H */ 
