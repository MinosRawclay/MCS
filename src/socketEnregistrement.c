/**
 * @file socketEnregistrement.c
 * @brief Implementation of the registration socket server with multi-threading
 * @details This file implements a multi-threaded server that handles client registration
 *          and game management. Each client connection is handled in a separate detached
 *          thread, allowing multiple simultaneous connections. The server listens for
 *          incoming connections and spawns a new thread for each client to process
 *          registration requests and responses.
 * @author Breviere Alexandre
 * @date 04/02/2026
 */
#include "../include/socketEnregistrement.h"

// ==================== THREAD FUNCTIONS ====================

/**
 * @brief Thread logic function to handle individual client connections
 * @details This function runs in a separate thread for each connected client.
 *          It continuously reads messages from the client, processes registration
 *          requests, and sends appropriate responses. The thread terminates when
 *          the client disconnects or when read() returns 0 or negative value.
 *          
 *          The function performs the following operations in a loop:
 *          - Reads incoming data from the client socket
 *          - Receives and parses the client's response
 *          - Processes the registration request via traiterRegister()
 *          - Sends back the generated request to the client
 *          - Logs all requests and responses to stderr
 * 
 * @param[in] arg Pointer to the socket_t structure for client communication
 * @return NULL when the thread terminates
 * 
 * @note The socket is closed when the client disconnects
 * @warning This function is designed to be called by pthread_create()
 */
void * threadLogic(void * arg){
    printf("Réussi thread");
    char msg[1];
    socket_t * sd = (socket_t * ) arg;
    reponse_t rep;
    requete_t req;
    
    // sockEcoute is an external variable
    
    while(recv(sd->fd, msg, 1, MSG_PEEK) > 0){
        //aquisition req clt
        //mise en forme
        //si fin dialogue alors break
        
        recevoir(sd, (generic)&rep, (pFct)str2rep);
        fprintf(stderr, REQ_STR_OUT"\n", rep.idRep, rep.verbRep, rep.optRep);
        
        req = traiterRegister(&rep, sd);
        printf("traitement termine, envoie en cours...\n");

        envoyer(sd, (generic)&req, (pFct)req2str);
        fprintf(stderr, REQ_STR_OUT"\n", req.idReq, req.verbReq, req.optReq);
    }
    
    CHECK(close(sd->fd), "-- PB close() --");
    free(sd);
    return NULL;
}

// ==================== MAIN FUNCTION ====================

/**
 * @brief Main entry point of the registration server
 * @details This function initializes and runs a multi-threaded server that handles
 *          client registrations for the Belote game. It creates a listening socket
 *          and enters an infinite loop that:
 *          - Accepts incoming client connections via accepterClt()
 *          - Initializes thread attributes for detached threads
 *          - Creates a new detached thread for each client using threadLogic()
 *          
 *          Each client connection is handled independently in its own thread,
 *          allowing multiple simultaneous client registrations and game management.
 * 
 * @return Exit status code (never reached due to infinite loop)
 * 
 * @note The server listens on IP_HOST and PORT (defined in header)
 * @note All threads are created in detached state (PTHREAD_CREATE_DETACHED)
 * @warning The infinite loop means the server runs continuously until terminated
 * @warning Each thread must properly handle its own socket cleanup
 */
int main(){
    socket_t sa;
    socket_t sockEcoute;
    pthread_t th;
    pthread_attr_t attr;
    
    // Create listening socket on specified IP and port
    sockEcoute = creerSocketEcoute(IP_HOST, PORT);
    
    // Infinite loop to handle client connections
    while(1){
        // Accept incoming client connection
        sa = accepterClt(sockEcoute);
    
        // On alloue un espace mémoire unique pour ce client
        socket_t *sa_thread = malloc(sizeof(socket_t)); 
        *sa_thread = sa; // On copie les données
        // Initialize thread attributes
        CHECK_ZERO(pthread_attr_init(&attr), "T ERROR main 1");
        
        // Set thread to detached state (automatic cleanup)
        CHECK_ZERO(pthread_attr_setdetachstate(&attr,
                PTHREAD_CREATE_DETACHED),
                "T ERROR main 2");
        
        // Create new thread to handle client
        CHECK_ZERO(pthread_create(&th, &attr, 
                (pFctThread) threadLogic,
                (void*) sa_thread), 
                "T ERROR main 3");
    


        
        printf("Réussi main");
    }
}