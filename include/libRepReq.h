/**
 * @file libRepReq.h
 * @brief Declarations of request/response management structures and functions
 * @details This header file defines data structures for requests and responses
 *          in the communication protocol, as well as formatting macros for conversion
 *          between structures and strings. It also declares the complete enumeration
 *          of request, response and communication codes for the Belote card game.
 * @author Breviere Alexandre
 * @date 04/02/2026
 */

#ifndef LIB_REP_REQ_H
#define LIB_REP_REQ_H

#include "data.h"
#include "users.h"

// ==================== FORMATTING MACROS ====================

/**
 * @def REQ_STR_OUT
 * @brief Output format to convert a request to a string
 * @details Format: "idReq:verbReq:optReq"
 */
#define REQ_STR_OUT "%i:%s:%s" 

/**
 * @def REQ_STR_IN
 * @brief Input format to parse a string into a request
 * @details Captures: idReq (integer), verbReq (up to ':'), optReq (up to newline)
 */
#define REQ_STR_IN "%i:%[^:]:%[^\n]" 

/**
 * @def REP_STR_OUT
 * @brief Output format to convert a response to a string
 * @details Format: "idRep:verbRep:optRep"
 */
#define REP_STR_OUT "%i:%s:%s"

/**
 * @def REP_STR_IN
 * @brief Input format to parse a string into a response
 * @details Captures: idRep (integer), verbRep (up to ':'), optRep (up to newline)
 */
#define REP_STR_IN "%i:%[^:]:%[^\n]"

// ==================== STRUCTURES ====================

/**
 * @struct requete
 * @brief Structure representing a protocol request
 * @details Contains a numeric identifier, a verb and options
 */
typedef struct requete{
    short idReq;           ///< Request identifier (see enum requeteList)
    char verbReq[20];      ///< Request verb (textual command)
    char optReq[100];      ///< Request options/parameters
} requete_t;

/**
 * @struct reponse
 * @brief Structure representing a protocol response
 * @details Contains a numeric identifier, a verb and options
 */
typedef struct reponse{
    short idRep;           ///< Response identifier (see enum requeteList)
    char verbRep[20];      ///< Response verb
    char optRep[100];      ///< Response options/data
} reponse_t;

// ==================== CODE ENUMERATION ====================

/**
 * @enum requeteList
 * @brief Enumeration of all request, response and communication codes
 * @details Organizes codes by categories:
 *          - Error codes (1-5)
 *          - Game requests (100-123)
 *          - Game responses (200-223)
 *          - Server communications (301-305)
 *          - Communication responses (401-403)
 */
typedef enum requeteList {
    // ========== ERROR CODES ==========
    ERR_MAUVAISE_CONNEXION = 1,    ///< Server connection error
    ERR_PARTIE_PLEINE = 2,         ///< Game is full (4 players)
    ERR_SERVEUR_PLEIN = 3,         ///< Server cannot accept more players
    ERR_MAUVAIS_CODE = 4,          ///< Invalid request/response code
    ERR_SERVEUR_NON_DISPO = 5,     ///< Server unavailable
    
    // ========== GAME REQUESTS ==========
    REQ_LANCER_PARTIE = 100,       ///< Request to start the game
    
    // Display request functions
    REQ_ENVOYER_DECK = 110,        ///< Request to send the deck (player's hand)
    REQ_ENVOYER_PLI = 111,         ///< Request to send the current trick
    REQ_ENVOYER_SCORE = 112,       ///< Request to send scores
    
    // Game functions
    REQ_JOUER = 120,               ///< Request player to play a card
    REQ_CARTE_LEGALE = 121,        ///< Notification that played card is legal
    REQ_CHOIX_ATOUT = 122,         ///< Request to take trump (round 1)
    REQ_CHOIX_ATOUT_COULEUR = 123, ///< Request to choose trump suit (round 2)
    
    // ========== GAME RESPONSES ==========
    REP_ACK_LANCER_PARTIE = 200,   ///< Acknowledgment of game start
    REP_ACK_ENVOYER_DECK = 210,    ///< Acknowledgment of deck received
    REP_ACK_ENVOYER_PLI = 211,     ///< Acknowledgment of trick received
    REP_ACK_ENVOYER_SCORE = 212,   ///< Acknowledgment of scores received
    REP_ENVOI_CARTE = 220,         ///< Send card played by player
    REP_ACK_CARTE_LEGALE = 221,    ///< Acknowledgment of legal card
    REP_CHOIX_ATOUT = 222,         ///< Response to trump choice (1=yes, 0=no)
    REP_CHOIX_ATOUT_COULEUR = 223, ///< Response with chosen suit (format: "1|H")
    
    // ========== SERVER COMMUNICATIONS ==========
    COM_CONNECTER = 301,           ///< Request to connect to server
    COM_DECONNECTER = 302,         ///< Request to disconnect from server
    COM_CREER_PARTIE = 303,        ///< Request to create a new game
    COM_REJOINDRE_PARTIE = 304,    ///< Request to join a specific game
    COM_REJOINDRE_ALEA = 305,      ///< Request to join a random game
    
    // ========== COMMUNICATION RESPONSES ==========
    REP_ACK_CONNECTER = 401,       ///< Acknowledgment of successful connection
    REP_LIST_USERS = 403,          ///< List of available users/games
} requeteList_t;

// ==================== FUNCTION PROTOTYPES ====================

// Conversion functions

/**
 * @brief Converts a request structure to a string
 * @param[in] req Pointer to the request structure to convert
 * @param[out] str String buffer to store the result
 * @see str2req()
 */
void req2str(const requete_t * req, char * str);

/**
 * @brief Converts a string to a request structure
 * @param[in] str String to parse
 * @param[out] req Pointer to the request structure to fill
 * @see req2str()
 */
void str2req(const char * str, requete_t * req);

/**
 * @brief Converts a response structure to a string
 * @param[in] rep Pointer to the response structure to convert
 * @param[out] str String buffer to store the result
 * @see str2rep()
 */
void rep2str(const reponse_t * rep, char * str);

/**
 * @brief Converts a string to a response structure
 * @param[in] str String to parse
 * @param[out] rep Pointer to the response structure to fill
 * @see rep2str()
 */
void str2rep(const char * str, reponse_t * rep);

// Processing functions

/**
 * @brief Processes a request of type 101
 * @param[in] req Pointer to the request to process
 */
void traiterReq101(requete_t * req);

/**
 * @brief Processes a request of type 102
 * @param[in] req Pointer to the request to process
 */
void traiterReq102(requete_t * req);

/**
 * @brief Processes a request of type 103
 * @param[in] req Pointer to the request to process
 */
void traiterReq103(requete_t * req);

/**
 * @brief Processes a generic response
 * @param[in] rep Pointer to the response to process
 */
void traiterRep(reponse_t * rep);

// Dialogue functions

/**
 * @brief Server-side dialogue function
 * @param[in] sockEcoute Listening socket to accept client connections
 * @see DialC()
 */
void DialSrv(socket_t sockEcoute);

/**
 * @brief Client-side dialogue function
 * @param[in] sd Dialogue socket with the server
 * @see DialSrv()
 */
void DialC(socket_t sd);

/**
 * @brief Processes registration and game management requests
 * @param[in] rep Pointer to the received response to process
 * @param[in] sDial Pointer to the client's dialogue socket
 * @return Generated request in response to processing
 */
requete_t traiterRegister(reponse_t * rep, socket_t * sDial);

#endif