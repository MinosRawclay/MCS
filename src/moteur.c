#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PLAYERS_MAX 4 //correspond aux règle de la bellote
#define NB_CARD_HAND 8
#define NB_CARD_DECK 32

#define STR_ROUGE_START printf("\033[31m")
#define STR_MAGENTA_START printf("\033[35m")
#define STR_VERT_START printf("\033[32m")
#define STR_BLEU_START printf("\033[34m")
#define STR_COLOR_END printf("\033[0m")







// ==================== DECLARATION ==============================================
enum card {
    H_AS, H_7, H_8, H_9, H_10, H_V, H_D, H_R,
    C_AS, C_7, C_8, C_9, C_10, C_V, C_D, C_R,
    P_AS, P_7, P_8, P_9, P_10, P_V, P_D, P_R,
    T_AS, T_7, T_8, T_9, T_10, T_V, T_D, T_R,NOTHING=-1
}card_t;
enum colorCard {H,C,P,T,NONE};
enum equipe {EQUIPE1,EQUIPE2};
enum state {FINISH,PLAYING,WAITING};
typedef struct player
{
    //socket_t sd;
    int num;
    enum state s;
    int cards[NB_CARD_HAND];
}player_t;

typedef player_t* players_t[PLAYERS_MAX] ;

typedef struct pileCard
{
    enum card deck[NB_CARD_DECK];
    int lastcard;
}pileCard_t;

typedef enum card pli_t[PLAYERS_MAX];

const int value[9] = {11, 0, 0, 0, 10, 2, 3, 4,0};
const int valueAtout[9] = {11, 0, 0, 14, 10, 20, 3, 4,0};
const char *CARD_NAMES[NB_CARD_DECK] = {
    "H_AS","H_7","H_8","H_9","H_10","H_V","H_D","H_R",
    "C_AS","C_7","C_8","C_9","C_10","C_V","C_D","C_R",
    "P_AS","P_7","P_8","P_9","P_10","P_V","P_D","P_R",
    "T_AS","T_7","T_8","T_9","T_10","T_V","T_D","T_R"
};

// ==================== FONCTION DECLARATION ==============================================

//Communication
bool addPlayer(players_t players, int *nbPlayer);
void giveCard(players_t players, int playingPlayer);
void givePli(players_t players, pli_t pli);
bool askTakeAtout(players_t players, int player);
bool askTakeAtoutTurn2(players_t players, int player, enum colorCard *c);

//Card
enum card dealCard(pileCard_t* pileDeck);
void resetCards(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2, players_t players);
int string_to_card(const char *s, enum card *out) ;
int point_of_gain(pileCard_t* pileEq1,pileCard_t* pileEq2, enum equipe eq,enum colorCard c);
void cardShuffle(pileCard_t* pileDeck) ;
bool verifColor(char color);
int betterInPli(pli_t pli, enum colorCard c);
enum colorCard card2Color(enum card card);
enum card searchMaxCardInHand(players_t players, int player, enum colorCard color);
bool verifCard(players_t players, pli_t pli, int player, enum colorCard colorAtout, enum colorCard * colorPli, enum card card);
enum card searchMaxCardInPli(pli_t pli, enum colorCard color);
bool isOvercut(enum card maxPlayerCard, enum card maxPliCard, enum colorCard colorAtout, enum colorCard colorPli);

//Game
void initPile(pileCard_t **pile);
void firstDeal(pileCard_t* deck, players_t players, int * startPlayer, pli_t pli);
int playerTurnAtout(players_t players, int turn,int * startPlayer,enum colorCard *c);
void secondDeal(pileCard_t* deck, players_t players,int* startPlayer,pli_t pli);
void thirdDeal(pileCard_t* deck, players_t players,int* startPlayer, pli_t pli, int playerTakeAtout);
bool turnDeal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorCard * c);
int nextPlayingPlayer(int* startPlayer, int nbNextPlayer);
void turnNormal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorCard * c);

//Affichage
void str_color(enum card card);
void afficherGainEq(pileCard_t* pileDeck,pileCard_t* pileEq1,pileCard_t* pileEq2);
void afficherEq(pileCard_t* pileEq);
void afficherDeck(pileCard_t* pileDeck);
void afficherPlayers(players_t players);
void afficherPli(pli_t pli);



void clearPli(pli_t pli) {
    for (int i = 0; i < PLAYERS_MAX; i++)
        pli[i] = NOTHING;
}

void clearHand(player_t *p) {
    for (int i = 0; i < NB_CARD_HAND; i++)
        p->cards[i] = NOTHING;
}

void addCard(player_t *p, enum card c, int idx) {
    p->cards[idx] = c;
}

#define TEST(name) printf("\n==== %s ====\n", name)


// ==================== COMMUNICATION =====================================================
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

//TODO --> transmet toutes les cartes du joueur au joueur
void giveCard(players_t players, int player){
    return;
}

//TODO --> transmett le pli au joueurs
void givePli(players_t players, pli_t pli){
    afficherPli(pli);
    afficherPlayers(players);
    return;
}

//TODO --> demander si tu veux accepter la carte du milieu
bool askTakeAtout(players_t players, int player){
    // if(player == 0){CLient INTERNE}
    printf("Tu prend l'atout T1 ? \n");
    printf("yes=1,no=0 :");
    char choice;
    scanf(" %c",&choice);
    if(choice == '1') return true; 
    return false;
}

//TODO --> chose atout color
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

enum card askCard(players_t players, int player){
    // if(player == 0){CLient INTERNE}
    printf("Tu met quel card ? \n");
    char choice;
    scanf(" %c",&choice);
    if(choice == '1') return true; 
    return false;
}

// ==================== CARD ==============================================================

enum card dealCard(pileCard_t* pileDeck){
    enum card c = pileDeck->deck[pileDeck->lastcard];
    pileDeck->deck[pileDeck->lastcard] = -1;
    pileDeck->lastcard++;
    return c;
}

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

int string_to_card(const char *s, enum card *out) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(s, CARD_NAMES[i]) == 0) {
            *out = (enum card)i;
            return 1;
        }
    }
    return 0;
}

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
            points+=(color==c)?valueAtout[numero]:value[numero];
            //printf("color:%d,c:%d\n",color,c);
            //printf("i:%d--%s;temp:%d;point:%d\n",i,CARD_NAMES[i],numero,points);
            //printf("val:%d;valA:%d\n",value[numero],valueAtout[numero]);
            i++;
        }
    }
    else
    {
       while ((card=pileEq2->deck[i])!=NOTHING)
        {
            color = card2Color(card);
            numero = card - color*8;
            points+=(color==c)?valueAtout[numero]:value[numero];
            i++;
        } 
    }
    return points;
}

void cardShuffle(pileCard_t* pileDeck) {
    for (int i = NB_CARD_DECK - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        enum card temp = pileDeck->deck[j];
        pileDeck->deck[j] = pileDeck->deck[i];
        pileDeck->deck[i] = temp;
    }
    pileDeck->lastcard;
}

bool verifColor(char color){
    if(color == 'H' || color == 'C' || color == 'P' || color == 'T') {
        return true;}
    return false;
}

/**
 *	\fn			int betterInPli(enum colorCard c)
 *	\brief		renvoit l'indice du joueur gagnant d'un pli
 *	\param 		
 *	\note		On trouve le meilleur du pli a partir de la  
 *	\result		paramètre sockEch modifié pour le mode DGRAM
 */
int betterInPli(pli_t pli, enum colorCard c){

    int color, numero, bestVal, val, bestP, bestColor;
    enum card card, bestCard;
    
    bestCard = pli[0];
    bestP = 0;
    bestColor = card2Color(bestCard);
    numero = bestCard - bestColor*8;
    bestVal = (bestColor==c)?valueAtout[numero]:value[numero];
    for (int i = 1; i < PLAYERS_MAX; i++)
    {
        card = pli[i];
        color = card2Color(card);
        numero = card - color*8;
        if (color==c)//Card is Atout
        {        
            if (bestColor==c){// Both are Atout
                val = valueAtout[numero];
                if (val < bestVal) break; // new Card has a smaller value thant the old
                // Both are atout and of same value -> 7 of Atout & 8 of Atout, so the biggest is the best                
                if(val = bestVal && card<=bestCard) break; 
            }
            // old card isn't Atout
               
        }
        else//Card isn't Atout
        {
            if(bestColor==c)break;//old is a Atout
            val = value[numero];
            if (val < bestVal) break; // new Card has a smaller value thant the old
            // Both are atout and of same value -> 7 of Atout & 8 of Atout, so the biggest is the best                
            if(val = bestVal && card<=bestCard) break; 
        }
        bestColor = color;
        bestCard = card;
        bestP = i;
    }
    
}

void initPile(pileCard_t **pile)
{
    *pile = malloc(sizeof(pileCard_t));
    if (*pile == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

bool verifCard(players_t players, pli_t pli, int player, enum colorCard colorAtout, enum colorCard * colorPli, enum card card){
    /*
    Follow suit
        If you have at least one card of the suit that was led, you must play that suit.
    If you don’t have the suit led
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

    printf("verifCard in hand? : player=%d, card=%s, colorAtout=%d, colorPli=%d\n",player,CARD_NAMES[card],colorAtout,*colorPli);
    // if card is in Player hand then ok
    for (i = 0; i < NB_CARD_HAND; i++)
    {
        if (players[player]->cards[i] == NOTHING) break;
        if(card == players[player]->cards[i]) flag = 1;
    }
    if(flag == 0) return false;
    flag=0;
    i=0;

    printf("card is first in pli?\n");
    //if card is first in pli -> colorPli is now color of card. card is good
    if (pli[0]==NOTHING){
        *colorPli = colorCard;
        return true;
    }
    *colorPli = card2Color(pli[0]);
    printf("colorPli : %d ,atout : %d\n",*colorPli,colorAtout);

    maxAtoutCard = searchMaxCardInHand(players,player,colorAtout);
    printf("maxAtoutCard in hand=%s==%d\n",CARD_NAMES[maxAtoutCard],maxAtoutCard);
    maxColorCard = searchMaxCardInHand(players,player,*colorPli);
    printf("maxColorCard in hand=%s==%d\n",CARD_NAMES[maxColorCard],maxColorCard);


    printf("the color in is hand?\n");
    //if the player have the color in is hand he must play it
    if(maxColorCard != NOTHING && card2Color(card) != *colorPli) return false;

    
    //Atout is asked
    if (*colorPli == colorAtout)
    {
        printf("Atout is asked\n");
        //if the player have the atout color in is hand he must play and overcome if possible
        // the player have the atout color in is hand he must play it -> done before
        
        maxAtoutCardPli = searchMaxCardInPli(pli,colorAtout);
        // if the player can overcut he must overcut
        if(isOvercut(maxAtoutCard,maxAtoutCardPli,colorAtout,*colorPli) && !isOvercut(card,maxAtoutCardPli,colorAtout,*colorPli)){
            printf("overcut maxAtoutCard =%d, overcut card =%d\n",isOvercut(maxAtoutCard,maxAtoutCardPli,colorAtout,*colorPli),isOvercut(card,maxAtoutCardPli,colorAtout,*colorPli));
            printf("He's not overcutting\n");
            return false;
        }
        printf("He overcut or can't overcut\n");
    }
    else // atout isn't asked
    {
        printf("Atout isn't asked\n");

        if(maxAtoutCard == NOTHING){
            printf("the player don't have atout in is hand\n");
            return true;
        }

        //search number of card in the pli 
        while (i < PLAYERS_MAX && pli[i] != NOTHING){i++;}
        nbCardPli = i;
        i=0;        
        printf("nbCardPli=%d\n",nbCardPli);

        //search in pli for the best card
        maxAtoutCardPli = searchMaxCardInPli(pli,colorAtout);
        maxColorCardPli = searchMaxCardInPli(pli,*colorPli);
        if (maxAtoutCardPli != NOTHING)
            maxCardPli = maxAtoutCardPli;
        else
            maxCardPli = maxColorCardPli;
        printf("maxCard in pli=%s\n",CARD_NAMES[maxCardPli]);
        printf("is partner wining the trick?\n");
        //if your partner is wining the trick
        if(nbCardPli>=2)
            if (pli[nbCardPli-2]==maxCardPli)
                return true;
        printf("partner isn't wining the trick\n");

        // if the player can overcut he must overcut
        if(isOvercut(card,maxCardPli,colorAtout,*colorPli))
        {
            printf("maxAtoutCard =%s, maxCardPli=%s, card=%s\n",CARD_NAMES[maxAtoutCard],CARD_NAMES[maxCardPli],CARD_NAMES[card]);
            printf("overcut maxAtoutCard =%d, overcut card =%d\n",isOvercut(maxAtoutCard,maxAtoutCardPli,colorAtout,*colorPli),isOvercut(card,maxAtoutCardPli,colorAtout,*colorPli));
            printf("He's not overcutting\n");
            return false;
        }
        if (isOvercut(maxAtoutCard,maxCardPli,colorAtout,*colorPli))
        printf("He overcut or can't overcut\n");

        
    }
    return true;
}

enum card searchMaxCardInHand(players_t players, int player, enum colorCard color){
    enum card maxCard = NOTHING;
    for (int i = 0; i < NB_CARD_HAND; i++)
    {
        if (players[player]->cards[i] == NOTHING) break;
        if (color == card2Color(players[player]->cards[i])) {
            if(maxCard < players[player]->cards[i]) maxCard = players[player]->cards[i];
        }
    }
    return maxCard;
}

enum card searchMaxCardInPli(pli_t pli, enum colorCard color){
    enum card maxCard = NOTHING;
    for (int i = 0; i < PLAYERS_MAX; i++)
    {
        if (pli[i] == NOTHING) break;
        if (color == card2Color(pli[i])) {
            if(maxCard < pli[i]) maxCard = pli[i];
        }
    }
    return maxCard;
}

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
        if(value[numeroPlayerCard] > value[numeroPliCard]) return true;
        return false;
    }
    //Both are Atout
    if(valueAtout[numeroPlayerCard] > valueAtout[numeroPliCard]) return true;
    return false;
}

enum colorCard card2Color(enum card card){
    if(card == NOTHING) return NONE;
    return card/8;
}

// ==================== GAME ==============================================
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

void turnNormal(pileCard_t * deck, pileCard_t* pileEq1, pileCard_t* pileEq2, players_t players, int * startPlayer, pli_t pli,enum colorCard * c){
    int i=0;
    int playingPlayer=nextPlayingPlayer(startPlayer,i);
    do
    {
          
        i++;
    } while ((playingPlayer=nextPlayingPlayer(startPlayer,i))!=*startPlayer);
}

int nextPlayingPlayer(int* startPlayer, int nbNextPlayer){
    return (*startPlayer+nbNextPlayer)%PLAYERS_MAX;
}



// ==================== AFFICHAGE =========================================
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

void afficherEq(pileCard_t* pileEq){
    int j=0;
    enum card card=NOTHING;
    while ((card=pileEq->deck[j])!=NOTHING)
    {
        str_color(card);
        printf("-%s-",CARD_NAMES[card]);
        STR_COLOR_END;
        j++;
    }
}

void afficherDeck(pileCard_t* pileDeck){
    for (int i = pileDeck->lastcard; i < NB_CARD_DECK; i++)
    {
        str_color(pileDeck->deck[i]);
        printf("-%s-",CARD_NAMES[pileDeck->deck[i]]);
        STR_COLOR_END;
        i++;
    }
    
}

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
                printf("-%s-",CARD_NAMES[players[i]->cards[j]]);
                STR_COLOR_END;
            }
        }
        printf("\n");
    }
}

void afficherPli(pli_t pli){
    printf("Pli : ");
    for (int i = 0; i < 4; i++)
    {
        if (pli[i]==NOTHING)
            {printf("-o-");}
        else{
            str_color(pli[i]);
            printf("-%s-",CARD_NAMES[pli[i]]);
            STR_COLOR_END;
        }  
    }
    printf("\n");
}
    
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
    
    TEST("Premier joueur joue n'importe quelle carte");

    clearPli(pli);
    clearHand(players[0]);

    addCard(players[0], H_7, 0);

    enum colorCard colorPli = NONE;
    STR_BLEU_START;
    printf("verifCard returns %d\n", verifCard(players, pli, 0, T, &colorPli, H_7));
    STR_COLOR_END;
    colorPli = H;

    TEST("Doit suivre la couleur demandée");

    clearPli(pli);
    clearHand(players[1]);

    pli[0] = H_10;           // cœur demandé
    colorPli = NONE;

    addCard(players[1], H_7, 0);
    addCard(players[1], C_7, 1);

    STR_BLEU_START;
    printf("verifCard returns %d\n\n", verifCard(players, pli, 1, T, &colorPli, C_7));//0
    printf("verifCard returns %d\n", verifCard(players, pli, 1, T, &colorPli, H_7));//1
    STR_COLOR_END;


    TEST("Pas la couleur, doit couper");

    clearPli(pli);
    clearHand(players[1]);

    pli[0] = H_10;
    colorPli = NONE;
    enum colorCard atout = T;

    addCard(players[1], T_7, 0);
    addCard(players[1], C_8, 1);

    STR_BLEU_START;
    printf("verifCard returns %d\n\n", verifCard(players, pli, 1, atout, &colorPli, C_8));//0
    printf("verifCard returns %d\n", verifCard(players, pli, 1, atout, &colorPli, T_7));//1
    STR_COLOR_END;



    return 0;

    //atout overcome = 1 ok
    //atout not overcome = 1 ok
    //atout is asked and not have atout = 1 ok
    //color is asked and have color but can overcut and don't overcut = 0
    //color is asked and not have color and have atout and overcut
}
