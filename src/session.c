/**
 * @file session.c
 * @brief Session layer functions for socket management
 * @details This file implements the session layer functions for creating, binding,
 *          listening, accepting, and connecting sockets. It provides a high-level
 *          interface for both TCP (STREAM) and UDP (DGRAM) socket operations,
 *          abstracting the low-level Berkeley sockets API into simpler function calls.
 * @author Alexandre BREVIERE
 * @date January 6, 2026
 * @version 1.0
 */
#include "../include/session.h"

// ==================== MACRO DEFINITIONS ====================

/**
 * @def CHECK(sts, msg)
 * @brief Macro function to check if a system call returned an error
 * @details Verifies that sts is equal to -1 (error case: sts==-1).
 *          In case of error, displays the appropriate message and terminates execution.
 * 
 * @param sts Status value to check (typically return value from system call)
 * @param msg Error message to display if check fails
 * 
 * @warning Terminates the program with exit(-1) on error
 */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}

/**
 * @def PAUSE(msg)
 * @brief Macro function to display a message and wait for keyboard input
 * @details Displays msg and waits for the user to press Enter before continuing.
 * 
 * @param msg Message to display to the user
 */
#define PAUSE(msg)  printf("%s [Appuyez sur entrée pour continuer]", msg); getchar();

// ==================== ADDRESS CONVERSION ====================

/**
 * @brief Converts IP address and port to a sockaddr_in structure
 * @details Fills a sockaddr_in structure with the provided IP address and port number.
 *          Handles special cases:
 *          - If adrIP is NULL, empty, or "0.0.0.0", uses INADDR_ANY (bind to all interfaces)
 *          - Converts port number to network byte order (big-endian)
 *          - Sets address family to AF_INET (IPv4)
 *          - Zeros out the padding field (sin_zero)
 * 
 * @param[out] addr Pointer to sockaddr_in structure to fill
 * @param[in] adrIP IP address string (e.g., "192.168.1.1" or "0.0.0.0" for any)
 * @param[in] port Port number in host byte order
 * 
 * @note Uses inet_addr() for IP address conversion
 * @note Port is converted using htons() to network byte order
 */
void adr2struct(struct sockaddr_in * addr, char* adrIP, short port){
    if(adrIP == NULL || strlen(adrIP) == 0 || strcmp(adrIP, "0.0.0.0") == 0) {
        addr->sin_addr.s_addr = INADDR_ANY;
    } else {
        addr->sin_addr.s_addr = inet_addr(adrIP);
    }
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(adrIP);
    memset(&addr->sin_zero, 0, 8);
}

// ==================== SOCKET CREATION ====================

/**
 * @brief Creates a socket with specified mode
 * @details Creates a socket using the Berkeley sockets API with the specified mode.
 *          Supports both SOCK_STREAM (TCP) and SOCK_DGRAM (UDP) modes.
 *          If an invalid mode is provided, defaults to SOCK_STREAM.
 * 
 * @param[in] mode Socket mode: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
 * @return socket_t structure containing the created socket file descriptor and mode
 * 
 * @note If mode is neither SOCK_STREAM nor SOCK_DGRAM, defaults to SOCK_STREAM
 * @note Uses PF_INET (IPv4) protocol family
 * @warning Terminates program if socket creation fails
 */
socket_t creerSocket(int mode){
    if(mode != SOCK_STREAM && mode != SOCK_DGRAM) mode = SOCK_STREAM;
    socket_t s;
    s.mode = mode;
    CHECK(s.fd=socket(PF_INET, mode, 0), "Can't create socket");
    return s;
}

/**
 * @brief Creates a socket and binds it to a specific address and port
 * @details Creates a socket using creerSocket(), converts the address using adr2struct(),
 *          and binds the socket to the specified IP address and port. This function
 *          is typically used for servers that need to listen on a specific address/port.
 * 
 * @param[in] mode Socket mode: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
 * @param[in] adrIp IP address to bind to (use "0.0.0.0" for all interfaces)
 * @param[in] port Port number to bind to
 * @return socket_t structure containing the bound socket
 * 
 * @note Calls creerSocket() and adr2struct() internally
 * @warning Terminates program if socket creation or binding fails
 * @see creerSocket(), adr2struct()
 */
socket_t creerSocketAddr(int mode, char* adrIp, int port){
    socket_t sock = creerSocket(mode);
    struct sockaddr_in svc;
    adr2struct(&svc, adrIp, port);
    CHECK(bind(sock.fd, (struct sockaddr *) &svc, sizeof svc), "Can't bind");
    return sock;
}

/**
 * @brief Creates a listening socket for accepting client connections
 * @details Creates a TCP socket (SOCK_STREAM), binds it to the specified address and port,
 *          and puts it in listening mode. This function is used by servers to prepare
 *          for accepting incoming client connections. The listen backlog is set to 5.
 * 
 * @param[in] adrIP IP address to bind to (use "0.0.0.0" for all interfaces)
 * @param[in] port Port number to bind to
 * @return socket_t structure containing the listening socket
 * 
 * @note Mode is always SOCK_STREAM (TCP) for listening sockets
 * @note Listen backlog is set to 5 pending connections
 * @warning Terminates program if socket creation, binding, or listen fails
 * @see creerSocketAddr(), accepterClt()
 */
socket_t creerSocketEcoute(char* adrIP, short port){
    socket_t sock = creerSocketAddr(SOCK_STREAM, adrIP, port);
    CHECK(listen(sock.fd, 5), "Can't calibrate");
    return sock;
}

// ==================== CONNECTION HANDLING ====================

/**
 * @brief Accepts an incoming client connection
 * @details Blocks until a client connects to the listening socket, then accepts
 *          the connection and returns a new socket for communication with that client.
 *          The client's address information is stored in the returned socket structure.
 * 
 * @param[in] sockEcoute Listening socket (created by creerSocketEcoute())
 * @return socket_t structure containing the accepted connection socket and client address
 * 
 * @note The returned socket inherits the mode from the listening socket
 * @note Client address is stored in sock.addrDst for later use
 * @note This function blocks until a client connects
 * @warning Terminates program if accept fails
 * @see creerSocketEcoute()
 */
socket_t accepterClt(const socket_t sockEcoute){
    socket_t sock;
    struct sockaddr_in clt;
    socklen_t cltLen = sizeof(clt); // Important initialization
    
    sock.mode = sockEcoute.mode;
    
    // Correction here: accept fills the clt structure
    CHECK(sock.fd = accept(sockEcoute.fd, (struct sockaddr *)&clt, &cltLen), "Can't accept");
    
    // Store client address in the session socket
    sock.addrDst = clt; 
    return sock;
}

/**
 * @brief Connects a client to a server
 * @details Creates a TCP socket (SOCK_STREAM) and connects it to the specified
 *          server address and port. This function is used by clients to establish
 *          a connection to a server.
 * 
 * @param[in] adrIP Server IP address to connect to
 * @param[in] port Server port number to connect to
 * @return socket_t structure containing the connected socket
 * 
 * @note Always creates a SOCK_STREAM (TCP) socket
 * @note This function blocks until connection is established or fails
 * @warning Terminates program if socket creation or connection fails
 * @see creerSocket()
 */
socket_t connecterClt2Srv(char *adrIP, short port){
    socket_t sockDest;
    struct sockaddr_in svc;
    sockDest = creerSocket(SOCK_STREAM); 
    adr2struct(&svc, adrIP, port);
    CHECK(connect(sockDest.fd, (struct sockaddr *)&svc, sizeof svc), "Can't connect");
    return sockDest;
}