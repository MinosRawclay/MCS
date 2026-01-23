/**
 *	\file		data.c
 *	\brief		Implémentation de la couche Data Representation
 *	\author		Alexandre BREVIERE
 *	\date		7 janvier 2026
 *	\version	1.0
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/data.h"




/*
*****************************************************************************************
 *	\noop		DEFINITION  DES   MACROS
 */
/**
 *	\def		CHECK(sts, msg)
 *	\brief		Macro-fonction qui vérifie que sts est égal -1 (cas d'erreur : sts==-1) 
 *				En cas d'erreur, il y a affichage du message adéquat et fin d'exécution  
 */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
/**
 *	\def		PAUSE(msg)
 *	\brief		Macro-fonction qui affiche msg et attend une entrée clavier  
 */
#define PAUSE(msg)	printf("%s [Appuyez sur entrée pour continuer]", msg); getchar();

/*
*****************************************************************************************
 *	\noop		D E F I N I T I O N   DES   C O N S T A N T E S
 */
/**
 *	\def		MAX_BUFFER
 *	\brief		taille d'un buffer_t d'émission/réception
 */
#define MAX_BUFFER	1024
/**
 *	\def		RECV_FLAGS
 *	\brief		Flags à utiliser en réception
 */
#define RECV_FLAGS 	0
/**
 *	\def		SEND_FLAGS
 *	\brief		Flags à utiliser en émission
 */
#define SEND_FLAGS 	0
/*
*****************************************************************************************
 *	\noop		D E C L A R A T I O N   DES   V A R I A B L E S    G L O B A L E S
 */
/**
 *	\var		progName
 *	\brief		Nom de l'exécutable
 *	\note		Variable externe : à déclarer par l'utilisateur
 */
extern char *progName;
/*
*****************************************************************************************
 *	\noop		I M P L E M E N T A T I O N   DES   F O N C T I O N S
 *					M O D E    D G R A M
 */

/**
 *	\fn			void envoyerMessDGRAM (socket_t *sockEch, char *msg, cchar *adrDest, short portDest)
 *	\brief		Envoi d'un message sur une socket en mode DGRAM
 *	\param 		sockEch : socket d'échange à utiliser pour l'envoi
 *	\param 		msg : message à envoyer
 *	\param 		adrDest : adresse IP du destinataire
 *	\param 		portDest : port du destinataire
 */
void envoyerMessDGRAM (socket_t *sockEch, char *msg, char *adrDest, short portDest) ;
/**
 *	\fn			void recevoirMessDGRAM (socket_t *sockEch, char *msg, int msgSize)
 *	\brief		Réception d'un message sur une socket en mode DGRAM
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		msg : message reçu
 *	\param 		msgSize : taille de l'espace mémoire préalablement alloué à msg
 *	\result		paramètre modifié avec le message reçu
 */
void recevoirMessDGRAM (socket_t *sockEch, char *msg, int msgSize) ;
/*
*****************************************************************************************
 *	\noop		I M P L E M E N T A T I O N   DES   F O N C T I O N S
 *					M O D E    S T R E A M
 */
/**
 *	\fn			void envoyerMessSTREAM (const socket_t *sockEch, char *msg)
 *	\brief		Envoi d'un message sur une socket en mode STREAM
 *	\param 		sockEch : socket d'échange à utiliser pour l'envoi
 *	\param 		msg : message à envoyer
*/
void envoyerMessSTREAM (const socket_t *sockEch, char *msg) ;
/**
 *	\fn			void recevoirMessSTREAM (const socket_t *sockEch, char *msg, int msgSize)
 *	\brief		Réception d'un message sur une socket en mode STREAM
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		msg	 : message reçu
 *	\param 		msgSize : taille de l'espace mémoire préalablement alloué à msg
 *	\result		paramètre modifié avec le message reçu
 */
void recevoirMessSTREAM (const socket_t *sockEch, char *msg, int msgSize) ;
/*
*****************************************************************************************
 *	\noop		I M P L E M E N T A T I O N   DES   F O N C T I O N S
 *					M O D E    D G R A M / S T R E A M
 */
/**
 *	\fn			void envoyer(socket_t *sockEch, generic quoi, pFct serial, ...)
 *	\brief		Envoi d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour l'envoi
 *	\param 		quoi : requête/réponse à serialiser avant l'envoi
 *	\param 		serial : pointeur sur la fonction de serialisation d'une requête/réponse
 *	\note		si le paramètre serial vaut NULL alors quoi est une chaîne de caractères
 *	\note		Si le mode est DGRAM, l'appel nécessite en plus l'adresse IP et le port.
 *	\result		paramètre sockEch modifié pour le mode DGRAM
 */
void envoyer(socket_t *sockEch, generic quoi, pFct serial, ...) {
	buffer_t buff;	// buffer d'envoi
	
	// Serialiser dans buff la requête/réponse à envoyer
	if (serial != NULL) serial(quoi, buff);
	else strcpy(buff , (char *)quoi);
	
	// Envoi : appel de la fonction adéquate selon le mode
	if (sockEch->mode==SOCK_STREAM) envoyerMessSTREAM(sockEch, buff);
	else {
		va_list pArg;
		va_start(pArg, serial);
			envoyerMessDGRAM(sockEch, buff, va_arg(pArg, char *), va_arg(pArg, int));
		va_end(pArg);
		}	
}
/**
 *	\fn			void recevoir(socket_t *sockEch, generic quoi, pFct deSerial)
 *	\brief		Réception d'une requête/réponse sur une socket
 *	\param 		sockEch : socket d'échange à utiliser pour la réception
 *	\param 		quoi : requête/réponse reçue après dé-serialisation du buffer de réception
 *	\param 		deSerial : pointeur sur la fonction de dé-serialisation d'une requête/réponse
 *	\note		si le paramètre deSerial vaut NULL alors quoi est une chaîne de caractères
 *	\result		paramètre quoi modifié avec le requête/réponse reçue
 *				paramètre sockEch modifié pour le mode DGRAM
 */
void recevoir(socket_t *sockEch, generic quoi, pFct deSerial) {
	buffer_t buff;	// buffer de réception
	
	// Réception : appel de la fonction adéquate selon le mode
	if (sockEch->mode==SOCK_STREAM) recevoirMessSTREAM(sockEch, buff, MAX_BUFFER);
	else recevoirMessDGRAM(sockEch, buff, MAX_BUFFER);
	// Dé-serialiser la requête/réponse
	if (deSerial != NULL) deSerial(buff, quoi);
	else strcpy((char * ) quoi, buff);
}


//////////////////////////////
// Mes fonctions 			//
//////////////////////////////

void envoyerMessDGRAM (socket_t *sockEch, char *msg, char *adrDest, short portDest) {
	int nbOctets;
	struct sockaddr_in dest;

	dest.sin_family = AF_INET;
	dest.sin_port = htons(portDest);
	dest.sin_addr.s_addr = inet_addr(adrDest);
	memset(dest.sin_zero, 0, 8);
	CHECK(nbOctets = sendto(sockEch->fd, msg, strlen(msg) + 1, SEND_FLAGS, (struct sockaddr *)&dest, sizeof(dest)), "sendTo DGRAM");
}


void recevoirMessDGRAM (socket_t *sockEch, char *msg, int msgSize) {
	int nbOctets;
	struct sockaddr_in svc;
	socklen_t svcLen = sizeof(svc);
	CHECK(	nbOctets = recvfrom(sockEch->fd, msg, msgSize - 1, RECV_FLAGS,(struct sockaddr *)&svc, &svcLen), "recvfrom DGRAM");

	sockEch->addrDst = svc;
	
}


void envoyerMessSTREAM (const socket_t *sockEch, char *msg) {
	int nbOctets;
	int longueur = strlen(msg) + 1;
	int totalEnvoye = 0;
	while (totalEnvoye < longueur) {
		CHECK(nbOctets = send(sockEch->fd, msg + totalEnvoye, longueur - totalEnvoye, SEND_FLAGS), "send STREAM");
		totalEnvoye += nbOctets;
	}
}
void recevoirMessSTREAM (const socket_t * sockEch, char *msg, int msgSize) {
    CHECK(read(sockEch->fd, msg, msgSize), "Can't receive");
}
