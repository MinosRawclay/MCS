/**
 *	\file		session.c
 *	\brief		Fonctions de la couche session
 *	\author		Alexandre BREVIERE
 *	\date		6 janvier 2026
 *	\version	1.0
 */
 #include "../include/session.h"


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
 



void adr2struct(struct sockaddr_in * addr, char* adrIP, short port){
    if(adrIP == NULL || strlen(adrIP) == 0 || strcmp(adrIP, "0.0.0.0") == 0) {
        addr->sin_addr.s_addr = INADDR_ANY;
    } else {
        addr->sin_addr.s_addr = inet_addr(adrIP);
    }

    addr->sin_family = AF_INET;
    addr->sin_port = htons (port);
    addr->sin_addr.s_addr = inet_addr(adrIP);
    memset(&addr->sin_zero, 0, 8);

}


// STREAM OU DGRAM
socket_t creerSocket(int mode){
    if(mode != SOCK_STREAM && mode != SOCK_DGRAM) mode = SOCK_STREAM;
    socket_t s;
    s.mode = mode;
    CHECK(s.fd=socket(PF_INET, mode, 0), "Can't create socket");
    return s;
}


/* Fait appel à creerSocket et adr2struct +
 bind et affectation de l'adresse locale */
socket_t creerSocketAddr(int mode, char* adrIp, int port){


    socket_t sock = creerSocket(mode);

    struct sockaddr_in svc;
    adr2struct(&svc, adrIp, port);

    CHECK(bind(sock.fd, (struct sockaddr *) &svc, sizeof svc) , "Can't bind");
    return sock;

}

 // le mode est forcement STREAM appel a creerSocketAddr + listen()
socket_t creerSocketEcoute(char* adrIP, short port){

    socket_t sock = creerSocketAddr(SOCK_STREAM, adrIP, port);
    CHECK(listen(sock.fd, 5) , "Can't calibrate");
    return sock;

}


socket_t accepterClt (const socket_t sockEcoute){
    socket_t sock;
    struct sockaddr_in clt;
    socklen_t cltLen = sizeof(clt); // Initialisation importante
    
    sock.mode = sockEcoute.mode;
    
    // Correction ici : accept remplit la structure clt
    CHECK(sock.fd = accept(sockEcoute.fd, (struct sockaddr *)&clt, &cltLen), "Can't accept");
    
    // On stocke l'adresse du client dans la socket de session
    sock.addrDst = clt; 
    return sock;
}




socket_t connecterClt2Srv (char *adrIP, short port){
    socket_t sockDest;
    struct sockaddr_in svc;
	    sockDest = creerSocket(SOCK_STREAM); 

    adr2struct(&svc, adrIP, port);

    CHECK(connect(sockDest.fd, (struct sockaddr *)&svc, sizeof svc) , "Can't connect");
    return sockDest;

}
