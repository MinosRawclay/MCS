/**
 * @file libDial.c
 * @brief Implementation of client-server dialogue functions
 * @details This file implements dialogue functions to manage communications
 *          between clients and servers via sockets for the Belote card game.
 * @author [Your name]
 * @date 04/02/2026
 */
#include "../include/libDial.h"

/**
 * @brief Client-side dialogue function
 * @details This function manages continuous dialogue with a server. It receives requests,
 *          processes them according to their identifier, then sends a standard response.
 *          The function performs an infinite loop that:
 *          - Receives a request from the server
 *          - Processes the request according to its identifier (101, 102, 103)
 *          - Sends a standard response (id 201)
 * 
 * @param[in] sd Dialogue socket with the server
 * 
 * @note The loop is infinite, the exit is never reached in the current code.
 *       Supported requests are:
 *       - 101: Processed by traiterReq101()
 *       - 102: Processed by traiterReq102()
 *       - 103: Processed by traiterReq103()
 *       - Others: Error message display
 */
void DialC(socket_t sd) {
    requete_t req; 
    reponse_t rep;
    
    while(1) {
        recevoir(&sd, (generic)&req, (pFct)str2req);
        
        switch (req.idReq) {
            case 101:
                traiterReq101(&req);
                break;
            case 102:
                traiterReq102(&req);
                break;
            case 103:
                traiterReq103(&req);
                break;
            default:
                printf("Requête inconnue : %d\n", req.idReq);
                break;
        }
        
        rep.idRep = 201;
        strcpy(rep.verbRep, "Verb");
        strcpy(rep.optRep, "Opt");
        envoyer(&sd, (generic)&rep, (pFct)rep2str);
    }
    
    printf("Fermeture du dialogue.\n");
    CHECK(shutdown(sd.fd, SHUT_WR), "-- PB shutdown() --");
}

/**
 * @brief Server-side dialogue function
 * @details This function manages dialogue with clients on the server side. It accepts
 *          incoming connections, sends standard requests and processes received responses.
 *          The function performs an infinite loop that:
 *          - Accepts a client connection via accepterClt()
 *          - Sends a standard request (id 101)
 *          - Receives the response from the client
 *          - Processes the response via traiterRep()
 * 
 * @param[in] sockEcoute Listening socket to accept client connections
 * 
 * @note The loop is infinite, the closing code is never reached.
 * @warning The dialogue socket (sa) is only closed after exiting the loop,
 *          which never happens in the current implementation.
 */
void DialSrv(socket_t sockEcoute){
    socket_t sa;        // dialogue socket with a client
    reponse_t rep;
    requete_t req;
    
    // sockEcoute is an external variable
    while(1){
        req.idReq = 101;
        strcpy(req.verbReq, "Verb");
        strcpy(req.optReq, "opt");
        
        sa = accepterClt(sockEcoute);
        //aquisition req clt
        //mise en forme
        //si fin dialogue alors break
        
        envoyer(&sa, (generic)&req, (pFct)req2str);
        recevoir(&sa, (generic)&rep, (pFct)str2rep);
        traiterRep(&rep);
    }
    
    CHECK(close(sa.fd), "-- PB close() --");
}

/**
 * @brief Dialogue function for client registration
 * @details This function manages the registration dialogue with a server.
 *          It repeatedly sends registration requests and processes
 *          responses received from the server.
 *          The function performs an infinite loop that:
 *          - Sends a registration request (id 201)
 *          - Receives the response from the server
 *          - Processes the response via traiterRep()
 * 
 * @param[in] sd Dialogue socket with the server
 * 
 * @note The loop is infinite, the exit and socket closure
 *       are never reached in the current code.
 */
void DialRegister(socket_t sd) {
    requete_t req; 
    reponse_t rep;
    
    while(1) {
        req.idReq = 201;
        strcpy(req.verbReq, "Verb");
        strcpy(req.optReq, "Opt");
        
        envoyer(&sd, (generic)&req, (pFct)req2str);
        recevoir(&sd, (generic)&rep, (pFct)str2rep);
        traiterRep(&rep);
    }
    
    printf("Fermeture du dialogue.\n");
    CHECK(shutdown(sd.fd, SHUT_WR), "-- PB shutdown() --");
}