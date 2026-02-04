/**
 * @file servApp.h
 * @brief Header file for the application server of the Belote card game
 * @details This header defines the data structures, enumerations, and function prototypes
 *          for implementing the French card game Belote for 4 players in 2 teams.
 * @author Vaniet Quentin
 * @date 04/02/2026
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "libRepReq.h"
#include "moteur.h"

// ==================== FUNCTION PROTOTYPES ===================================

/**
 * @brief Displays the main menu with available game options
 * @details Shows options to create a game, join a user's game, join a random game, or quit
 */
void afficherMenu();

/**
 * @brief Prompts the user to enter their menu choice
 * @return The integer value corresponding to the user's choice
 */
int choisir();

/**
 * @brief Establishes a connection to the registration server (SE)
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param nomUtilisateur Buffer to store the username entered by the user
 * @return 0 on success, -1 on failure
 */
int connexionToSE(socket_t *sdSE, char * nomUtilisateur);

/**
 * @brief Creates a new game session on the registration server
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param nomUtilisateur The username of the player creating the game
 * @return 0 on success, -1 on failure
 */
int creerPartieSe(socket_t *sdSE, char * nomUtilisateur);

/**
 * @brief Requests and displays the list of available games with users
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @return 0 on success, -1 on failure
 */
int demanderAffichagePartiesUtilisateur(socket_t *sdSE);

/**
 * @brief Allows the user to select and join a specific user's game
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param IPUtilisateur Buffer to store the IP address of the selected game host
 * @param portUtilisateur Pointer to store the port number of the selected game host
 * @return 0 on success, -1 on failure (game full or error)
 */
int choisirPartieUtilisateur(socket_t *sdSE, char * IPUtilisateur, int * portUtilisateur);

/**
 * @brief Manages a game as the host server
 * @details Creates a listening socket, waits for 3 players to connect,
 *          initializes the game engine, and handles game execution and cleanup
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param nomUtilisateur The username of the player hosting the game
 */
void gestionPartie(socket_t *sdSE, char *nomUtilisateur);

/**
 * @brief Joins an existing game hosted by another user
 * @details Displays available games and prompts the user to select one to join
 * @param sdSE Pointer to the socket descriptor for the registration server
 */
void rejoindrePartie(socket_t *sdSE);

/**
 * @brief Joins a random available game or creates a new one if none exists
 * @param sdSE Pointer to the socket descriptor for the registration server
 * @param nomUtilisateur The username of the player
 * @return 0 on success, -1 on failure
 */
int rejoindrePartieAleatoire(socket_t *sdSE, char *nomUtilisateur);
