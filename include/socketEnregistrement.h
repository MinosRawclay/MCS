#ifndef SOCKET_ENREGISTREMENT_H
#define SOCKET_ENREGISTREMENT_H

#include "libRepReq.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>

typedef void* (*pFctThread) (void*);

#define CHECK_ZERO(status, msg) \
    if (0 != (status)) { \
        fprintf(stderr, "pthread erreur : %s\n", msg); \
        exit(EXIT_FAILURE); \
    }

#define IP_HOST  "0.0.0.0"

#define PORT    50000

#endif