/**
 * @file libRepReq.c
 * @brief Implementation of request/response conversion and processing functions
 * @details This file implements conversion functions between structures and strings
 *          for requests and responses in the communication protocol,
 *          as well as processing functions for different message types.
 *          It also handles specific processing of registration requests
 *          and multiplayer game management.
 * @author [Your name]
 * @date 04/02/2026
 */
#include "../include/libRepReq.h"

// ==================== REQUEST/RESPONSE CONVERSION ====================

/**
 * @brief Converts a request structure to a string
 * @details Uses sprintf to format the request according to REQ_STR_OUT format.
 *          In DEBUG mode, displays the converted request on stderr.
 * 
 * @param[in] req Pointer to the request structure to convert
 * @param[out] str String buffer to store the result
 * 
 * @note Output format defined by REQ_STR_OUT macro
 */
void req2str(const requete_t * req, char* str){
    //CHECK
    sprintf(str, REQ_STR_OUT, req->idReq, req->verbReq, req->optReq);
    #ifdef DEBUG
        fprintf(stderr, "requete recu");
        fprintf(stderr, REQ_STR_OUT"\n", req->idReq, req->verbReq, req->optReq);
    #endif
}

/**
 * @brief Converts a string to a request structure
 * @details Uses sscanf to parse the string according to REQ_STR_IN format.
 *          In DEBUG mode, displays the parsed request on stderr.
 * 
 * @param[in] str String to parse
 * @param[out] req Pointer to the request structure to fill
 * 
 * @note Input format defined by REQ_STR_IN macro
 */
void str2req(const char* str, requete_t  * req){
    //CHECK
    sscanf(str, REQ_STR_IN, &req->idReq, req->verbReq, req->optReq);
    #ifdef DEBUG
        fprintf(stderr, "requete recu");
        fprintf(stderr, REQ_STR_OUT"\n", req->idReq, req->verbReq, req->optReq);
    #endif
}

/**
 * @brief Converts a response structure to a string
 * @details Uses sprintf to format the response according to REP_STR_OUT format.
 *          In DEBUG mode, displays the converted response on stderr.
 * 
 * @param[in] rep Pointer to the response structure to convert
 * @param[out] str String buffer to store the result
 * 
 * @note Output format defined by REP_STR_OUT macro
 */
void rep2str(const reponse_t * rep, char* str){
    //CHECK
    sprintf(str, REP_STR_OUT, rep->idRep, rep->verbRep, rep->optRep);
    #ifdef DEBUG
        fprintf(stderr, "reponse recu");
        fprintf(stderr, REQ_STR_OUT"\n", rep->idRep, rep->verbRep, rep->optRep);
    #endif
}

/**
 * @brief Converts a string to a response structure
 * @details Uses sscanf to parse the string according to REP_STR_IN format.
 *          In DEBUG mode, displays the parsed response on stderr.
 * 
 * @param[in] str String to parse
 * @param[out] rep Pointer to the response structure to fill
 * 
 * @note Input format defined by REP_STR_IN macro
 */
void str2rep(const char* str, reponse_t  * rep){
    //CHECK
    sscanf(str, REP_STR_IN, &rep->idRep, rep->verbRep, rep->optRep);
    #ifdef DEBUG
        fprintf(stderr, "reponse recu");
        fprintf(stderr, REQ_STR_OUT"\n", rep->idRep, rep->verbRep, rep->optRep);
    #endif
}

// ==================== REQUEST PROCESSING ====================

/**
 * @brief Processes a request of type 101
 * @details Displays request details on stderr according to REQ_STR_OUT format.
 * 
 * @param[in] req Pointer to the request to process
 */
void traiterReq101(requete_t * req){
    fprintf(stderr, REQ_STR_OUT"\n", req->idReq, req->verbReq, req->optReq);
}

/**
 * @brief Processes a request of type 102
 * @details Displays request details on stderr according to REQ_STR_OUT format.
 * 
 * @param[in] req Pointer to the request to process
 */
void traiterReq102(requete_t * req){
    fprintf(stderr, REQ_STR_OUT"\n", req->idReq, req->verbReq, req->optReq);
}

/**
 * @brief Processes a request of type 103
 * @details Displays request details on stderr according to REQ_STR_OUT format.
 * 
 * @param[in] req Pointer to the request to process
 */
void traiterReq103(requete_t * req){
    fprintf(stderr, REQ_STR_OUT"\n", req->idReq, req->verbReq, req->optReq);
}

/**
 * @brief Processes a generic response
 * @details Displays response details on stderr according to REQ_STR_OUT format.
 * 
 * @param[in] rep Pointer to the response to process
 */
void traiterRep(reponse_t * rep){
    fprintf(stderr, REQ_STR_OUT"\n", rep->idRep, rep->verbRep, rep->optRep);
}

// ==================== REGISTRATION PROCESSING ====================

/**
 * @brief Processes registration and game management requests
 * @details This function analyzes the received registration response and generates
 *          the appropriate request according to the response code. It handles:
 *          - 301: Check user existence
 *          - 302: Disconnect a user
 *          - 303: Create a new game
 *          - 304: Join a specific game
 *          - 305: Join a random game
 *          - 306: List available games
 *          - 307: Get game connection information
 * 
 * @param[in] rep Pointer to the received response to process
 * @param[in] sDial Pointer to the client's dialogue socket
 * @return Generated request in response to processing
 * 
 * @note Returned request codes are:
 *       - 2: Game full
 *       - 3: User not found
 *       - 401: Operation successful
 *       - 402: Error or unknown request
 *       - 404: List of available games
 *       - 405: Connection information (IP|port)
 */
requete_t traiterRegister(reponse_t * rep, socket_t * sDial){
    int index;
    requete_t req;
    strcpy(req.optReq, "");
    switch(rep->idRep){
        case 301:
			strcpy(req.verbReq, "connexion");
            if (identifierUser(sDial, rep) == -1) {
                req.idReq = 3; // User not found
            } else {
                req.idReq = 401; // Success
            }
            break;
            
        case 302:
			strcpy(req.verbReq, "deconnexion");

            // Disconnect a user
            deconnecterUser(userFromSocket(sDial));
            break;
            
        case 303:
			strcpy(req.verbReq, "new game");
            // Create a new game
            creerPartieBDD(sDial);
            req.idReq = 401;
            break;
            
        case 304:
			strcpy(req.verbReq, "join spe game");
            // Join a specific game
            if(isFull(trouverUser(rep->optRep))){ 
                req.idReq = 2;
            }
            else{
                modifierDest(userFromSocket(sDial), rep->optRep);
                req.idReq = 401;
            }
            break;
            
        case 305:
			strcpy(req.verbReq, "random game");
            // Join a random available game
			req.idReq = 402;

            for(int i = 0; i < MAX_USERS; i++){
                if(!isFull(i)){
                    modifierDest(userFromSocket(sDial), nameUser(i));
                    req.idReq = 401;
                    strcpy(req.optReq, "join aléatoire");
					break;
                }
				
            }
            break;
            
        case 306:
			strcpy(req.verbReq, "list games");
            // List all available games
            req.idReq = 403;
            req.optReq[0] = '\0';
            for(int i = 0; i < MAX_USERS; i++){
                if(!isFull(i)){
                    char *nom = nameUser(i);
                    
                    if (strlen(req.optReq) > 0) {
                        strncat(req.optReq, "|", sizeof(req.optReq) - strlen(req.optReq) - 1);
                    }
                    
                    strncat(req.optReq, nom, sizeof(req.optReq) - strlen(req.optReq) - 1);
                }
            }
            break;
            
        case 307:
			strcpy(req.verbReq, "get game IP PORT");
            // Get game connection information (IP and port)
            socket_t * sa = socketUser(trouverUser(rep->optRep));
            char buffer[64]; 
    
            char *ip_text = inet_ntoa(sa->addrDst.sin_addr);
    
            int port = ntohs(sa->addrDst.sin_port);
            
            snprintf(buffer, sizeof(buffer), "%s|%d", ip_text, port);
            req.idReq = 405;
            strcpy(req.optReq, buffer);
            break;
            
        default:
            // Unrecognized request
            req.idReq = 402;
            strcpy(req.verbReq, "Demande inconnue");
            break;
    }
    
    return req;
}