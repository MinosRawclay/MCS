/**
 * @file moteur_standalone.c
 * @brief Include of Implementation of the Belote card game
 * @details This program implements the French card game Belote for 4 players in 2 teams.
 *          It handles card dealing, trump selection, trick playing, and scoring.
 * @author Raphael ALLO
 * @date 04/02/2026
 */
#include "../include/moteur.h"

// ==================== COMMUNICATION =====================================================

/**
 * @brief Converts the current trick to a string format for sending
 * @param[out] pliStr String buffer to store the result
 * @param[in] pli Current trick (4 cards)
 */
void pli2str(char*pliStr, pli_t pli){
    pliStr[0] = '\0';  // Initialize the string buffer
    for (int i = 0; i < PLAYERS_MAX; i++) {
        if (i > 0) strcat(pliStr, "|");
        if (pli[i] != NOTHING) {
            strcat(pliStr, getNameCard(pli[i]));
        } else {
            strcat(pliStr, "NOTHING");
        }
    }
    return;
}

/**
 * @brief Converts a player's cards to a string format for sending
 * @param[out] cardsStr String buffer to store the result
 * @param[in] player Pointer to the player
 */
void playerCards2str(char* cardsStr, player_t* player){
    cardsStr[0] = '\0';  // Initialize the string buffer
    for (int i = 0; i < NB_CARD_HAND; i++) {
        if (player->cards[i] != NOTHING) {
            if (strlen(cardsStr) > 0) {
                strcat(cardsStr, "|");
            }
            strcat(cardsStr, getNameCard(player->cards[i]));
        }
    }
    return;
}

/**
 * @brief Converts a string format back to an array of cards
 * @param[in] str String representation of cards (e.g., "H_AS|C_10|P_V")
 * @param[out] cards Array to store the resulting cards
 * @param[in] maxCards Maximum number of cards to parse
 */
void str2Cards(const char* str, enum card* cards, int maxCards){
    char buffer[MAX_LINE];
    char* token = strtok(buffer, "|");
    int index = 0;

    strncpy(buffer, str, MAX_LINE);
    buffer[MAX_LINE - 1] = '\0';  // Ensure null-termination
    while (token != NULL && index < maxCards) {
        enum card c;
        if (string_to_card(token, &c)) {
            cards[index++] = c;
        } else {
            cards[index++] = NOTHING;  // Handle invalid card names
        }
        token = strtok(NULL, "|");
    }
    // Fill remaining slots with NOTHING
    while (index < maxCards) {
        cards[index++] = NOTHING;
    }
    return;
}

/**
 * @brief Adds a new player to the game
 * @param[in,out] players Array of player pointers
 * @param[in,out] nbPlayer Current number of players, incremented if successful
 * @return true if player was added successfully, false if game is full or allocation failed
 */
bool addPlayer(players_t players, int *nbPlayer){
    if (*nbPlayer>=4){return false;}

    players[*nbPlayer] = (player_t*) malloc(sizeof(player_t));
    if (players[0] == NULL) {
        return false;
    }
    players[*nbPlayer]->num=*nbPlayer;
    players[*nbPlayer]->sock = NULL;  // Initialiser la socket à NULL
    for (int i = 0; i < NB_CARD_HAND; i++)
    {
        players[*nbPlayer]->cards[i]=NOTHING;
    }
    (*nbPlayer)++;
    return true;
}

/**
 * @brief Confirms to the player that their card is valid
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player to notify
 */
void okCard(players_t players, int player){
    printf("Player %d card is valid.\n", player);
    return;
}

/**
 * @brief Sends all cards in hand to the player
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player to send cards to
 */
void giveCard(players_t players, int player){
    // Joueur local : pas besoin d'envoyer
    printf("Player %d cards:\n", player);
    afficherCards(players[player]->cards,NB_CARD_HAND);    
    return;
}

/**
 * @brief Notifies all players that the game is starting
 * @param[in] players Array of player pointers
 */
void startPartie(players_t players){    
    // Affichage pour le joueur local
    printf("\n========================================\n");
    printf("       LA PARTIE COMMENCE !\n");
    printf("========================================\n\n");
}

/**
 * @brief Sends the current trick to all players for display
 * @param[in] players Array of player pointers
 * @param[in] pli Current trick (4 cards)
 */
void givePli(players_t players, pli_t pli){
    // Afficher pour le joueur local
    afficherCards(pli,PLAYERS_MAX);
    return;
}

/**
 * @brief Asks a player if they want to take the revealed trump card (first round)
 * @param[in] players Array of player pointers
 * @param[in] pli Current trick (contains revealed trump card)
 * @param[in] player Index of the player being asked
 * @return true if player accepts, false otherwise
 */
bool askTakeAtout(players_t players, pli_t pli, int player){
    char choice;
    // Joueur local
    printf(" =================== Player %d ===================\n", player);
    afficherCards(players[player]->cards, NB_CARD_HAND);
    printf("Tu prends l'atout ? (1=oui, 0=non) : ");
    scanf(" %c", &choice);
    return (choice == '1');
}

/**
 * @brief Asks a player if they want to choose trump (second round) and which suit
 * @param[in] players Array of player pointers
 * @param[in] pli Current trick (contains revealed trump card)
 * @param[in] player Index of the player being asked
 * @param[out] c Chosen trump suit (if player accepts)
 * @return true if player accepts and chooses a suit, false otherwise
 */
bool askTakeAtoutTurn2(players_t players, pli_t pli, int player, enum colorCard *c){
    char color = 0;
    char choice = 0;

    // Joueur local
    printf("Tu prends l'atout T2 ? (1=oui, 0=non) : ");
    scanf(" %c", &choice);
    if (choice == '0') return false;
    do {
        printf("Quelle couleur (H/C/P/T) : ");
        scanf(" %c", &color);
    } while (!verifColor(color));
    *c = color;
    printf("Couleur choisie : %c\n", color);
    return true;
}

/**
 * @brief Sends the final scores to all players at the end of the game
 * @param[in] players Array of player pointers
 * @param[in] scoreEq1 Final score of team 1
 * @param[in] scoreEq2 Final score of team 2
 * @return true if scores were sent successfully, false if any error occurred
 */
bool giveScore(players_t players, int scoreEq1, int scoreEq2){

    // Joueur local : pas besoin d'envoyer
    printf("Score - Equipe 1: %d, Equipe 2: %d\n", scoreEq1, scoreEq2);
    return true;
}

/**
 * @brief Asks a player which card they want to play
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player being asked
 * @return The card chosen by the player
 */
enum card askCard(players_t players, int player){

    // Joueur local - logique simple
    printf(" =================== Player %d ===================\n", player);
    afficherCards(players[player]->cards, NB_CARD_HAND);
    printf("Tu mets quelle carte ? \n");
    int choice;
    scanf(" %d", &choice);
    if (choice >= 0 && choice < NB_CARD_HAND) {
        return players[player]->cards[choice];
    }
    return NOTHING;
}

// ==================== VARIABLES =====================================================

/**
 * @brief Gets the point value of a non-trump card
 * @param[in] card Card to evaluate
 * @return Point value of the card
 */
int getValueCard(enum card card){
    int value[9] = {11, 0, 0, 0, 10, 2, 3, 4, 0};         ///< Point values for non-trump cards
    int numero = card % 8;
    return value[numero];
}

/**
 * @brief Gets the point value of a trump card
 * @param[in] card Card to evaluate
 * @return Point value of the card
 */
int getValueAtoutCard(enum card card){
    int valueAtout[9] = {11, 0, 0, 14, 10, 20, 3, 4, 0};  ///< Point values for trump cards
    int numero = card % 8;
    return valueAtout[numero];
}

/**
 * @brief Gets the name of a card as a string
 * @param[in] card Card to get the name of
 * @return String representation of the card
 */
char* getNameCard(enum card card){
    if (card == NOTHING) return "NOTHING";
    
    char *CARD_NAMES[NB_CARD_DECK] = {
    "H_AS","H_7","H_8","H_9","H_10","H_V","H_D","H_R",
    "C_AS","C_7","C_8","C_9","C_10","C_V","C_D","C_R",
    "P_AS","P_7","P_8","P_9","P_10","P_V","P_D","P_R",
    "T_AS","T_7","T_8","T_9","T_10","T_V","T_D","T_R"
    };
    return CARD_NAMES[card];
}

/**
 * @brief Gets the string representation of a card's value
 * @param[in] card Card to get the value string of
 * @return String representation of the card's value
 */
char* getValueCardString(enum card card){
    char *VALUE_CARD_NAMES[9] = {
    "AS","7","8","9","10","V","D","R",""
    };
    int numero = card % 8;
    return VALUE_CARD_NAMES[numero];
}

/**
 * @brief Gets the string representation of a card's suit
 * @param[in] color Suit to get the string of
 * @return String representation of the suit
 */
char* getCouleurCardString(enum colorCard color){
    char *COULEUR_CARD_NAMES[4] = {
    "♥","♦","♠","♣"
    };
    return COULEUR_CARD_NAMES[color];
}

/**
 * @brief Gets the ASCII color code for a card suit
 * @param[in] color Suit to get the ASCII color for
 * @return ASCII color code string
 */
char* getAsciiColor(enum colorCard color){
    char *ASCII_COLOR[5] = {
    "\x1b[31m", // Hearts - Red
    "\x1b[35m", // Diamonds - Magenta
    "\x1b[33m", // Spades - Bleu
    "\x1b[32m", // Clubs - Green
    "\x1b[0m"   // Reset
    };
    return ASCII_COLOR[color];
}

// ==================== CARD ==============================================================

/**
 * @brief Deals one card from the deck
 * @param[in,out] pileDeck Pointer to the deck
 * @return The dealt card
 * @note Increments lastcard index and marks dealt position as empty
 */
enum card dealCard(pileCard_t* pileDeck){
    enum card c = pileDeck->deck[pileDeck->lastcard];
    pileDeck->deck[pileDeck->lastcard] = -1;
    pileDeck->lastcard++;
    return c;
}

/**
 * @brief Resets all cards to initial state for a new round
 * @param[out] pileDeck Deck to reset
 * @param[out] pileEq1 Team 1's won cards pile to reset
 * @param[out] pileEq2 Team 2's won cards pile to reset
 * @param[out] players Array of players whose hands will be cleared
 */
void resetCards(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2, players_t players){
    for (int i = 0; i < 32; i++)
    {
        pileEq1->deck[i]=NOTHING;
        pileEq1->lastcard = 0;
        pileEq2->deck[i]=NOTHING;
        pileEq2->lastcard = 0;
        pileDeck->deck[i]=i;
        pileDeck->lastcard = 0;
    }
    pileDeck->lastcard = 0;
    for (int i = 0; i < PLAYERS_MAX; i++){
        for (int j = 0; j < NB_CARD_HAND; j++)
            {players[i]->cards[j]=NOTHING;}
        //players[i]->num = i;   
    }
}

/**
 * @brief Converts a card name string to its enum value
 * @param[in] s String representation of the card (e.g., "H_AS")
 * @param[out] out Pointer to store the resulting card enum
 * @return 1 if conversion successful, 0 if card name not found
 */
int string_to_card(const char *s, enum card *out) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(s, getNameCard(i)) == 0) {
            *out = (enum card)i;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Calculates points earned by a team from their won cards
 * @param[in] pileEq1 Team 1's won cards pile
 * @param[in] pileEq2 Team 2's won cards pile
 * @param[in] eq Team to calculate points for
 * @param[in] c Trump suit
 * @return Total points earned by the specified team
 */
int point_of_gain(pileCard_t* pileEq1,pileCard_t* pileEq2, enum equipe eq,enum colorCard c){
    int points=0;
    enum colorCard color=0;
    int i=0;
    enum card card;

    if (eq == EQUIPE1){
        while ((card=pileEq1->deck[i])!=NOTHING)
        {
            color = card2Color(card);
            points+=(color==c)?getValueAtoutCard(card):getValueCard(card);
            i++;
        }
    }
    else
    {
       while ((card=pileEq2->deck[i])!=NOTHING)
        {
            color = card2Color(card);
            points+=(color==c)?getValueAtoutCard(card):getValueCard(card);
            i++;
        } 
    }
    return points;
}

/**
 * @brief Shuffles the deck randomly
 * @param[in,out] pileDeck Deck to shuffle
 * @note Uses Fisher-Yates shuffle algorithm
 */
void cardShuffle(pileCard_t* pileDeck) {
    for (int i = NB_CARD_DECK - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        enum card temp = pileDeck->deck[j];
        pileDeck->deck[j] = pileDeck->deck[i];
        pileDeck->deck[i] = temp;
    }
    pileDeck->lastcard=0;
}

/**
 * @brief Verifies if a character represents a valid card suit
 * @param[in] color Character to verify (H, C, P, or T)
 * @return true if valid suit, false otherwise
 */
bool verifColor(char color){
    if(color == 'H' || color == 'C' || color == 'P' || color == 'T') {
        return true;}
    return false;
}

/**
 * @brief Determines which player wins the trick
 * @param[in] pli The trick (4 cards)
 * @param[in] c Trump suit
 * @return Index of the winning player
 * @note Considers trump hierarchy and card values
 */
int betterInPli(pli_t pli, enum colorCard c){
    int color, bestVal, val, bestP, bestColor;
    enum card card, bestCard;
    
    bestCard = pli[0];
    bestP = 0;
    bestColor = card2Color(bestCard);
    bestVal = (bestColor==c)?getValueAtoutCard(bestCard):getValueCard(bestCard);
    for (int i = 1; i < PLAYERS_MAX; i++)
    {
        card = pli[i];
        color = card2Color(card);
        if (color==c)//Card is Atout
        {        
            if (bestColor==c){// Both are Atout
                val = getValueAtoutCard(card);
                if (val < bestVal) break; // new Card has a smaller value thant the old
                // Both are atout and of same value -> 7 of Atout & 8 of Atout, so the biggest is the best                
                if(val == bestVal && card<=bestCard) break; 
            }
            // old card isn't Atout
               
        }
        else//Card isn't Atout
        {
            if(bestColor==c)break;//old is a Atout
            val = getValueCard(card);
            if (val < bestVal) break; // new Card has a smaller value thant the old
            // Both are atout and of same value -> 7 of Atout & 8 of Atout, so the biggest is the best                
            if(val == bestVal && card<=bestCard) break; 
        }
        bestColor = color;
        bestCard = card;
        bestP = i;
    }
    return bestP;
}

/**
 * @brief Initializes a card pile by allocating memory
 * @param[out] pile Pointer to pile pointer to initialize
 * @note Exits program on allocation failure
 */
void initPile(pileCard_t **pile)
{
    *pile = malloc(sizeof(pileCard_t));
    if (*pile == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Validates if a card can be legally played according to Belote rules
 * @param[in] players Array of player pointers
 * @param[in] pli Current trick
 * @param[in] player Index of player attempting to play
 * @param[in] colorAtout Trump suit
 * @param[in,out] colorPli Suit led in the trick (updated if first card)
 * @param[in] card Card the player wants to play
 * @return true if card is legal, false otherwise
 * @details Implements Belote rules:
 *          - Must follow suit if possible
 *          - Must trump if can't follow suit
 *          - Must overtrump if possible
 *          - Partner winning allows any card
 */
bool verifCard(players_t players, pli_t pli, int player, enum colorCard colorAtout, enum colorCard * colorPli, enum card card){
    int flag = 0;
    int i = 0;
    int nbCardPli = 0;
    enum card maxAtoutCard = NOTHING;
    enum card maxColorCard = NOTHING;
    enum card maxAtoutCardPli = NOTHING;
    enum card maxColorCardPli = NOTHING;
    enum card maxCardPli = NOTHING;
    enum colorCard colorCard = card2Color(card);

    printf("[verifCard] === DEBUT verifCard ===\n");
    printf("[verifCard] player=%d, card jouée=%s (color=%d), colorAtout=%d\n",
           player, getNameCard(card), colorCard, colorAtout);

    // Afficher la main du joueur
    printf("[verifCard] Main du joueur %d : ", player);
    for (int j = 0; j < NB_CARD_HAND; j++) {
        if (players[player]->cards[j] == NOTHING) break;
        printf("%s ", getNameCard(players[player]->cards[j]));
    }
    printf("\n");

    // Afficher le pli actuel
    printf("[verifCard] Pli actuel : ");
    for (int j = 0; j < PLAYERS_MAX; j++) {
        if (pli[j] == NOTHING) break;
        printf("%s ", getNameCard(pli[j]));
    }
    printf("\n");

    // Vérifier si la carte est dans la main
    for (i = 0; i < NB_CARD_HAND; i++)
    {
        if (players[player]->cards[i] == NOTHING) break;
        if(card == players[player]->cards[i]) flag = 1;
    }
    if(flag == 0) {
        printf("[verifCard] ERREUR : carte %s PAS dans la main du joueur %d\n",
               getNameCard(card), player);
        return false;
    }
    flag = 0;
    i = 0;
    printf("[verifCard] OK : carte %s bien dans la main\n", getNameCard(card));

    // Premier joueur du pli
    if (pli[0] == NOTHING){
        *colorPli = colorCard;
        printf("[verifCard] Premier joueur du pli -> colorPli défini à %d\n", *colorPli);
        printf("[verifCard] === RETOUR TRUE (première carte) ===\n");
        return true;
    }

    *colorPli = card2Color(pli[0]);
    printf("[verifCard] colorPli (couleur demandée) = %d\n", *colorPli);

    maxAtoutCard = searchMaxCardInHand(players, player, colorAtout, colorAtout);
    maxColorCard = searchMaxCardInHand(players, player, *colorPli, colorAtout);
    printf("[verifCard] maxAtoutCard (meilleur atout en main) = %s\n", getNameCard(maxAtoutCard));
    if (maxAtoutCard != NOTHING) printf("[verifCard] maxColorCard (meilleure carte couleur demandée en main) = %s\n", getNameCard(maxColorCard));
    if (maxColorCard != NOTHING) printf("[verifCard] maxColorCard (meilleure carte couleur demandée en main) = %s\n", getNameCard(maxColorCard));

    if (card2Color(card) == *colorPli )
    {
        printf("[verifCard] Le joueur suit la couleur demandée\n");
        printf("[verifCard] === RETOUR TRUE (carte de la couleur demandée) ===\n");
        return true;
    }
    // Le joueur a la couleur demandée -> il doit la jouer
    if(maxColorCard != NOTHING && card2Color(card) != *colorPli) {
        printf("[verifCard] REFUS : le joueur a la couleur demandée (%s) mais joue %s\n",
               getNameCard(maxColorCard), getNameCard(card));
        return false;
    }
    

    // === CAS 1 : L'atout est demandé ===
    if (*colorPli == colorAtout)
    {
        printf("[verifCard] --- CAS : Atout est demandé ---\n");
        maxAtoutCardPli = searchMaxCardInPli(pli, colorAtout, colorAtout);
        printf("[verifCard] maxAtoutCardPli (meilleur atout dans le pli) = %s\n", getNameCard(maxAtoutCardPli));
        printf("[verifCard] maxCardPli (utilisé pour isOvercut) = %s\n", getNameCard(maxCardPli));
        // BUG POTENTIEL : maxCardPli n'est pas mis à jour ici avant isOvercut
        // Il vaut encore NOTHING à ce point. Devrait être = maxAtoutCardPli ?

        if(isOvercut(card, maxCardPli, colorAtout, *colorPli))
        {
            printf("[verifCard] maxAtoutCard=%s, maxCardPli=%s, card=%s\n",
                   getNameCard(maxAtoutCard), getNameCard(maxCardPli), getNameCard(card));
            printf("[verifCard] isOvercut(maxAtoutCard, maxAtoutCardPli)=%d, isOvercut(card, maxAtoutCardPli)=%d\n",
                   isOvercut(maxAtoutCard, maxAtoutCardPli, colorAtout, *colorPli),
                   isOvercut(card, maxAtoutCardPli, colorAtout, *colorPli));
            printf("[verifCard] === RETOUR TRUE (overcut atout demandé) ===\n");
            return true;
        }
        if (isOvercut(maxAtoutCard, maxCardPli, colorAtout, *colorPli))
        {
            printf("[verifCard] REFUS : le joueur PEUT overcutter (maxAtoutCard=%s) mais ne le fait pas (card=%s)\n",
                   getNameCard(maxAtoutCard), getNameCard(card));
            printf("[verifCard] === RETOUR FALSE (pas d'overcut alors que possible) ===\n");
            return false;
        }
        printf("[verifCard] Atout demandé : pas d'overcut possible, carte acceptée par défaut\n");
    }
    // === CAS 2 : L'atout n'est PAS demandé ===
    else
    {
        printf("[verifCard] --- CAS : Atout PAS demandé ---\n");

        if(maxAtoutCard == NOTHING){
            printf("[verifCard] Le joueur n'a pas d'atout en main\n");
            printf("[verifCard] === RETOUR TRUE (pas d'atout disponible) ===\n");
            return true;
        }

        // Compter les cartes dans le pli
        while (i < PLAYERS_MAX && pli[i] != NOTHING){ i++; }
        nbCardPli = i;
        i = 0;
        printf("[verifCard] Nombre de cartes dans le pli : %d\n", nbCardPli);
        

        // Chercher les meilleures cartes dans le pli
        maxAtoutCardPli = searchMaxCardInPli(pli, colorAtout, colorAtout);
       
        maxColorCardPli = searchMaxCardInPli(pli, *colorPli, colorAtout);
        
        //printf("[verifCard] maxAtoutCardPli (meilleur atout dans le pli) = %s\n", getNameCard(maxAtoutCardPli));
        //printf("[verifCard] maxColorCardPli (meilleure carte couleur dans le pli) = %s\n", getNameCard(maxColorCardPli));

        if (maxAtoutCardPli != NOTHING)
            maxCardPli = maxAtoutCardPli;
        else
            maxCardPli = maxColorCardPli;
        printf("[verifCard] maxCardPli (meilleure carte globalement dans le pli) = %s\n", getNameCard(maxCardPli));

        // Vérifier si le partenaire gagne le pli
        if(nbCardPli >= 2) {
            printf("[verifCard] Carte du partenaire (pli[%d]) = %s, maxCardPli = %s\n",
                   nbCardPli - 2, getNameCard(pli[nbCardPli - 2]), getNameCard(maxCardPli));
            if (pli[nbCardPli - 2] == maxCardPli) {
                printf("[verifCard] Le partenaire gagne le pli -> toute carte autorisée\n");
                printf("[verifCard] === RETOUR TRUE (partenaire gagne) ===\n");
                return true;
            }
        }

        // Vérifier si le joueur overcutte
        printf("[verifCard] Vérifie isOvercut(card=%s, maxCardPli=%s)\n",
               getNameCard(card), getNameCard(maxCardPli));
        if(isOvercut(card, maxCardPli, colorAtout, *colorPli))
        {
            printf("[verifCard] maxAtoutCard=%s, maxCardPli=%s, card=%s\n",
                   getNameCard(maxAtoutCard), getNameCard(maxCardPli), getNameCard(card));
            printf("[verifCard] isOvercut(maxAtoutCard, maxAtoutCardPli)=%d, isOvercut(card, maxAtoutCardPli)=%d\n",
                   isOvercut(maxAtoutCard, maxAtoutCardPli, colorAtout, *colorPli),
                   isOvercut(card, maxAtoutCardPli, colorAtout, *colorPli));
            printf("[verifCard] === RETOUR TRUE (overcut réussi) ===\n");
            return true;
        }
        // Le joueur peut overcutter mais ne le fait pas
        printf("[verifCard] Vérifie isOvercut(maxAtoutCard=%s, maxCardPli=%s)\n",
               getNameCard(maxAtoutCard), getNameCard(maxCardPli));
        if (isOvercut(maxAtoutCard, maxCardPli, colorAtout, *colorPli))
        {
            printf("[verifCard] REFUS : le joueur PEUT overcutter (maxAtoutCard=%s) mais joue %s\n",
                   getNameCard(maxAtoutCard), getNameCard(card));
            printf("[verifCard] === RETOUR FALSE (pas d'overcut alors que possible) ===\n");
            return false;
        }
        printf("[verifCard] Pas d'overcut possible, carte acceptée par défaut\n");
    }
    printf("[verifCard] === RETOUR TRUE (fin de fonction) ===\n");
    return true;
}

/**
 * @brief Finds the highest value card of a specific suit in a player's hand
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player
 * @param[in] color Suit to search for
 * @param[in] colorAtout Trump suit (affects value calculation)
 * @return The highest card of the specified suit, or NOTHING if none found
 */
enum card searchMaxCardInHand(players_t players, int player, enum colorCard color, enum colorCard colorAtout){
    enum card maxCard = NOTHING;

    if(color == colorAtout){
        for (int i = 0; i < NB_CARD_HAND; i++)
        {
            if (players[player]->cards[i] == NOTHING) break;
            if (color == card2Color(players[player]->cards[i])) {
                if(getValueAtoutCard(players[player]->cards[i]) > getValueAtoutCard(maxCard)) maxCard = players[player]->cards[i];
                if(getValueAtoutCard(players[player]->cards[i]) == getValueAtoutCard(maxCard) && players[player]->cards[i] > maxCard) maxCard = players[player]->cards[i];
            }
        }
    }
    else for (int i = 0; i < NB_CARD_HAND; i++)
    {
        if (players[player]->cards[i] == NOTHING) break;
        if (color == card2Color(players[player]->cards[i])) {
            if(getValueCard(players[player]->cards[i]) > getValueCard(maxCard)) maxCard = players[player]->cards[i];
            if(getValueCard(players[player]->cards[i]) == getValueCard(maxCard) && players[player]->cards[i] > maxCard) maxCard = players[player]->cards[i];
        }
    }
    return maxCard;
}

/**
 * @brief Finds the highest value card of a specific suit in the current trick
 * @param[in] pli Current trick
 * @param[in] color Suit to search for
 * @param[in] colorAtout Trump suit (affects value calculation)
 * @return The highest card of the specified suit, or NOTHING if none found
 */
enum card searchMaxCardInPli(pli_t pli, enum colorCard color,enum colorCard colorAtout){
    enum card maxCard = NOTHING;
    if(color == colorAtout){
        for (int i = 0; i < PLAYERS_MAX; i++)
        {
            if (pli[i] == NOTHING) break;
            if (color == card2Color(pli[i])) {
                if(getValueAtoutCard(pli[i]) > getValueAtoutCard(maxCard)) maxCard = pli[i];
                if(getValueAtoutCard(pli[i]) == getValueAtoutCard(maxCard) && pli[i] > maxCard) maxCard = pli[i];
            }
        }
    }
    else for (int i = 0; i < PLAYERS_MAX; i++)
    {
        if (pli[i] == NOTHING) break;
        if (color == card2Color(pli[i])) {
            if(getValueCard(pli[i]) > getValueCard(maxCard)) maxCard = pli[i];
            if(getValueCard(pli[i]) == getValueCard(maxCard) && pli[i] > maxCard) maxCard = pli[i];
        }
    }
    return maxCard;
}

/**
 * @brief Determines if a player's card beats the current best card in the trick
 * @param[in] maxPlayerCard Player's card to check
 * @param[in] maxPliCard Current best card in the trick
 * @param[in] colorAtout Trump suit
 * @param[in] colorPli Suit led in the trick
 * @return true if player's card beats the trick card, false otherwise
 */
bool isOvercut(enum card maxPlayerCard, enum card maxPliCard, enum colorCard colorAtout, enum colorCard colorPli){
    enum colorCard colorPlayerCard = card2Color(maxPlayerCard);
    enum colorCard colorPliCard = card2Color(maxPliCard);

    //Player card isn't colorPli
    if(colorPlayerCard != colorPli && colorPlayerCard != colorAtout) return false;
    //Player card isn't Atout and Pli card is Atout
    if(colorPlayerCard != colorAtout && colorPliCard == colorAtout) return false;
    //Player card is Atout and Pli card isn't Atout
    if(colorPlayerCard == colorAtout &&colorPliCard != colorAtout) return true;
    //Both are not Atout
    if(colorPlayerCard != colorAtout && colorPliCard != colorAtout){
        if(getValueCard(maxPlayerCard) > getValueCard(maxPliCard)) return true;
        return false;
    }
    //Both are Atout
    if(getValueAtoutCard(maxPlayerCard) > getValueAtoutCard(maxPliCard)) return true;
    return false;
}

/**
 * @brief Converts a card to its suit
 * @param[in] card Card to convert
 * @return The suit of the card, or NONE if card is NOTHING
 */
enum colorCard card2Color(enum card card){
    if(card == NOTHING) return NONE;
    return card/8;
}

/**
 * @brief Removes a card from a player's hand and shifts remaining cards
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player
 * @param[in] card Card to remove
 */
void removeCardFromHand(players_t players, int player, enum card card){
    bool flag = false;

    for (int i = 0; i < NB_CARD_HAND; i++)
    {
        if(flag){
            players[player]->cards[i-1]=players[player]->cards[i];
            players[player]->cards[i]=NOTHING;
        }
        if (players[player]->cards[i] == card){
            players[player]->cards[i]=NOTHING;
            flag = true;
        }
    }
}

// ==================== GAME ==============================================

/**
 * @brief Performs the first deal (3 cards to each player)
 * @param[in,out] deck Deck to deal from
 * @param[out] players Players receiving cards
 * @param[in] startPlayer Pointer to the starting player index
 * @param[in] pli Current trick (for display)
 */
void firstDeal(pileCard_t* deck, players_t players, int * startPlayer, pli_t pli){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        players[playingPlayer]->cards[0]=dealCard(deck);
        players[playingPlayer]->cards[1]=dealCard(deck);
        players[playingPlayer]->cards[2]=dealCard(deck);
        giveCard(players, playingPlayer);
        i++;
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
    givePli(players, pli);
}

/**
 * @brief Conducts the trump selection phase
 * @param[in] players Array of player pointers
 * @param[in] pli Current trick (contains revealed trump card)
 * @param[in] turn Round of bidding (1 or 2)
 * @param[in] startPlayer Pointer to the starting player index
 * @param[out] c Chosen trump suit (only set in round 2)
 * @return Index of player who accepted trump, or -1 if all passed
 */
int playerTurnAtout(players_t players, pli_t pli, int turn,int * startPlayer,enum colorCard *c){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        if(turn == 1)
            {if(askTakeAtout(players, pli, playingPlayer))return playingPlayer;}
        else
            {if(askTakeAtoutTurn2(players,pli,playingPlayer,c))return playingPlayer;}
        i++;   
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
    return -1;
}

/**
 * @brief Performs the second deal (2 cards to each player + reveal trump card)
 * @param[in,out] deck Deck to deal from
 * @param[out] players Players receiving cards
 * @param[in] startPlayer Pointer to the starting player index
 * @param[out] pli Current trick (stores revealed trump card)
 */
void secondDeal(pileCard_t* deck, players_t players,int* startPlayer,pli_t pli){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        players[playingPlayer]->cards[3]=dealCard(deck);
        players[playingPlayer]->cards[4]=dealCard(deck);
        giveCard(players, playingPlayer);  
        i++;
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
    pli[0]=dealCard(deck);
}

/**
 * @brief Performs the third deal (3 cards to each player, trump taker gets revealed card)
 * @param[in,out] deck Deck to deal from
 * @param[out] players Players receiving cards
 * @param[in] startPlayer Pointer to the starting player index
 * @param[in,out] pli Current trick (trump card will be taken)
 * @param[in] playerTakeAtout Index of player who accepted trump
 */
void thirdDeal(pileCard_t* deck, players_t players,int* startPlayer, pli_t pli, int playerTakeAtout){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        if(playingPlayer==playerTakeAtout){
            players[playingPlayer]->cards[5]=pli[0];
        }
        else players[playingPlayer]->cards[5]=dealCard(deck);
        players[playingPlayer]->cards[6]=dealCard(deck);
        players[playingPlayer]->cards[7]=dealCard(deck);
        giveCard(players, playingPlayer); 
        i++;   
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
    pli[0]=NOTHING;
}

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
bool turnDeal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorCard * c){
    int p;
    printf("\n[turnDeal] Début du tour de distribution, startPlayer=%d\n", *startPlayer);
    getc(stdin);
    resetCards(deck,pileEq1,pileEq2, players);
    cardShuffle(deck);
    printf("[turnDeal] Deck mélangé\n");
    getc(stdin);

    firstDeal(deck,players,startPlayer,pli);
    secondDeal(deck,players,startPlayer,pli);
    givePli(players,pli);
    if((p = playerTurnAtout(players,pli,1,startPlayer,c))==-1)
        if((p = playerTurnAtout(players,pli,2,startPlayer,c))==-1)
            return false;
    *c=card2Color(pli[0]);
    printf("Player %d took the atout of color %d\n",p,*c);
    thirdDeal(deck,players,startPlayer,pli,p);
    return true;
}

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
void turnNormal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorCard * c){
    int i=0;
    enum card card=NOTHING;
    enum colorCard colorPli=NONE;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        printf("\n[turnNormal] Joueur %d joue son tour\n", playingPlayer);
        printf("colorAtout=%d\n",*c);
        do
        {
            card = askCard(players, playingPlayer);
            printf("Player %d played card %s\n",playingPlayer,getNameCard(card));
            printf("colorAtout=%d\n",*c);
        } while (verifCard(players, pli, playingPlayer, *c, &colorPli, card) == false);
        printf("colorAtout=%d\n",*c);
        removeCardFromHand(players, playingPlayer, card);
        okCard(players, playingPlayer);
        pli[i]=card;
        givePli(players, pli);
        card=NOTHING;
        i++;
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);

    int winnerPli = betterInPli(pli, *c);
    if (winnerPli % 2 == *startPlayer % 2){
        //Equipe 1 win
        for (int j = 0; j < PLAYERS_MAX; j++)
        {
            pileEq1->deck[pileEq1->lastcard]=pli[j];
            pileEq1->lastcard++;
            pli[j]=NOTHING;
        }
    }
    else{
        //Equipe 2 win
        for (int j = 0; j < PLAYERS_MAX; j++)
        {
            pileEq2->deck[pileEq2->lastcard]=pli[j];
            pileEq2->lastcard++;
            pli[j]=NOTHING;
        }
    }
}

/**
 * @brief Calculates the next player in turn order
 * @param[in] startPlayer Pointer to the starting player index
 * @param[in] nbNextPlayer Number of positions to advance
 * @return Index of the next player (0-3)
 */
int nextPlayingPlayer(int* startPlayer, int nbNextPlayer){
    return (*startPlayer+nbNextPlayer)%PLAYERS_MAX;
}

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
bool manche(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorCard * c, int* scoreEq1, int* scoreEq2){
    printf("\n[manche] Début de la manche, startPlayer=%d\n", *startPlayer);
    if(turnDeal( deck, pileEq1, pileEq2, players, startPlayer, pli, c)==false)
        return false;
    printf("Atout is of color %d\n",*c);
    for(int i =0; i < NB_CARD_HAND; i++){
        printf("\n\n\n--- Tour de jeu %d ---\n",i+1);
        turnNormal( deck, pileEq1, pileEq2, players, startPlayer, pli, c);
    }

    *scoreEq1 += point_of_gain(pileEq1,pileEq2,EQUIPE1,*c);
    *scoreEq2 += point_of_gain(pileEq1,pileEq2,EQUIPE2,*c);

    return true;
}

/**
 * @brief Main game loop managing complete Belote game until a team wins
 * @param[in] players Array of 4 players
 */
void game(players_t players){
    pileCard_t* deck;//les cartes existante dans le deck
    pileCard_t* gain_Eq1;
    pileCard_t* gain_Eq2;
    enum card pli[PLAYERS_MAX] = {NOTHING,NOTHING,NOTHING,NOTHING};
    int startPlayer=0;
    enum colorCard colorAtout;
    int scoreEq1=0;
    int scoreEq2=0;

    initPile(&gain_Eq1);
    initPile(&gain_Eq2);
    initPile(&deck);

    // Notifier tous les joueurs que la partie commence
    startPartie(players);

    while (scoreEq1 < POINT_WIN || scoreEq2 < POINT_WIN)
    {
        printf("\n\n\n=== Nouvelle manche ===\n");
        manche(deck,gain_Eq1,gain_Eq2,players,&startPlayer,pli,&colorAtout,&scoreEq1,&scoreEq2);
        afficherGainEq(deck,gain_Eq1,gain_Eq2);
        giveScore(players, scoreEq1, scoreEq2);
    }
    
}

// ==================== AFFICHAGE =========================================

/**
 * @brief Sets console text color based on card suit
 * @param[in] card Card whose suit determines the color
 */
void str_color(enum card card){
    enum colorCard color = card/8;
    //if (color == c){STR_VERT_START;return;}
    switch (color)
    {
    case H:
        STR_ROUGE_START;
        break;
    case C:
        STR_MAGENTA_START;
        break;
    case P:
        STR_BLEU_START;
        break;
    case T:
        STR_VERT_START;
        break;
    
    default:
        break;
    }
}

/**
 * @brief Displays a single card in a formatted manner
 * @param[in] card Card to display
 */
void afficher_carte(const enum card card) {
    printf("┌─────────┐\n");
    printf("│ %-2s      │\n", getValueCardString(card));
    printf("│         │\n");
    printf("│    %s    │\n", getCouleurCardString(card2Color(card)));
    printf("│         │\n");
    printf("│      %-2s │\n", getValueCardString(card));
    printf("└─────────┘\n");
}

/**
 * @brief Displays multiple cards side by side in a formatted manner
 * @param[in] cards Array of cards to display
 * @param[in] size Number of cards in the array
 */
void afficherCards(const enum card* cards, const int size)
{
    char l1[MAX_LINE], l2[MAX_LINE], l3[MAX_LINE];
    char l4[MAX_LINE], l5[MAX_LINE], l6[MAX_LINE], l7[MAX_LINE];
    char color[10];

    for (int i = 0; i < (size + 7) / 8; i++) {
        l1[0]=l2[0]=l3[0]=l4[0]=l5[0]=l6[0]=l7[0]='\0';
        color[0]='\0';
        for (int j = 0; j < 8; j++) {
            int idx = i * 8 + j;
            if (idx >= size) break;
            if(cards[idx]==NOTHING){
                strcat(l1, "           ");
                strcat(l2, "           ");
                strcat(l3, "           ");
                strcat(l4, "           ");
                strcat(l5, "           ");
                strcat(l6, "           ");
                strcat(l7, "           ");
                continue;
            }
            strcpy(color, getAsciiColor(card2Color(cards[idx])));
            strcat(l1, color);strcat(l2, color);strcat(l3, color);
            strcat(l4, color);strcat(l5, color);strcat(l6, color);strcat(l7, color);
            
            strcat(l1, "┌─────────┐ ");
            strcat(l2, "│ ");
            strcat(l2, getValueCardString(cards[idx]));
            if (cards[idx]%8 != 0 && cards[idx]%8 != 4) strcat(l2," ");
            strcat(l2, "      │ ");
            strcat(l3, "│         │ ");
            strcat(l4, "│    ");
            strcat(l4, getCouleurCardString(card2Color(cards[idx])));
            strcat(l4, "    │ ");
            strcat(l5, "│         │ ");
            strcat(l6, "│      ");
            strcat(l6, getValueCardString(cards[idx]));
            if (cards[idx]%8 != 0 && cards[idx]%8 != 4) strcat(l6," ");
            strcat(l6, " │ ");
            strcat(l7, "└─────────┘ ");

            strcat(l1, "\x1b[0m");strcat(l2, "\x1b[0m");strcat(l3, "\x1b[0m");
            strcat(l4, "\x1b[0m");strcat(l5, "\x1b[0m");strcat(l6, "\x1b[0m");strcat(l7, "\x1b[0m");
        }

        printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
               l1,l2,l3,l4,l5,l6,l7);
    }
}

/**
 * @brief Displays all cards: deck and both teams' won piles
 * @param[in] pileDeck Remaining cards in deck
 * @param[in] pileEq1 Team 1's won cards
 * @param[in] pileEq2 Team 2's won cards
 */
void afficherGainEq(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2){
    printf("Deck\n");
    afficherDeck(pileDeck);
    printf("Equipe1\n");
    afficherEq(pileEq1);
    printf("\nFin Eq1\n");
    printf("Equipe2\n");
    afficherEq(pileEq2);
    printf("\nFin Eq2\n");
}

/**
 * @brief Displays all cards in a team's won pile
 * @param[in] pileEq Team's card pile to display
 */
void afficherEq(pileCard_t* pileEq){
    int j=0;
    enum card card=NOTHING;
    while ((card=pileEq->deck[j])!=NOTHING)
    {
        str_color(card);
        printf("-%s-",getNameCard(card));
        STR_COLOR_END;
        j++;
    }
}

/**
 * @brief Displays remaining cards in the deck
 * @param[in] pileDeck Deck to display
 */
void afficherDeck(pileCard_t* pileDeck){
    for (int i = pileDeck->lastcard; i < NB_CARD_DECK; i++)
    {
        str_color(pileDeck->deck[i]);
        printf("-%s-",getNameCard(pileDeck->deck[i]));
        STR_COLOR_END;
        i++;
    }
    
}

/**
 * @brief Displays all players and their hands
 * @param[in] players Array of players to display
 */
void afficherPlayers(players_t players){
    for (int i = 0; i < PLAYERS_MAX; i++)
    {
        printf("player n°%d\n",players[i]->num);
        for (int j = 0; j < NB_CARD_HAND; j++)
        {
            if(players[i]->cards[j]==NOTHING){
                printf("-o-");
            }
            else{
                str_color(players[i]->cards[j]);
                printf("-%s-",getNameCard(players[i]->cards[j]));
                STR_COLOR_END;
            }
        }
        printf("\n");
    }
}

/**
 * @brief Displays the current trick
 * @param[in] pli Trick to display
 */
void afficherPli(pli_t pli){
    printf("Pli : ");
    for (int i = 0; i < 4; i++)
    {
        if (pli[i]==NOTHING)
            {printf("-o-");}
        else{
            str_color(pli[i]);
            printf("-%s-",getNameCard(pli[i]));
            STR_COLOR_END;
        }  
    }
    printf("\n");
}


int main(int argc, char const *argv[])
{
    players_t players;
    int nbPlayer = 0;

    STR_ROUGE_START;
    printf("TEST\n");
    STR_COLOR_END;
    
    // =============================================================
    //création des 4 joueur - connection des clients
    while (nbPlayer < 4)
    {
        addPlayer(players,&nbPlayer);
    }
    afficherPlayers(players);
    
    game(players);
}

