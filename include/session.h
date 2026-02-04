/**
 * @file session.h
 * @brief Session layer specification for socket management
 * @details This header file provides the specification for the session layer,
 *          defining data structures and function prototypes for socket operations.
 *          It supports both TCP (SOCK_STREAM) and UDP (SOCK_DGRAM) socket modes,
 *          providing a high-level interface for creating, binding, listening,
 *          accepting, and connecting sockets in client-server applications.
 * @author Samir El Khattabi
 * @date March 3, 2023
 * @version 1.0
 */

#ifndef SESSION_H
#define SESSION_H

// ==================== SPECIFIC INCLUDES ====================

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

// ==================== DATA STRUCTURES ====================

/**
 * @struct socket
 * @brief Socket data structure definition
 * @details This structure encapsulates all information related to a socket,
 *          including its file descriptor, connection mode, and both local
 *          and remote address information. It provides a complete representation
 *          of a socket for both client and server applications.
 * 
 * @note This type is composed of the socket file descriptor, mode (connected/non-connected),
 *       and application addresses (local/remote)
 */
struct socket {
    int fd;                         ///< File descriptor of the created socket
    int mode;                       ///< Connection mode: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
    struct sockaddr_in addrLoc;     ///< Local address of the socket
    struct sockaddr_in addrDst;     ///< Remote/destination address of the socket
};

/**
 * @typedef socket_t
 * @brief Socket data type definition
 * @details Convenient type alias for struct socket
 */
typedef struct socket socket_t;

// ==================== FUNCTION PROTOTYPES ====================

/**
 * @brief Converts human-readable address format to BSD socket structure
 * @details Transforms an IP address string and port number into a sockaddr_in structure
 *          suitable for use with Berkeley sockets API. Handles address family configuration,
 *          byte order conversion, and structure initialization.
 * 
 * @param[out] addr BSD socket addressing structure to fill (INET domain)
 * @param[in] adrIP IP address string of the socket (e.g., "192.168.1.1" or "0.0.0.0")
 * @param[in] port Port number of the socket
 * 
 * @note The domain depends on the chosen mode (TCP/UDP)
 * @note The addr parameter is modified by this function
 * @see creerSocketAdr()
 */
void adr2struct(struct sockaddr_in *addr, char *adrIP, short port);

/**
 * @brief Creates a socket of type DGRAM or STREAM
 * @details Creates a socket using the Berkeley sockets API with the specified mode.
 *          Supports both connection-oriented (SOCK_STREAM/TCP) and connectionless
 *          (SOCK_DGRAM/UDP) communication modes.
 * 
 * @param[in] mode Connection mode: SOCK_STREAM (connected/TCP) or SOCK_DGRAM (connectionless/UDP)
 * @return Socket created according to the chosen mode
 * 
 * @note If an invalid mode is provided, defaults to SOCK_STREAM
 * @see creerSocketAdr()
 */
socket_t creerSocket(int mode);

/**
 * @brief Creates a socket of type DGRAM/STREAM with specified addressing
 * @details Creates a socket with the specified mode and binds it to the provided
 *          IP address and port. This function combines socket creation and address
 *          binding in a single operation.
 * 
 * @param[in] mode Connection mode: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
 * @param[in] adrIP IP address to bind the socket to
 * @param[in] port Port number to bind the socket to
 * @return Socket created in the chosen domain with the provided addressing
 * 
 * @see creerSocket(), adr2struct()
 */
socket_t creerSocketAdr(int mode, char *adrIP, short port);

/**
 * @brief Creates a listening socket with the provided addressing
 * @details Creates a TCP socket, binds it to the specified address and port,
 *          and puts it in listening mode to accept incoming client connections.
 *          This function is specifically designed for server applications.
 * 
 * @param[in] adrIP IP address of the server to put in listening mode
 * @param[in] port TCP port of the server to put in listening mode
 * @return Socket created with the provided addressing and in listening state
 * 
 * @note The mode is necessarily SOCK_STREAM (TCP)
 * @note Listen backlog is configured for pending connections
 * @see accepterClt()
 */
socket_t creerSocketEcoute(char *adrIP, short port);

/**
 * @brief Accepts a client connection request
 * @details Blocks waiting for an incoming connection on the listening socket,
 *          then accepts it and returns a new socket for communication with
 *          the connected client. The client's address information is stored
 *          in the returned socket structure.
 * 
 * @param[in] sockEcoute Listening socket to receive the connection request
 * @return Dialogue socket connected by the server with a client
 * 
 * @note This function blocks until a client connection request arrives
 * @note The returned socket is ready for bidirectional communication
 * @see creerSocketEcoute()
 */
socket_t accepterClt(const socket_t sockEcoute);

/**
 * @brief Creates a calling socket and connects to a server
 * @details Creates a TCP socket and initiates a connection to the server
 *          whose address is provided as parameter. This function is used
 *          by client applications to establish connections to servers.
 * 
 * @param[in] adrIP IP address of the server to connect to
 * @param[in] port TCP port of the server to connect to
 * @return Socket connected to the server provided as parameter
 * 
 * @note Always creates a SOCK_STREAM (TCP) socket
 * @note This function blocks until connection is established or fails
 * @see creerSocket()
 */
socket_t connecterClt2Srv(char *adrIP, short port);

#endif /* SESSION_H */