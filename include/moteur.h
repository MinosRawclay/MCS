/**
 * @file moteur.h
 * @brief Header file for the Belote card game implementation
 * @details This header defines the data structures, enumerations, and function prototypes
 *          for implementing the French card game Belote for 4 players in 2 teams.
 * @author Raphael ALLO
 * @date 02/02/2026
 */

#ifndef MOTEUR_H
#define MOTEUR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "data.h"

// ==================== CONSTANTS =============================================

#define PLAYERS_MAX 4        ///< Maximum number of players (corresponds to Belote rules)
#define NB_CARD_HAND 8       ///< Number of cards per player's hand
#define NB_CARD_DECK 32      ///< Total number of cards in the deck

#define POINT_WIN 1500       ///< Points needed to win the game

#define STR_ROUGE_START printf("\033[31m")     ///< ANSI code to start red text
#define STR_MAGENTA_START printf("\033[35m")   ///< ANSI code to start magenta text
#define STR_VERT_START printf("\033[32m")      ///< ANSI code to start green text
#define STR_BLEU_START printf("\033[34m")      ///< ANSI code to start blue text
#define STR_COLOR_END printf("\033[0m")        ///< ANSI code to reset text color

// ==================== ENUMERATIONS ==========================================

/**
 * @enum card
 * @brief Enumeration of all cards in the deck
 * @details Cards are organized by suit: Hearts (H), Clubs (C), Spades (P), Diamonds (T)
 *          Each suit contains: AS, 7, 8, 9, 10, V (Jack), D (Queen), R (King)
 */
enum card {
    H_AS, H_7, H_8, H_9, H_10, H_V, H_D, H_R,
    C_AS, C_7, C_8, C_9, C_10, C_V, C_D, C_R,
    P_AS, P_7, P_8, P_9, P_10, P_V, P_D, P_R,
    T_AS, T_7, T_8, T_9, T_10, T_V, T_D, T_R,
    NOTHING = -1  ///< Represents no card or empty slot
};

/**
 * @enum colorCard
 * @brief Enumeration of card suits
 */
enum colorCard {
    H,      ///< Hearts (Coeur)
    C,      ///< Clubs (Trèfle)
    P,      ///< Spades (Pique)
    T,      ///< Diamonds (Carreau)
    NONE    ///< No suit
};

/**
 * @enum equipe
 * @brief Enumeration of teams
 */
enum equipe {
    EQUIPE1,    ///< Team 1 (players 0 and 2)
    EQUIPE2     ///< Team 2 (players 1 and 3)
};

/**
 * @enum state
 * @brief Enumeration of player states
 */
enum state {
    FINISH,     ///< Player has finished their turn
    PLAYING,    ///< Player is currently playing
    WAITING     ///< Player is waiting for their turn
};

// ==================== STRUCTURES ============================================

/**
 * @struct player
 * @brief Structure representing a player
 */
typedef struct player {
    int num;                    ///< Player number (0-3)
    enum state s;               ///< Current player state
    int cards[NB_CARD_HAND];    ///< Cards in player's hand
    socket_t* sock;             ///< Socket for network communication (NULL for local player)
} player_t;

/**
 * @typedef players_t
 * @brief Array of pointers to all players in the game
 */
typedef player_t* players_t[PLAYERS_MAX];

/**
 * @struct pileCard
 * @brief Structure representing a pile of cards (deck or team's won cards)
 */
typedef struct pileCard {
    enum card deck[NB_CARD_DECK];   ///< Array of cards in the pile
    int lastcard;                   ///< Index of the last card dealt/added
} pileCard_t;

/**
 * @typedef pli_t
 * @brief Array representing the current trick (4 cards)
 */
typedef enum card pli_t[PLAYERS_MAX];

// ==================== FUNCTION PROTOTYPES ===================================

// -------------------- Communication Functions -------------------------------

/**
 * @brief Adds a new player to the game
 * @param[in,out] players Array of player pointers
 * @param[in,out] nbPlayer Current number of players, incremented if successful
 * @return true if player was added successfully, false if game is full or allocation failed
 */
bool addPlayer(players_t players, int *nbPlayer);

/**
 * @brief Sends all cards in hand to the player
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player to send cards to
 */
void giveCard(players_t players, int player);

/**
 * @brief Confirms to the player that their card is valid
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player to notify
 */
void okCard(players_t players, int player);

/**
 * @brief Sends the current trick to all players for display
 * @param[in] players Array of player pointers
 * @param[in] pli Current trick (4 cards)
 */
void givePli(players_t players, pli_t pli);

/**
 * @brief Asks a player if they want to take the revealed trump card (first round)
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player being asked
 * @return true if player accepts, false otherwise
 */
bool askTakeAtout(players_t players, int player);

/**
 * @brief Asks a player if they want to choose trump (second round) and which suit
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player being asked
 * @param[out] c Chosen trump suit (if player accepts)
 * @return true if player accepts and chooses a suit, false otherwise
 */
bool askTakeAtoutTurn2(players_t players, int player, enum colorCard *c);

/**
 * @brief Asks a player which card they want to play
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player being asked
 * @return The card chosen by the player
 */
enum card askCard(players_t players, int player);

// -------------------- Variable/Value Functions ------------------------------

/**
 * @brief Gets the point value of a non-trump card
 * @param[in] card Card to evaluate
 * @return Point value of the card
 */
int getValueCard(enum card card);

/**
 * @brief Gets the point value of a trump card
 * @param[in] card Card to evaluate
 * @return Point value of the card
 */
int getValueAtoutCard(enum card card);

/**
 * @brief Gets the name of a card as a string
 * @param[in] card Card to get the name of
 * @return String representation of the card
 */
char* getNameCard(enum card card);

/**
 * @brief Gets the string representation of a card's value
 * @param[in] card Card to get the value string of
 * @return String representation of the card's value
 */
char* getValueCardString(enum card card);

/**
 * @brief Gets the string representation of a card's suit
 * @param[in] color Suit to get the string of
 * @return String representation of the suit
 */
char* getCouleurCardString(enum colorCard color);

/**
 * @brief Gets the ASCII color code for a card suit
 * @param[in] color Suit to get the ASCII color for
 * @return ASCII color code string
 */
char* getAsciiColor(enum colorCard color);




// -------------------- Card Management Functions -----------------------------

/**
 * @brief Deals one card from the deck
 * @param[in,out] pileDeck Pointer to the deck
 * @return The dealt card
 */
enum card dealCard(pileCard_t* pileDeck);

/**
 * @brief Resets all cards to initial state for a new round
 * @param[out] pileDeck Deck to reset
 * @param[out] pileEq1 Team 1's won cards pile to reset
 * @param[out] pileEq2 Team 2's won cards pile to reset
 * @param[out] players Array of players whose hands will be cleared
 */
void resetCards(pileCard_t* pileDeck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players);

/**
 * @brief Converts a card name string to its enum value
 * @param[in] s String representation of the card (e.g., "H_AS")
 * @param[out] out Pointer to store the resulting card enum
 * @return 1 if conversion successful, 0 if card name not found
 */
int string_to_card(const char *s, enum card *out);

/**
 * @brief Calculates points earned by a team from their won cards
 * @param[in] pileEq1 Team 1's won cards pile
 * @param[in] pileEq2 Team 2's won cards pile
 * @param[in] eq Team to calculate points for
 * @param[in] c Trump suit
 * @return Total points earned by the specified team
 */
int point_of_gain(pileCard_t* pileEq1, pileCard_t* pileEq2, enum equipe eq, enum colorCard c);

/**
 * @brief Shuffles the deck randomly
 * @param[in,out] pileDeck Deck to shuffle
 */
void cardShuffle(pileCard_t* pileDeck);

/**
 * @brief Verifies if a character represents a valid card suit
 * @param[in] color Character to verify (H, C, P, or T)
 * @return true if valid suit, false otherwise
 */
bool verifColor(char color);

/**
 * @brief Determines which player wins the trick
 * @param[in] pli The trick (4 cards)
 * @param[in] c Trump suit
 * @return Index of the winning player
 */
int betterInPli(pli_t pli, enum colorCard c);

/**
 * @brief Converts a card to its suit
 * @param[in] card Card to convert
 * @return The suit of the card, or NONE if card is NOTHING
 */
enum colorCard card2Color(enum card card);

/**
 * @brief Finds the highest value card of a specific suit in a player's hand
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player
 * @param[in] color Suit to search for
 * @param[in] colorAtout Trump suit (affects value calculation)
 * @return The highest card of the specified suit, or NOTHING if none found
 */
enum card searchMaxCardInHand(players_t players, int player, enum colorCard color, enum colorCard colorAtout);

/**
 * @brief Validates if a card can be legally played according to Belote rules
 * @param[in] players Array of player pointers
 * @param[in] pli Current trick
 * @param[in] player Index of player attempting to play
 * @param[in] colorAtout Trump suit
 * @param[in,out] colorPli Suit led in the trick (updated if first card)
 * @param[in] card Card the player wants to play
 * @return true if card is legal, false otherwise
 */
bool verifCard(players_t players, pli_t pli, int player, enum colorCard colorAtout, enum colorCard *colorPli, enum card card);

/**
 * @brief Finds the highest value card of a specific suit in the current trick
 * @param[in] pli Current trick
 * @param[in] color Suit to search for
 * @param[in] colorAtout Trump suit (affects value calculation)
 * @return The highest card of the specified suit, or NOTHING if none found
 */
enum card searchMaxCardInPli(pli_t pli, enum colorCard color, enum colorCard colorAtout);

/**
 * @brief Determines if a player's card beats the current best card in the trick
 * @param[in] maxPlayerCard Player's card to check
 * @param[in] maxPliCard Current best card in the trick
 * @param[in] colorAtout Trump suit
 * @param[in] colorPli Suit led in the trick
 * @return true if player's card beats the trick card, false otherwise
 */
bool isOvercut(enum card maxPlayerCard, enum card maxPliCard, enum colorCard colorAtout, enum colorCard colorPli);

/**
 * @brief Removes a card from a player's hand and shifts remaining cards
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player
 * @param[in] card Card to remove
 */
void removeCardFromHand(players_t players, int player, enum card card);

// -------------------- Game Flow Functions -----------------------------------

/**
 * @brief Initializes a card pile by allocating memory
 * @param[out] pile Pointer to pile pointer to initialize
 */
void initPile(pileCard_t **pile);

/**
 * @brief Performs the first deal (3 cards to each player)
 * @param[in,out] deck Deck to deal from
 * @param[out] players Players receiving cards
 * @param[in] startPlayer Pointer to the starting player index
 * @param[in] pli Current trick (for display)
 */
void firstDeal(pileCard_t* deck, players_t players, int *startPlayer, pli_t pli);

/**
 * @brief Conducts the trump selection phase
 * @param[in] players Array of player pointers
 * @param[in] turn Round of bidding (1 or 2)
 * @param[in] startPlayer Pointer to the starting player index
 * @param[out] c Chosen trump suit (only set in round 2)
 * @return Index of player who accepted trump, or -1 if all passed
 */
int playerTurnAtout(players_t players, int turn, int *startPlayer, enum colorCard *c);

/**
 * @brief Performs the second deal (2 cards to each player + reveal trump card)
 * @param[in,out] deck Deck to deal from
 * @param[out] players Players receiving cards
 * @param[in] startPlayer Pointer to the starting player index
 * @param[out] pli Current trick (stores revealed trump card)
 */
void secondDeal(pileCard_t* deck, players_t players, int *startPlayer, pli_t pli);

/**
 * @brief Performs the third deal (3 cards to each player, trump taker gets revealed card)
 * @param[in,out] deck Deck to deal from
 * @param[out] players Players receiving cards
 * @param[in] startPlayer Pointer to the starting player index
 * @param[in,out] pli Current trick (trump card will be taken)
 * @param[in] playerTakeAtout Index of player who accepted trump
 */
void thirdDeal(pileCard_t* deck, players_t players, int *startPlayer, pli_t pli, int playerTakeAtout);

/**
 * @brief Manages the complete dealing phase including trump selection
 * @param[in,out] deck Deck to deal from
 * @param[out] pileEq1 Team 1's won cards pile (cleared)
 * @param[out] pileEq2 Team 2's won cards pile (cleared)
 * @param[out] players Players receiving cards
 * @param[in] startPlayer Pointer to the starting player index
 * @param[out] pli Current trick
 * @param[out] c Trump suit determined
 * @return true if trump was selected, false if all players passed
 */
bool turnDeal(pileCard_t *deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int *startPlayer, pli_t pli, enum colorCard *c);

/**
 * @brief Calculates the next player in turn order
 * @param[in] startPlayer Pointer to the starting player index
 * @param[in] nbNextPlayer Number of positions to advance
 * @return Index of the next player (0-3)
 */
int nextPlayingPlayer(int *startPlayer, int nbNextPlayer);

/**
 * @brief Manages one complete trick of play
 * @param[in,out] deck Game deck (unused in this function)
 * @param[in,out] pileEq1 Team 1's won cards pile
 * @param[in,out] pileEq2 Team 2's won cards pile
 * @param[in,out] players Array of players
 * @param[in,out] startPlayer Pointer to starting player (updated to winner)
 * @param[in,out] pli Current trick
 * @param[in] c Trump suit
 */
void turnNormal(pileCard_t *deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int *startPlayer, pli_t pli, enum colorCard *c);

/**
 * @brief Manages one complete round (manche) of Belote
 * @param[in,out] deck Game deck
 * @param[in,out] pileEq1 Team 1's won cards pile
 * @param[in,out] pileEq2 Team 2's won cards pile
 * @param[in,out] players Array of players
 * @param[in,out] startPlayer Pointer to starting player
 * @param[in,out] pli Current trick
 * @param[out] c Trump suit
 * @param[in,out] scoreEq1 Team 1's score (updated)
 * @param[in,out] scoreEq2 Team 2's score (updated)
 * @return true if round completed successfully, false if all players passed on trump
 */
bool manche(pileCard_t *deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int *startPlayer, pli_t pli, enum colorCard *c, int *scoreEq1, int *scoreEq2);

/**
 * @brief Main game loop managing complete Belote game until a team wins
 * @param[in] players Array of 4 players
 */
void game(players_t players);

// -------------------- Display Functions -------------------------------------

/**
 * @brief Sets console text color based on card suit
 * @param[in] card Card whose suit determines the color
 */
void str_color(enum card card);

/**
 * @brief Displays a single card in a formatted manner
 * @param[in] card Card to display
 */
void afficher_carte(const enum card card);

/**
 * @brief Displays multiple cards side by side in a formatted manner
 * @param[in] cards Array of cards to display
 * @param[in] size Number of cards in the array
 */
void afficherCards(const enum card* cards, const int size);

/**
 * @brief Displays all cards: deck and both teams' won piles
 * @param[in] pileDeck Remaining cards in deck
 * @param[in] pileEq1 Team 1's won cards
 * @param[in] pileEq2 Team 2's won cards
 */
void afficherGainEq(pileCard_t* pileDeck, pileCard_t* pileEq1, pileCard_t* pileEq2);

/**
 * @brief Displays all cards in a team's won pile
 * @param[in] pileEq Team's card pile to display
 */
void afficherEq(pileCard_t* pileEq);

/**
 * @brief Displays remaining cards in the deck
 * @param[in] pileDeck Deck to display
 */
void afficherDeck(pileCard_t* pileDeck);

/**
 * @brief Displays all players and their hands
 * @param[in] players Array of players to display
 */
void afficherPlayers(players_t players);

/**
 * @brief Displays the current trick
 * @param[in] pli Trick to display
 */
void afficherPli(pli_t pli);

#endif /* MOTEUR_H */