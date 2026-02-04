#ifndef LIBDIAL_H
#define LIBDIAL_H


// fichier : libDial.h
#include "libRepReq.h"
// include des fichier

// prototypes des fonctions
void DialSrv(socket_t sockEcoute);
void DialC(socket_t sd);
void DialRegister(socket_t sd);

#endif /* LIBDIAL_H */