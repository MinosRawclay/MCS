/**
 * @file libDial.h
 * @brief Declarations of client-server dialogue functions
 * @details This header file declares dialogue functions to manage communications
 *          between clients and servers via sockets for the Belote card game.
 *          It provides function prototypes for dialogue operations for:
 *          - The server (accepting and processing connections)
 *          - The client (communication with the server)
 *          - Registration (registration procedure)
 * @author Breviere Alexandre
 * @date 04/02/2026
 */

#ifndef LIBDIAL_H
#define LIBDIAL_H

// fichier : libDial.h
#include "libRepReq.h"

// ==================== FUNCTION PROTOTYPES ====================

/**
 * @brief Server-side dialogue function
 * @details Manages dialogue with clients on the server side by accepting connections,
 *          sending requests and processing responses.
 * @param[in] sockEcoute Listening socket to accept client connections
 * @see DialC(), DialRegister()
 */
void DialSrv(socket_t sockEcoute);

/**
 * @brief Client-side dialogue function
 * @details Manages continuous dialogue with a server by receiving requests,
 *          processing them according to their identifier, then sending a response.
 * @param[in] sd Dialogue socket with the server
 * @see DialSrv(), DialRegister()
 */
void DialC(socket_t sd);

/**
 * @brief Dialogue function for registration
 * @details Manages the registration dialogue with a server by sending
 *          registration requests and processing received responses.
 * @param[in] sd Dialogue socket with the server
 * @see DialC(), DialSrv()
 */
void DialRegister(socket_t sd);

#endif /* LIBDIAL_H */