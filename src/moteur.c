/**
 * @file moteur.h
 * @brief Include of Implementation of the Belote card game
 * @details This program implements the French card game Belote for 4 players in 2 teams.
 *          It handles card dealing, trump selection, trick playing, and scoring.
 * @author Raphael ALLO
 * @date 02/02/2026
 */
#include "../include/moteur.h"

// ==================== COMMUNICATION =====================================================

/**
 * @brief Adds a new player to the game
 * @param[in,out] players Array of player pointers
 * @param[in,out] nbPlayer Current number of players, incremented if successful
 * @return true if player was added successfully, false if game is full or allocation failed
 */
bool addPlayer(players_t players, int *nbPlayer){
    if (*nbPlayer>=4){return false;}

    players[*nbPlayer] = (player_t*) malloc(sizeof(player_t));
    if (&players[*nbPlayer] == NULL) {
        return false;
    }
    players[*nbPlayer]->num=*nbPlayer;
    players[*nbPlayer]->s = FINISH;
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
 * @todo Implement network communication to send confirmation
 */
void okCard(players_t players, int player){
    //TODO --> transmet l'ok de la carte au joueur
    return;
}

/**
 * @brief Sends all cards in hand to the player
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player to send cards to
 * @todo Implement network communication to send cards
 */
void giveCard(players_t players, int player){
    return;
}

/**
 * @brief Sends the current trick to all players for display
 * @param[in] players Array of player pointers
 * @param[in] pli Current trick (4 cards)
 * @todo Implement network communication to broadcast trick
 */
void givePli(players_t players, pli_t pli){
    afficherPli(pli);
    afficherPlayers(players);
    return;
}

/**
 * @brief Asks a player if they want to take the revealed trump card (first round)
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player being asked
 * @return true if player accepts, false otherwise
 * @todo Implement network communication for remote players
 */
bool askTakeAtout(players_t players, int player){
    // if(player == 0){CLient INTERNE}
    printf("Tu prend l'atout T1 ? \n");
    printf("yes=1,no=0 :");
    char choice;
    scanf(" %c",&choice);
    if(choice == '1') return true; 
    return false;
}

/**
 * @brief Asks a player if they want to choose trump (second round) and which suit
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player being asked
 * @param[out] c Chosen trump suit (if player accepts)
 * @return true if player accepts and chooses a suit, false otherwise
 * @todo Implement network communication for remote players
 */
bool askTakeAtoutTurn2(players_t players, int player, enum colorCard *c){
    char color=0;
    char choice=0;
    printf("Tu prend l'atout T2 ? \n");
    printf("yes=1,no=0 :");
    scanf(" %c",&choice);
    
    if(choice == '0') return false; 
    do
    {
        printf("color (H,C,P,T) ? \n");
        scanf(" %c",&color);
    } while (!verifColor(color));
    
    *c=color;
    printf("Color : %c\n",color);
    return true;
}

/**
 * @brief Asks a player which card they want to play
 * @param[in] players Array of player pointers
 * @param[in] player Index of the player being asked
 * @return The card chosen by the player
 * @todo Implement proper card selection logic
 */
enum card askCard(players_t players, int player){
    // if(player == 0){CLient INTERNE}
    printf("Tu met quel card ? \n");
    char choice;
    scanf(" %c",&choice);
    if(choice == '1') return true; 
    return false;
}

// ==================== VARIABLES =====================================================

/**
 * @brief Gets the point value of a non-trump card
 * @param[in] card Card to evaluate
 * @return Point value of the card
 */
int getValueCard(enum card card){
    const int value[9] = {11, 0, 0, 0, 10, 2, 3, 4, 0};         ///< Point values for non-trump cards
    int numero = card % 8;
    return value[numero];
}

/**
 * @brief Gets the point value of a trump card
 * @param[in] card Card to evaluate
 * @return Point value of the card
 */
int getValueAtoutCard(enum card card){
    const int valueAtout[9] = {11, 0, 0, 14, 10, 20, 3, 4, 0};  ///< Point values for trump cards
    int numero = card % 8;
    return valueAtout[numero];
}

/**
 * @brief Gets the name of a card as a string
 * @param[in] card Card to get the name of
 * @return String representation of the card
 */
char* getNameCard(enum card card){
    const char *CARD_NAMES[NB_CARD_DECK] = {
    "H_AS","H_7","H_8","H_9","H_10","H_V","H_D","H_R",
    "C_AS","C_7","C_8","C_9","C_10","C_V","C_D","C_R",
    "P_AS","P_7","P_8","P_9","P_10","P_V","P_D","P_R",
    "T_AS","T_7","T_8","T_9","T_10","T_V","T_D","T_R"
    };
    return CARD_NAMES[card];
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
        players[i]->s = FINISH; 
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
    int numero=0;
    int i=0;
    enum card card;

    if (eq == EQUIPE1){
        while ((card=pileEq1->deck[i])!=NOTHING)
        {
            color = card2Color(card);
            numero = card - color*8;
            points+=(color==c)?getValueAtoutCard(card):getValueCard(card);
            i++;
        }
    }
    else
    {
       while ((card=pileEq2->deck[i])!=NOTHING)
        {
            color = card2Color(card);
            numero = card - color*8;
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
    pileDeck->lastcard;
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

    int color, numero, bestVal, val, bestP, bestColor;
    enum card card, bestCard;
    
    bestCard = pli[0];
    bestP = 0;
    bestColor = card2Color(bestCard);
    numero = bestCard - bestColor*8;
    bestVal = (bestColor==c)?getValueAtoutCard(bestCard):getValueCard(bestCard);
    for (int i = 1; i < PLAYERS_MAX; i++)
    {
        card = pli[i];
        color = card2Color(card);
        numero = card - color*8;
        if (color==c)//Card is Atout
        {        
            if (bestColor==c){// Both are Atout
                val = getValueAtoutCard(card);
                if (val < bestVal) break; // new Card has a smaller value thant the old
                // Both are atout and of same value -> 7 of Atout & 8 of Atout, so the biggest is the best                
                if(val = bestVal && card<=bestCard) break; 
            }
            // old card isn't Atout
               
        }
        else//Card isn't Atout
        {
            if(bestColor==c)break;//old is a Atout
            val = getValueCard(card);
            if (val < bestVal) break; // new Card has a smaller value thant the old
            // Both are atout and of same value -> 7 of Atout & 8 of Atout, so the biggest is the best                
            if(val = bestVal && card<=bestCard) break; 
        }
        bestColor = color;
        bestCard = card;
        bestP = i;
    }
    
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
    /*
    Follow suit
        If you have at least one card of the suit that was led, you must play that suit.
    If you don't have the suit led
        a) The suit led is trump
            You must play a trump if you have one.
            If a trump has already been played in the trick, you must overtrump (play a higher trump) if possible.
        b) The suit led is NOT trump
            If you have trump and your partner is not currently winning the trick:
                You must play a trump (cut).
                You must overtrump if possible.
                If your partner is winning the trick:
                You may play any card (discard).
                You may also play a trump.
    */
    int flag = 0,flag2=0;
    int i=0;
    int nbCardPli=0;
    enum card maxAtoutCard = NOTHING;
    enum card maxColorCard = NOTHING;
    enum card maxAtoutCardPli = NOTHING;
    enum card maxColorCardPli = NOTHING;
    enum card maxCardPli = NOTHING;
    enum colorCard colorCard = card2Color(card);

    // if card is in Player hand then ok
    for (i = 0; i < NB_CARD_HAND; i++)
    {
        if (players[player]->cards[i] == NOTHING) break;
        if(card == players[player]->cards[i]) flag = 1;
    }
    if(flag == 0) return false;
    flag=0;
    i=0;

    //if card is first in pli -> colorPli is now color of card. card is good
    if (pli[0]==NOTHING){
        *colorPli = colorCard;
        return true;
    }
    *colorPli = card2Color(pli[0]);
    maxAtoutCard = searchMaxCardInHand(players,player,colorAtout,colorAtout);
    maxColorCard = searchMaxCardInHand(players,player,*colorPli,colorAtout);

    //if the player have the color in is hand he must play it
    if(maxColorCard != NOTHING && card2Color(card) != *colorPli) return false;

    //Atout is asked
    if (*colorPli == colorAtout)
    {
        //if the player have the atout color in is hand he must play and overcome if possible
        // the player have the atout color in is hand he must play it -> done before
        maxAtoutCardPli = searchMaxCardInPli(pli,colorAtout,colorAtout);
        if(isOvercut(card,maxCardPli,colorAtout,*colorPli))
        {
            printf("maxAtoutCard =%s, maxCardPli=%s, card=%s\n",getNameCard(maxAtoutCard),getNameCard(maxCardPli),getNameCard(card));
            printf("overcut maxAtoutCard =%d, overcut card =%d\n",isOvercut(maxAtoutCard,maxAtoutCardPli,colorAtout,*colorPli),isOvercut(card,maxAtoutCardPli,colorAtout,*colorPli));
            printf("He is overcutting\n");
            return true;
        }
        if (isOvercut(maxAtoutCard,maxCardPli,colorAtout,*colorPli))
        {
            printf("He didn't overcut\n");
            return false;
        }
    }
    else // atout isn't asked
    {
        if(maxAtoutCard == NOTHING){
            printf("the player don't have atout in is hand\n");
            return true;
        }

        //search number of card in the pli 
        while (i < PLAYERS_MAX && pli[i] != NOTHING){i++;}
        nbCardPli = i;
        i=0;        

        //search in pli for the best card
        maxAtoutCardPli = searchMaxCardInPli(pli,colorAtout,colorAtout);
        maxColorCardPli = searchMaxCardInPli(pli,*colorPli,colorAtout);
        if (maxAtoutCardPli != NOTHING)
            maxCardPli = maxAtoutCardPli;
        else
            maxCardPli = maxColorCardPli;
        //if your partner is wining the trick
        if(nbCardPli>=2)
            if (pli[nbCardPli-2]==maxCardPli)
                return true;

        // if the player overcut
        if(isOvercut(card,maxCardPli,colorAtout,*colorPli))
        {
            printf("maxAtoutCard =%s, maxCardPli=%s, card=%s\n",getNameCard(maxAtoutCard),getNameCard(maxCardPli),getNameCard(card));
            printf("overcut maxAtoutCard =%d, overcut card =%d\n",isOvercut(maxAtoutCard,maxAtoutCardPli,colorAtout,*colorPli),isOvercut(card,maxAtoutCardPli,colorAtout,*colorPli));
            printf("He is overcutting\n");
            return true;
        }
        // if the player can overcut but don't
        if (isOvercut(maxAtoutCard,maxCardPli,colorAtout,*colorPli))
        {
            printf("He didn't overcut\n");
            return false;
        }
    }
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
    int numeroPlayerCard = maxPlayerCard - colorPlayerCard*8;
    int numeroPliCard = maxPliCard - colorPliCard*8;

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
 * @param[in] turn Round of bidding (1 or 2)
 * @param[in] startPlayer Pointer to the starting player index
 * @param[out] c Chosen trump suit (only set in round 2)
 * @return Index of player who accepted trump, or -1 if all passed
 */
int playerTurnAtout(players_t players, int turn,int * startPlayer,enum colorCard *c){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        if(turn == 1)
            {if(askTakeAtout(players, playingPlayer))return playingPlayer;}
        else
            {if(askTakeAtoutTurn2(players,playingPlayer,c))return playingPlayer;}
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
            pli[0]==NOTHING;
        }
        else players[playingPlayer]->cards[5]=dealCard(deck);
        players[playingPlayer]->cards[6]=dealCard(deck);
        players[playingPlayer]->cards[7]=dealCard(deck);
        giveCard(players, playingPlayer); 
        i++;   
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
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
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    int p;
    resetCards(deck,pileEq1,pileEq2, players);
    cardShuffle(deck);
    firstDeal(deck,players,startPlayer,pli);
    secondDeal(deck,players,startPlayer,pli);
    givePli(players,pli);
    if((p = playerTurnAtout(players,1,startPlayer,c))==-1)
        if((p = playerTurnAtout(players,2,startPlayer,c))==-1)
            return false;
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
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
        do
        {
            card = askCard(players, playingPlayer);
        } while (verifCard(players, pli, playingPlayer, *c, c, card) == false);
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
    if(turnDeal( deck, pileEq1, pileEq2, players, startPlayer, pli, c)==false)
        return false;

    for(int i =0; i < NB_CARD_DECK/NB_CARD_HAND; i++)
        turnNormal( deck, pileEq1, pileEq2, players, startPlayer, pli, c);

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
    int lastDeal = 0;
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

    while (scoreEq1 < POINT_WIN || scoreEq2 < POINT_WIN)
    {
        manche(deck,gain_Eq1,gain_Eq2,players,&startPlayer,pli,&colorAtout,&scoreEq1,&scoreEq2);
        afficherGainEq(deck,gain_Eq1,gain_Eq2);
        printf("Score Equipe 1 : %d\n",scoreEq1);
        printf("Score Equipe 2 : %d\n",scoreEq2);
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
 * @brief Displays all cards: deck and both teams' won piles
 * @param[in] pileDeck Remaining cards in deck
 * @param[in] pileEq1 Team 1's won cards
 * @param[in] pileEq2 Team 2's won cards
 */
void afficherGainEq(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2){
    int j=0;
    enum card card=NOTHING;
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
        printf("player n°%d : state=%d\n",players[i]->num,players[i]->s);
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

/**
 * @brief Main entry point of the program
 * @param[in] argc Number of command line arguments
 * @param[in] argv Array of command line argument strings
 * @return Exit status code
 */
int main(int argc, char const *argv[])
{
    pileCard_t* deck;//les cartes existante dans le deck
    int lastDeal = 0;
    pileCard_t* gain_Eq1;
    pileCard_t* gain_Eq2;
    enum card pli[PLAYERS_MAX] = {NOTHING,NOTHING,NOTHING,NOTHING};
    player_t *players[PLAYERS_MAX];
    int startPlayer=0;
    //enum colorCard colorAtout;
    //enum colorCard colorPli=NONE;

    int nbPlayer=0;
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
    
    // =============================================================
    
    initPile(&gain_Eq1);
    initPile(&gain_Eq2);
    initPile(&deck);
    //turnDeal(deck,gain_Eq1,gain_Eq2,players,&startPlayer,pli,&colorAtout);
    
    

    return 0;

}