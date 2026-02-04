/**
 * @file socketEnregistrement.h
 * @brief Declarations for the registration socket server with multi-threading
 * @details This header file provides the necessary declarations, macros, and includes
 *          for implementing a multi-threaded registration server for the Belote game.
 *          It defines thread function pointer types, error checking macros, and server
 *          configuration constants (IP address and port). The server uses POSIX threads
 *          to handle multiple simultaneous client connections independently.
 * @author [Your name]
 * @date 04/02/2026
 */

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

// ==================== TYPE DEFINITIONS ====================

/**
 * @typedef pFctThread
 * @brief Function pointer type for thread functions
 * @details Defines the signature for functions that can be passed to pthread_create().
 *          Thread functions must take a void* argument and return a void* result.
 */
typedef void* (*pFctThread) (void*);

// ==================== ERROR CHECKING MACROS ====================

/**
 * @def CHECK_ZERO
 * @brief Macro to check pthread function return values
 * @details Verifies that a pthread function returned 0 (success).
 *          If the status is non-zero (error), prints an error message
 *          to stderr and terminates the program with EXIT_FAILURE.
 * 
 * @param status Return value from a pthread function to check
 * @param msg Error message to display if check fails
 * 
 * @note This macro is specifically designed for pthread functions that return 0 on success
 * @warning Terminates the program on error - use only for critical failures
 * 
 * Example usage:
 * @code
 * CHECK_ZERO(pthread_create(&thread, NULL, thread_func, arg), "Failed to create thread");
 * @endcode
 */
#define CHECK_ZERO(status, msg) \
    if (0 != (status)) { \
        fprintf(stderr, "pthread erreur : %s\n", msg); \
        exit(EXIT_FAILURE); \
    }

// ==================== SERVER CONFIGURATION ====================

/**
 * @def IP_HOST
 * @brief Server listening IP address
 * @details Defines the IP address on which the server will listen for connections.
 *          "0.0.0.0" means the server listens on all available network interfaces.
 */
#define IP_HOST  "127.0.0.1"

/**
 * @def PORT
 * @brief Server listening port number
 * @details Defines the port number on which the server will listen for client connections.
 *          Port 50000 is used for the registration and game management server.
 */
#define PORT    50000

// ==================== FUNCTION PROTOTYPES ====================

/**
 * @brief Thread logic function to handle individual client connections
 * @details This function runs in a separate thread for each connected client.
 *          It continuously reads messages from the client, processes registration
 *          requests via traiterRegister(), and sends appropriate responses.
 *          The thread performs the following operations in a loop:
 *          - Reads incoming data from the client socket
 *          - Receives and parses the client's response
 *          - Processes the registration request
 *          - Sends back the generated request to the client
 *          - Logs all requests and responses to stderr
 * 
 * @param[in] arg Pointer to the socket_t structure for client communication
 * @return NULL when the thread terminates
 * 
 * @note The socket is automatically closed when the client disconnects
 * @note This function terminates when read() returns 0 or negative value
 * @warning This function is designed to be called by pthread_create()
 * @see main()
 */
void * threadLogic(void * arg);

/**
 * @brief Main entry point of the registration server
 * @details Initializes and runs a multi-threaded server that handles client registrations.
 *          The server creates a listening socket on IP_HOST:PORT and enters an infinite
 *          loop that:
 *          - Accepts incoming client connections via accepterClt()
 *          - Initializes thread attributes for detached threads
 *          - Creates a new detached thread for each client using threadLogic()
 *          
 *          Each client connection is handled independently in its own thread,
 *          allowing multiple simultaneous client registrations and game management.
 * 
 * @return Exit status code (never reached due to infinite loop)
 * 
 * @note The server listens on IP_HOST (0.0.0.0) and PORT (50000)
 * @note All threads are created in detached state (PTHREAD_CREATE_DETACHED)
 * @warning The infinite loop means the server runs continuously until terminated
 * @warning Each thread must properly handle its own socket cleanup
 * @see threadLogic()
 */
int main(void);

#endif /* SOCKET_ENREGISTREMENT_H */