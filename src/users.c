#include <string.h>
#include <arpa/inet.h>
#include "../include/libRepReq.h"
#include "../include/users.h"
/*
*****************************************************************************************
 *	\note		D E F I N I T I O N   DES   M A C R O S
 */
/**
 *	\def		CHECK(sts, msg)
 *	\brief		Macro-fonction qui vérifie que sts est égal à -1 (cas d'erreur) 
 *				En cas d'erreur, il y a affichage du message adéquat et fin d'exécution  
 */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
/**
 *	\def		CHECK_ZERO(sts, msg)
 *	\brief		Macro-fonction qui vérifie que sts est différent de 0 (cas d'erreur) 
 *				En cas d'erreur, il y a affichage du message adéquat et fin d'exécution  
 */
#define CHECK_ZERO(sts, msg) if ((sts)!=0) {fprintf(stderr, "erreur threading: %s\n", msg); exit(-1);}
/**
 *	\def		CHECK_NULL(sts, msg)
 *	\brief		Macro-fonction qui vérifie que sts est égal à NULL (cas d'erreur) 
 *				En cas d'erreur, il y a affichage du message adéquat et fin d'exécution  
 */
#define CHECK_NULL(sts, msg) if ((sts)==NULL) {perror(msg); exit(-1);}



users_t users;

int afficherUsers(char *cde) {
	printf("[%s] Liste des users [%d]\n", cde, users.nbUsers);	
	for (int i=0; i < users.nbUsers; i++)
		if (users.tab[i].sDial != NULL)
			printf("\tUser [%d:%s], Socket [%d], IP [%s], Dest [%i]\n",
				i,users.tab[i].name, users.tab[i].sDial->fd,
				inet_ntoa((users.tab[i].sDial->addrDst).sin_addr),users.tab[i].indDest);
		else
			printf("\tUser [%d:%s], Socket [-1], IP [0.0.0.0], Dest [%i]\n",
				i,users.tab[i].name,users.tab[i].indDest);
}

int trouverUser(name_t nom) {
	int i=0;
	for (i=0; i < MAX_USERS; i++)
		if (strcmp(nom, users.tab[i].name)==0) return i;
	return -1;
}

int creerUser(name_t nom, socket_t *sDial) {
	if (users.nbUsers == MAX_USERS) return -1;
	strncpy(users.tab[users.nbUsers].name, nom, MAX_NAME-1);
	if (strlen(nom)==MAX_NAME-1) users.tab[users.nbUsers].name[MAX_NAME-1]='\0';
	users.tab[users.nbUsers].sDial=sDial;
	users.tab[users.nbUsers].indDest=-1;
	users.nbUsers++;
	//
	afficherUsers("créer");
	return users.nbUsers-1;
}

int identifierUser(socket_t *sDial) {
	requete_t req;
	int index = -1;
	recevoir(sDial, &req, (pFct)str2req);
	if (req.idReq==300)
		if ((index=trouverUser(req.optReq))==-1) index=creerUser(req.optReq, sDial);
		else { users.tab[index].sDial=sDial; users.tab[index].indDest=-1; }
	if (index==-1) CHECK(close(sDial->fd),"--close()--");
	//
	afficherUsers("identifier");
	return index;
} 
void deconnecterUser(int indUser) {
	printf("Déconnexion : User [%s], Socket [%d], IP [%s]\n",users.tab[indUser].name,
		users.tab[indUser].sDial->fd, inet_ntoa((users.tab[indUser].sDial->addrDst).sin_addr));
	CHECK(close(((socket_t *)users.tab[indUser].sDial)->fd),"--close()--");
	users.tab[indUser].sDial = NULL;
	users.tab[indUser].indDest = -1;
	//
	afficherUsers("déconnecter");
}

void modifierDest(int indUser, name_t destName) {
	users.tab[indUser].indDest = trouverUser(destName);
	//
	afficherUsers("modifier");
	//return indDest;
}
int indiceDest(int indUser) { return users.tab[indUser].indDest; }

char * nameUser(int indUser) {
	if (indUser==-1) return NULL;
	else return users.tab[indUser].name;
}
socket_t *socketUser(int indUser) {
	if (indUser==-1) return NULL;
	else return users.tab[indUser].sDial;
}

void lireUsers(void) {
	FILE *fp;
	CHECK_NULL(fp=fopen("users.dat", "r"), "--fopen()--");
	CHECK(users.nbUsers=fread(users.tab, sizeof(user_t), MAX_USERS, fp),"--fread()--");
	CHECK(fclose(fp),"--fclose()--");
}
void ecrireUsers(void) {
	FILE *fp;
	CHECK_NULL(fp=fopen("users.dat", "w"), "--fopen()--");
	CHECK(fwrite(users.tab, sizeof(user_t), users.nbUsers, fp),"--fread()--");
	CHECK(fclose(fp),"--fclose()--");	
}





//TODO
void creerPartie(socket_t * sDial){
	user_t host = users.tab[identifierUser(sDial)];
	host.party.list[0] = &host;
	host.party.nbJoueurs = 1;
}

int isFull(int idUser){
	return users.tab[idUser].party.nbJoueurs >=4 || users.tab[idUser].party.nbJoueurs <= 0;
}









